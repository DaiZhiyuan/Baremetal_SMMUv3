#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#include "smmu_test.h"
#include "smmuv3.h"

#ifdef SMMU_DEBUG
#define smmu_dbg(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define smmu_dbg(fmt, ...) do { } while (0)
#endif

#define SMMU_PAGE1_BASE (SMMU_BASE_ADDR + 0x10000UL)

#define STRTAB_PA 0x80000000UL
#define CMDQ_PA 0x80004000UL
#define EVTQ_PA 0x80005000UL
#define CD_PA 0x80006000UL
#define S1_L1_PA 0x80007000UL
#define S1_L2_PA 0x80008000UL
#define S2_L1_PA 0x80009000UL
#define S2_L2_PA 0x8000a000UL
#define NESTED_S1_L1_PA 0x8000b000UL
#define NESTED_S1_L2_PA 0x8000c000UL
#define S2_META_L2_PA 0x8000d000UL
#define DATA_PA 0x80200000UL

#define TEST_SID 0
#define TEST_SIZE 0x1000
#define S1_IOVA 0x10000000UL
#define S2_IPA 0x20000000UL
#define NESTED_IOVA 0x30000000UL

/*
 * SMMUv3TestEngine control frames (platform instance).
 *
 * The engine exposes 64KB of user frames followed by 64KB of privileged
 * frames. Each frame is 128 bytes, so frame N lives at:
 *   user:       TE_USER_BASE + N * 128
 *   privileged: TE_PRIV_BASE + N * 128
 *
 * These tests only use frame 0.
 */
#define TE_USER_BASE 0x2bfe0000UL
#define TE_PRIV_BASE 0x2bff0000UL

/* Values written to the user-frame cmd field and read back as status. */
#define TE_ENGINE_HALTED 1
#define TE_ENGINE_MEMCPY 2
#define TE_ENGINE_ERROR 0xffffffffU

/* Privileged-frame pctrl bits: bit0 = non-secure, bit8 = ATS, bit9 = PRI. */
#define TE_PCTRL_NS 0x1

/* Privileged-frame substreamid value meaning "no substream". */
#define TE_NO_SUBSTREAMID 0xffffffffU

/*
 * Transaction attributes are packed as source[15:0] and destination[31:16].
 * 0x42ff is normal WB read/write allocate, inner-shareable, non-secure.
 */
#define TE_ATTR_WBRAWA_SH 0x42ffU
#define TE_ATTR_BOTH (TE_ATTR_WBRAWA_SH | (TE_ATTR_WBRAWA_SH << 16))

#define TABLE_ADDR_MASK 0x0000fffffffff000ULL
#define BLOCK_ADDR_MASK 0x0000ffffffe00000ULL
#define CD_PTR_MASK 0x000fffffffffffc0ULL
#define TTB_MASK 0x000ffffffffffff0ULL

/*
 * User frame, 128 bytes. Field offsets are fixed by the engine:
 *
 *   0x00 cmd:        command/status (HALTED, MEMCPY, ERROR, ...)
 *   0x04 uctrl:      control bits, bit0 = MSI aborted, [31:16] = rate limit
 *   0x08 launched:   number of transactions launched
 *   0x0c returned:   number of transactions returned
 *   0x10 msiaddress: MSI enable/doorbell (0 disables MSI for this test)
 *   0x18 msidata:    MSI payload
 *   0x1c msiattr:    MSI transaction attributes
 *   0x20 attributes: source attributes [15:0], destination [31:16]
 *   0x24 seed:       RNG seed
 *   0x28 begin:      source address/IOVA, inclusive
 *   0x30 end_incl:   source end address/IOVA, inclusive
 *   0x38 stride:     access stride used by the work engine
 *   0x40 udata[0]:   destination address/IOVA for MEMCPY
 *   0x48..0x78:      remaining engine-specific user data
 *
 * The rest of the frame must only be written while cmd is HALTED, ERROR,
 * or FRAME_MISCONFIGURED.
 */
struct te_uframe {
    volatile uint32_t cmd;
    volatile uint32_t uctrl;
    volatile uint32_t launched;
    volatile uint32_t returned;
    volatile uint64_t msiaddress;
    volatile uint32_t msidata;
    volatile uint32_t msiattr;
    volatile uint32_t attributes;
    volatile uint32_t seed;
    volatile uint64_t begin;
    volatile uint64_t end_incl;
    volatile uint64_t stride;
    volatile uint64_t udata[8];
};

/*
 * Privileged frame, 128 bytes. Field offsets are fixed by the engine:
 *
 *   0x00 pctrl:      bit0 = NS, bit8 = ATS enable, bit9 = PRI enable
 *   0x04 downstream_port_index: ignored by this test
 *   0x08 streamid:   StreamID used for platform transactions (ignored for PCIe)
 *   0x0c substreamid: SubstreamID/PASID, or TE_NO_SUBSTREAMID
 *   0x10 pdata[14]:  reserved/private engine data
 */
struct te_pframe {
    volatile uint32_t pctrl;
    volatile uint32_t downstream_port_index;
    volatile uint32_t streamid;
    volatile uint32_t substreamid;
    volatile uint64_t pdata[14];
};

static struct smmuv3 *smmu = (struct smmuv3 *)SMMU_BASE_ADDR;
static uint32_t cmdq_prod;

static void dsb_sy(void)
{
    __asm__ volatile("dsb sy" ::: "memory");
}

static void isb(void)
{
    __asm__ volatile("isb" ::: "memory");
}

static void mem_zero(void *addr, uint32_t size)
{
    volatile uint64_t *p = (volatile uint64_t *)addr;
    uint32_t i;

    for (i = 0; i < size / 8; i++)
        p[i] = 0;
}

static void write_cr0(uint32_t val)
{
    uint32_t i;

    smmu->SMMU_CR0 = val;

    for (i = 0; i < 1000000; i++) {
        if (smmu->SMMU_CR0ACK == val)
            return;
    }

    printf("SMMU CR0ACK timeout val=%x ack=%x\n", val, smmu->SMMU_CR0ACK);
    assert(0);
}

/*
 * Command queue: 256 entries of 16 bytes. The producer index is 9 bits
 * (index plus wrap bit); the low 8 bits select the ring slot and the wrap
 * bit makes the producer/consumer comparison unambiguous after wrap-around.
 */
static void cmdq_issue(uint64_t d0, uint64_t d1)
{
    volatile uint64_t *q = (volatile uint64_t *)CMDQ_PA;
    uint32_t idx = cmdq_prod & 0xff;

    q[idx * 2] = d0;
    q[idx * 2 + 1] = d1;

    smmu_dbg("CMDQ[%u] = %lx %lx\n", idx, (unsigned long)d0,
             (unsigned long)d1);

    cmdq_prod++;
    dsb_sy();
    smmu->SMMU_CMDQ_PROD = cmdq_prod;
}

static void cmdq_wait(uint32_t target)
{
    uint32_t i;

    for (i = 0; i < 1000000; i++) {
        if ((smmu->SMMU_CMDQ_CONS & 0x1ff) == (target & 0x1ff))
            return;
    }

    printf("SMMU CMDQ timeout cons=%x prod=%x gerr=%x\n",
           smmu->SMMU_CMDQ_CONS, target, smmu->SMMU_GERROR);
    assert(0);
}

/*
 * CMD_SYNC opcode is 0x46. CS=SEV (2) asks for a completion event, MSH=ISH
 * and the memory attribute are included for completeness. Because this
 * bare-metal test does not use interrupts, completion is observed by polling
 * the consumer index after publishing the producer index.
 */
static void cmdq_sync(void)
{
    uint64_t d0 = 0x46 | (2ULL << 12) | (3ULL << 22) | (0xfULL << 24);

    cmdq_issue(d0, 0);
    cmdq_wait(cmdq_prod);
}

/*
 * CFGI_ALL invalidates all cached stream-table/context-descriptor state.
 * CFGI_STE invalidates the entry for one StreamID after its STE is updated.
 * TLBI_NSNH_ALL invalidates all non-secure, non-hypervisor TLB entries.
 */
static void smmu_cfgi_all(void)
{
    smmu_dbg("SMMU: CFGI_ALL\n");
    cmdq_issue(0x4, 31);
    cmdq_sync();
}

static void smmu_cfgi_ste(uint32_t sid)
{
    smmu_dbg("SMMU: CFGI_STE sid=%u\n", sid);
    cmdq_issue(0x3 | ((uint64_t)sid << 32), 1);
    cmdq_sync();
}

static void smmu_tlbi_nsnh_all(void)
{
    smmu_dbg("SMMU: TLBI_NSNH_ALL\n");
    cmdq_issue(0x30, 0);
    cmdq_sync();
}

static void smmu_page1_write32(uint32_t off, uint32_t val)
{
    *(volatile uint32_t *)(SMMU_PAGE1_BASE + off) = val;
}

/*
 * Bring the non-secure SMMU register bank to a known state without enabling
 * translation yet:
 *
 * - CR0=0 disables the SMMU and all queues; CR0ACK must follow CR0.
 * - CR1 selects inner-shareable, write-back table and queue walks.
 * - CR2=0 leaves E2H off and disables PTM/RECINVSID.
 * - The stream table is linear with 256 entries (LOG2SIZE=8).
 * - The command queue is 256 entries (LOG2SIZE=8), 4KB aligned.
 * - CMDQEN is enabled first so CFGI_ALL and TLBI_NSNH_ALL can be issued
 *   while translation is still disabled.
 */
static void smmu_hw_init(void)
{
    smmu_dbg("SMMU: reset and configure non-secure register bank\n");
    write_cr0(0);

    smmu->SMMU_IRQ_CTRL = 0;
    smmu->SMMU_CR1 = 0xd75;
    smmu->SMMU_CR2 = 0;
    smmu_dbg("SMMU: CR1=%x CR2=%x\n", smmu->SMMU_CR1, smmu->SMMU_CR2);

    mem_zero((void *)STRTAB_PA, 256 * 64);
    mem_zero((void *)CMDQ_PA, 0x1000);
    mem_zero((void *)EVTQ_PA, 0x1000);

    smmu->SMMU_STRTAB_BASE = STRTAB_PA;
    smmu->SMMU_STRTAB_BASE_CFG = 8;
    smmu_dbg("SMMU: STRTAB_BASE=%lx CFG=%x\n",
             (unsigned long)STRTAB_PA, smmu->SMMU_STRTAB_BASE_CFG);

    smmu->SMMU_CMDQ_BASE = CMDQ_PA | (1ULL << 62) | 8;
    smmu->SMMU_CMDQ_PROD = 0;
    smmu->SMMU_CMDQ_CONS = 0;
    cmdq_prod = 0;
    smmu_dbg("SMMU: CMDQ_BASE=%lx\n",
             (unsigned long)smmu->SMMU_CMDQ_BASE);

    write_cr0(0x8);
    smmu_dbg("SMMU: CMDQ enabled\n");

    smmu_cfgi_all();
    smmu_tlbi_nsnh_all();
}

/*
 * Enable the event queue and translation. EVTQ_BASE uses the same 4KB
 * queue layout as CMDQ, with LOG2SIZE=7 (128 entries of 32 bytes). The
 * event-queue producer/consumer registers are in SMMU register page 1.
 */
static void smmu_enable(void)
{
    smmu->SMMU_EVENTQ_BASE = EVTQ_PA | (1ULL << 62) | 7;
    smmu_page1_write32(0xa8, 0);
    smmu_page1_write32(0xac, 0);

    smmu_dbg("SMMU: EVENTQ_BASE=%lx\n",
             (unsigned long)smmu->SMMU_EVENTQ_BASE);
    write_cr0(0xd);
    smmu_dbg("SMMU: CR0=%x (CMDQ+EVTQ+SMMU enabled)\n", smmu->SMMU_CR0ACK);
    dsb_sy();
    isb();
}

/*
 * AArch64 VMSA table descriptor: bits[1:0]=11 and the next-level table
 * address in bits[47:12]. Both levels are 4KB tables with 512 entries.
 */
static void set_table_desc(uint64_t *tbl, uint32_t idx, uint64_t next_pa)
{
    tbl[idx] = 0x3 | (next_pa & TABLE_ADDR_MASK);
}

/*
 * Stage 1 level-2 block descriptor:
 * - bits[1:0]=01 selects a 2MB block
 * - AF=1 marks the block as accessed
 * - SH=3 selects inner-shareable memory
 * - AP[2:1]=01 permits unprivileged read/write (the test engine is a DMA
 *   master and its transactions are not privileged)
 * - AttrIndx=0 selects MAIR attribute 0 (normal write-back)
 */
static void set_s1_block(uint64_t *tbl, uint32_t idx, uint64_t pa)
{
    tbl[idx] = 0x1 | (pa & BLOCK_ADDR_MASK) |
               (1ULL << 6) | (3ULL << 8) | (1ULL << 10);
}

/*
 * Stage 2 level-2 block descriptor:
 * - MemAttr[3:0]=0xf selects normal write-back read/write-allocate
 * - S2AP[1:0]=11 permits read and write
 * - SH=3 selects inner-shareable memory
 * - AF=1 marks the block as accessed
 */
static void set_s2_block(uint64_t *tbl, uint32_t idx, uint64_t pa)
{
    tbl[idx] = 0x1 | (pa & BLOCK_ADDR_MASK) |
               (0xfULL << 2) | (3ULL << 6) | (3ULL << 8) | (1ULL << 10);
}

/*
 * Build a minimal Stage 1 walk for a 32-bit VA using a 4KB granule:
 * L1 entry -> L2 table, L2 entry -> one 2MB block. The CD TCR.T0SZ value
 * used below (32) makes the walk start at level 1.
 */
static void setup_s1(uint64_t va, uint64_t pa, uint64_t l1_pa, uint64_t l2_pa)
{
    uint64_t *l1 = (uint64_t *)l1_pa;
    uint64_t *l2 = (uint64_t *)l2_pa;
    uint32_t l1_idx = (va >> 30) & 0x1ff;
    uint32_t l2_idx = (va >> 21) & 0x1ff;

    mem_zero(l1, 0x1000);
    mem_zero(l2, 0x1000);

    set_table_desc(l1, l1_idx, l2_pa);
    set_s1_block(l2, l2_idx, pa);
    dsb_sy();

    smmu_dbg("S1 PT: VA=%lx L1[%u]=%lx L2[%u]=%lx -> PA=%lx\n",
             (unsigned long)va, l1_idx, (unsigned long)l1[l1_idx],
             l2_idx, (unsigned long)l2[l2_idx], (unsigned long)pa);
}

/*
 * Build a minimal Stage 2 walk for a 32-bit IPA using a 4KB granule:
 * L1 entry -> L2 table, L2 entry -> one 2MB block. STE.VTCR.S2T0SZ=32
 * and S2SL0 selects level 1 as the starting level.
 */
static void setup_s2(uint64_t ipa, uint64_t pa, uint64_t l1_pa, uint64_t l2_pa)
{
    uint64_t *l1 = (uint64_t *)l1_pa;
    uint64_t *l2 = (uint64_t *)l2_pa;
    uint32_t l1_idx = (ipa >> 30) & 0x1ff;
    uint32_t l2_idx = (ipa >> 21) & 0x1ff;

    mem_zero(l1, 0x1000);
    mem_zero(l2, 0x1000);

    set_table_desc(l1, l1_idx, l2_pa);
    set_s2_block(l2, l2_idx, pa);
    dsb_sy();

    smmu_dbg("S2 PT: IPA=%lx L1[%u]=%lx L2[%u]=%lx -> PA=%lx\n",
             (unsigned long)ipa, l1_idx, (unsigned long)l1[l1_idx],
             l2_idx, (unsigned long)l2[l2_idx], (unsigned long)pa);
}

/*
 * Nested translation needs two Stage 2 mappings:
 * - the data IPA maps to the test DRAM page
 * - the metadata IPA identity-maps the DRAM block containing the Stage 1
 *   CD and page tables, because in nested mode the CD pointer and Stage 1
 *   table addresses are IPAs and must be translated by Stage 2 as well.
 */
static void setup_s2_nested(uint64_t ipa, uint64_t pa, uint64_t meta_ipa,
                            uint64_t meta_pa, uint64_t l1_pa, uint64_t l2_pa,
                            uint64_t meta_l2_pa)
{
    uint64_t *l1 = (uint64_t *)l1_pa;
    uint64_t *l2 = (uint64_t *)l2_pa;
    uint64_t *meta_l2 = (uint64_t *)meta_l2_pa;
    uint32_t ipa_l1_idx = (ipa >> 30) & 0x1ff;
    uint32_t ipa_l2_idx = (ipa >> 21) & 0x1ff;
    uint32_t meta_l1_idx = (meta_ipa >> 30) & 0x1ff;
    uint32_t meta_l2_idx = (meta_ipa >> 21) & 0x1ff;

    mem_zero(l1, 0x1000);
    mem_zero(l2, 0x1000);
    mem_zero(meta_l2, 0x1000);

    set_table_desc(l1, ipa_l1_idx, l2_pa);
    set_s2_block(l2, ipa_l2_idx, pa);

    set_table_desc(l1, meta_l1_idx, meta_l2_pa);
    set_s2_block(meta_l2, meta_l2_idx, meta_pa);
    dsb_sy();

    smmu_dbg("S2 nested: IPA=%lx -> PA=%lx, meta IPA=%lx -> PA=%lx\n",
             (unsigned long)ipa, (unsigned long)pa,
             (unsigned long)meta_ipa, (unsigned long)meta_pa);
    smmu_dbg("S2 nested: L1[%u]=%lx L2[%u]=%lx L1[%u]=%lx L2[%u]=%lx\n",
             ipa_l1_idx, (unsigned long)l1[ipa_l1_idx],
             ipa_l2_idx, (unsigned long)l2[ipa_l2_idx],
             meta_l1_idx, (unsigned long)l1[meta_l1_idx],
             meta_l2_idx, (unsigned long)meta_l2[meta_l2_idx]);
}

/*
 * Context descriptor for Stage 1 translation:
 * - CD[0] holds TCR: T0SZ=32 (4GB VA), TG0=4KB, inner/outer WB cacheable,
 *   inner-shareable walk, EPD1=1 (TTBR1 disabled), V=1, IPS=48-bit,
 *   AA64=1, R=1 (reads allowed), A=1 (Access flag updates), ASET=1, ASID=0.
 * - CD[1] holds TTBR0: the physical address of the level-1 table.
 * - CD[3] holds MAIR: attribute 0 is normal write-back read/write-allocate.
 */
static void write_cd(uint64_t l1_pa)
{
    volatile uint64_t *cd = (volatile uint64_t *)CD_PA;
    uint64_t cd0;

    cd0 = 32ULL;
    cd0 |= (1ULL << 8);
    cd0 |= (1ULL << 10);
    cd0 |= (3ULL << 12);
    cd0 |= (1ULL << 30);
    cd0 |= (1ULL << 31);
    cd0 |= (5ULL << 32);
    cd0 |= (1ULL << 41);
    cd0 |= (1ULL << 45);
    cd0 |= (1ULL << 46);
    cd0 |= (1ULL << 47);

    mem_zero((void *)CD_PA, 64);
    cd[0] = cd0;
    cd[1] = l1_pa & TTB_MASK;
    cd[3] = 0xff;
    dsb_sy();

    smmu_dbg("CD: [0]=%lx [1]=%lx [3]=%lx\n",
             (unsigned long)cd[0], (unsigned long)cd[1],
             (unsigned long)cd[3]);
}

/*
 * STE[2] for Stage 2:
 * - VMID=1 (VMID 0 is reserved for Stage 2 bypass)
 * - VTCR: S2T0SZ=32, S2SL0=1 (level 1 start), IRGN/ORGN write-back,
 *   SH=inner-shareable, TG=4KB, PS=48-bit
 * - S2AA64=1, S2PTW=1 (page-table walk cache), S2R=1 (read allocate)
 */
static uint64_t s2_ste_d2(void)
{
    uint64_t vtcr;

    vtcr = 32ULL;
    vtcr |= (1ULL << 6);
    vtcr |= (1ULL << 8);
    vtcr |= (1ULL << 10);
    vtcr |= (3ULL << 12);
    vtcr |= (5ULL << 16);

    return 1ULL | (vtcr << 32) | (1ULL << 51) | (1ULL << 54) |
           (1ULL << 58);
}

/*
 * Each STE is 64 bytes (eight 64-bit words) in the linear stream table.
 * This minimal setup only uses words 0-3; the remaining words are cleared.
 * Software must make the STE visible and then issue CFGI_STE before traffic
 * is allowed to use it.
 */
static void write_ste(uint32_t sid, uint64_t d0, uint64_t d1, uint64_t d2,
                      uint64_t d3)
{
    volatile uint64_t *ste = (volatile uint64_t *)(STRTAB_PA + sid * 64);
    uint32_t i;

    for (i = 0; i < 8; i++)
        ste[i] = 0;

    ste[0] = d0;
    ste[1] = d1;
    ste[2] = d2;
    ste[3] = d3;
    dsb_sy();

    smmu_dbg("STE[%u]: %lx %lx %lx %lx\n", sid,
             (unsigned long)ste[0], (unsigned long)ste[1],
             (unsigned long)ste[2], (unsigned long)ste[3]);
}

/*
 * Stage 1 STE:
 * - STE[0]: V=1, CFG=5 (S1 translate) or 7 (nested), S1FMT=linear,
 *   S1CDMAX=0 (no substreams), S1CTXPTR points at the context descriptor.
 * - STE[1]: S1DSS=SSID0, S1CIR/S1COR/S1CSH select write-back inner-shareable
 *   descriptor walks.
 * - Nested mode also programs the Stage 2 fields in STE[2]/STE[3].
 */
static void write_s1_ste(uint32_t sid, uint64_t cd_pa, int nested)
{
    uint64_t d0;
    uint64_t d1;
    uint64_t d2 = 0;
    uint64_t d3 = 0;

    d0 = 1ULL | ((nested ? 7ULL : 5ULL) << 1) |
         (cd_pa & CD_PTR_MASK);
    d1 = 2ULL | (1ULL << 2) | (1ULL << 4) | (3ULL << 6);

    if (nested) {
        d2 = s2_ste_d2();
        d3 = S2_L1_PA & TTB_MASK;
    }

    write_ste(sid, d0, d1, d2, d3);
}

/*
 * Stage 2-only STE:
 * - STE[0]: V=1, CFG=6 (S2 translate)
 * - STE[1]: SHCFG=incoming for attribute override
 * - STE[2]/STE[3]: VTCR/VMID and VTTBR (Stage 2 table base)
 */
static void write_s2_ste(uint32_t sid)
{
    uint64_t d0 = 1ULL | (6ULL << 1);
    uint64_t d1 = 1ULL << 44;

    write_ste(sid, d0, d1, s2_ste_d2(), S2_L1_PA & TTB_MASK);
}

static void prepare_data(uint64_t pa)
{
    volatile uint64_t *src = (volatile uint64_t *)pa;
    volatile uint64_t *dst = (volatile uint64_t *)(pa + TEST_SIZE);
    uint32_t i;

    for (i = 0; i < TEST_SIZE / 8; i++) {
        src[i] = 0x5a5a000000000000ULL | i;
        dst[i] = 0;
    }

    dsb_sy();
}

static void verify_data(uint64_t pa)
{
    volatile uint64_t *src = (volatile uint64_t *)pa;
    volatile uint64_t *dst = (volatile uint64_t *)(pa + TEST_SIZE);
    uint32_t i;

    for (i = 0; i < TEST_SIZE / 8; i++)
        assert(dst[i] == src[i]);
}

static void te_run_memcpy(uint32_t sid, uint64_t src, uint64_t dst,
                          uint32_t size)
{
    volatile struct te_uframe *uf = (volatile struct te_uframe *)TE_USER_BASE;
    volatile struct te_pframe *pf = (volatile struct te_pframe *)TE_PRIV_BASE;
    uint32_t i;

    /*
     * HALTED is the only state in which the rest of the frame is writable.
     * Program the privileged frame first to select non-secure traffic,
     * the platform StreamID, and "no substream".
     */
    uf->cmd = TE_ENGINE_HALTED;
    pf->pctrl = TE_PCTRL_NS;
    pf->downstream_port_index = 0;
    pf->streamid = sid;
    pf->substreamid = TE_NO_SUBSTREAMID;

    /*
     * Disable MSI (msiaddress = 0) and poll cmd instead. The attributes
     * apply the same normal, non-secure memory type to source and
     * destination. begin/end_incl describe the source range and udata[0]
     * is the destination for MEMCPY.
     */
    uf->uctrl = 0;
    uf->msiaddress = 0;
    uf->msidata = 0;
    uf->msiattr = 0;
    uf->attributes = TE_ATTR_BOTH;
    uf->seed = 0;
    uf->begin = src;
    uf->end_incl = src + size - 1;
    uf->stride = 1;
    uf->udata[0] = dst;

    for (i = 1; i < 8; i++)
        uf->udata[i] = 0;

    smmu_dbg("TE: pctrl=%x sid=%u attr=%x src=%lx dst=%lx size=%x\n",
             pf->pctrl, pf->streamid, uf->attributes,
             (unsigned long)uf->begin, (unsigned long)uf->udata[0], size);

    /*
     * Writing MEMCPY starts the engine. On success cmd returns to HALTED;
     * on failure it becomes ERROR or FRAME_MISCONFIGURED.
     */
    dsb_sy();
    uf->cmd = TE_ENGINE_MEMCPY;
    dsb_sy();

    for (i = 0; i < 1000000; i++) {
        if (uf->cmd != TE_ENGINE_MEMCPY)
            break;
    }

    smmu_dbg("TE: status=%x launched=%x returned=%x\n",
             uf->cmd, uf->launched, uf->returned);

    if (uf->cmd != TE_ENGINE_HALTED) {
        volatile uint64_t *evt = (volatile uint64_t *)EVTQ_PA;
        volatile uint32_t *page1 = (volatile uint32_t *)SMMU_PAGE1_BASE;

        printf("TE failed cmd=%x uctrl=%x launched=%x returned=%x\n",
               uf->cmd, uf->uctrl, uf->launched, uf->returned);
        printf("TE gerr=%x gerrn=%x statusr=%x cons=%x\n",
               smmu->SMMU_GERROR, smmu->SMMU_GERRORN, smmu->SMMU_STATUSR,
               smmu->SMMU_CMDQ_CONS);
        printf("EVTQ prod=%x cons=%x\n", page1[0xa8 / 4], page1[0xac / 4]);
        printf("EVT0=%lx %lx %lx %lx\n", evt[0], evt[1], evt[2], evt[3]);
        printf("EVT1=%lx %lx %lx %lx\n", evt[4], evt[5], evt[6], evt[7]);
        assert(0);
    }
}

void smmu_test_run(enum smmu_test_mode mode)
{
    uint64_t src_iova;
    uint64_t dst_iova;

    smmu_dbg("\n=== SMMU translation test mode %u ===\n",
             (unsigned int)mode);

    smmu_hw_init();
    prepare_data(DATA_PA);

    if (mode == SMMU_TEST_MODE_S1) {
        setup_s1(S1_IOVA, DATA_PA, S1_L1_PA, S1_L2_PA);
        write_cd(S1_L1_PA);
        write_s1_ste(TEST_SID, CD_PA, 0);
        src_iova = S1_IOVA;
        dst_iova = S1_IOVA + TEST_SIZE;
    } else if (mode == SMMU_TEST_MODE_S2) {
        setup_s2(S2_IPA, DATA_PA, S2_L1_PA, S2_L2_PA);
        write_s2_ste(TEST_SID);
        src_iova = S2_IPA;
        dst_iova = S2_IPA + TEST_SIZE;
    } else {
        setup_s1(NESTED_IOVA, S2_IPA, NESTED_S1_L1_PA, NESTED_S1_L2_PA);
        setup_s2_nested(S2_IPA, DATA_PA, 0x80000000UL, 0x80000000UL,
                        S2_L1_PA, S2_L2_PA, S2_META_L2_PA);
        write_cd(NESTED_S1_L1_PA);
        write_s1_ste(TEST_SID, CD_PA, 1);
        src_iova = NESTED_IOVA;
        dst_iova = NESTED_IOVA + TEST_SIZE;
    }

    smmu_dbg("Test: SID=%u src=%lx dst=%lx PA=%lx size=%x\n",
             TEST_SID, (unsigned long)src_iova, (unsigned long)dst_iova,
             (unsigned long)DATA_PA, TEST_SIZE);

    dsb_sy();
    smmu_cfgi_ste(TEST_SID);
    smmu_tlbi_nsnh_all();
    smmu_enable();

    te_run_memcpy(TEST_SID, src_iova, dst_iova, TEST_SIZE);
    verify_data(DATA_PA);

    if (mode == SMMU_TEST_MODE_S1)
        printf("S1 translation PASS\n");
    else if (mode == SMMU_TEST_MODE_S2)
        printf("S2 translation PASS\n");
    else
        printf("S1+S2 translation PASS\n");
}
