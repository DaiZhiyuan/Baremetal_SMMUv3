# Baremetal SMMUv3 Test Framework

A self-contained AArch64 bare-metal test framework for the Arm SMMUv3. It runs at EL3 on the
Arm FVP Base RevC model, validates the SMMU identification and register layout, and then drives
real DMA through the SMMU to verify Stage 1, Stage 2, and nested Stage 1 + Stage 2 translation.

The project is intentionally small and deterministic: there is no OS, no SMMU driver stack, no
host test runner, and no semihosting. Everything needed to boot, print results, program the SMMU,
and check memory is contained in this repository.

```
CPU (EL3)  --->  SMMUv3TestEngine  --->  SMMUv3  --->  DRAM
                     DMA generator       translation       data
```

## Why Bare Metal?

Running the tests at EL3 gives direct control over registers, page tables, command queues, and
the test engine without interference from an operating system. This makes the suite useful for:

- checking SMMUv3 register reset values and field decoding on a specific model
- validating the hand-written SMMU register map and structure offsets
- exercising the real SMMU translation path with DMA, not just CPU-visible MMIO
- bringing up and debugging minimal SMMU programming sequences
- catching model or hardware differences early, before integrating a full driver

## What the Framework Tests

| Area | Test cases | What is checked |
|---|---|---|
| Identification | `01`-`09` | SMMU IDR0-IDR6, IIDR, and AIDR values and fields |
| Register layout | `10` | Structure member offsets and reserved padding |
| Stage 1 translation | `11` | STE + context descriptor + Stage 1 page tables, IOVA to PA |
| Stage 2 translation | `12` | STE with VTCR/VTTBR and Stage 2 page tables, IPA to PA |
| Nested translation | `13` | Stage 1 VA to IPA followed by Stage 2 IPA to PA |

Every test is an on-target assertion. The suite is considered successful only when the final
line is printed:

```text
=== ALL TEST CASE PASS ===
```

The expected register values are the reset values of the FVP Base RevC model. They are part of
the test contract; changing the model or running on different hardware requires updating those
constants intentionally.

## Translation Tests

The translation tests use the platform `SMMUv3TestEngine`, which is the DMA traffic generator
upstream of the FVP PCIe SMMU:

- User frame window: `0x2BFE0000`
- Privileged frame window: `0x2BFF0000`
- Non-secure StreamID: `0`
- Test data and translation structures: DRAM starting at `0x80000000`
- Mapping granularity: 2MB block descriptors

Each test programs the SMMU stream table, command queue, event queue, context descriptor, and
page tables. It then asks the test engine to copy one 4KB page through the SMMU and verifies the
destination data.

The three translation paths are:

```text
S1:      IOVA 0x10000000  --->  PA 0x80200000
S2:      IPA  0x20000000  --->  PA 0x80200000
Nested:  VA   0x30000000  --->  IPA 0x20000000  --->  PA 0x80200000
```

The test engine also has `RAND48` and `SUM64` workloads; this framework currently uses `MEMCPY`
because it provides a simple and observable end-to-end translation check.

## Memory Map

| Peripheral | Start address | End address | Size | Security |
|---|---|---|---|---|
| Trusted ROM | `0x00_0000_0000` | `0x00_03FF_FFFF` | 64MB | S |
| Trusted SRAM | `0x00_0400_0000` | `0x00_0407_FFFF` | 512KB | S |
| UART0, PL011 | `0x00_1C09_0000` | `0x00_1C09_FFFF` | 64KB | S/NS |
| SMMUv3 | `0x00_2B40_0000` | `0x00_2B4F_FFFF` | 1MB | S/NS |
| SMMUv3TestEngine | `0x00_2BFE_0000` | `0x00_2BFF_FFFF` | 128KB | S/NS |
| GICv3 GICD | `0x00_2F00_0000` | `0x00_2F00_FFFF` | 64KB | S/NS |
| GICv3 GITS | `0x00_2F02_0000` | `0x00_2F03_FFFF` | 128KB | S/NS |
| GICv3 GICR | `0x00_2F10_0000` | `0x00_2F1F_FFFF` | 1MB | S/NS |
| Test DRAM window | `0x00_8000_0000` | `0x00_80FF_FFFF` | 16MB | NS |

## SPI GIC Assignments

| IRQ ID | SPI offset | Device |
|---|---|---|
| 37 | 5 | UART0, PL011 |
| 103 | 71 | SMMUv3 non-secure combined interrupt |
| 104 | 72 | SMMUv3 secure combined interrupt |
| 105 | 73 | SMMUv3 secure event queue |
| 106 | 74 | SMMUv3 non-secure event queue |
| 107 | 75 | SMMUv3 PRI queue |
| 108 | 76 | SMMUv3 secure command queue sync |
| 109 | 77 | SMMUv3 non-secure command queue sync |
| 110 | 78 | SMMUv3 secure GERROR |
| 111 | 79 | SMMUv3 non-secure GERROR |

## Repository Layout

```text
<root>
├── asm
│   ├── gicv3.s          GICv3 distributor/redistributor setup
│   ├── startup.s        EL3 entry, stack, .data/.bss initialization
│   └── vector.s         AArch64 vector table
├── include
│   ├── pl011_uart.h     UART interface
│   ├── smmu_test.h      Shared SMMU translation test API
│   ├── smmuv3.h         Hand-packed SMMUv3 register map and fields
│   └── testcase.h       Test declarations
├── src
│   ├── interrupts.c     FIQ handler stub
│   ├── kernel.c         Boot flow and test scheduling
│   ├── pl011_uart.c     UART, minimal printf, syscall/assert shims
│   └── smmu_test.c      SMMU/test-engine setup and translation checks
├── test_case
│   ├── 01_dump_ird0.c
│   ├── 02_dump_ird1.c
│   ├── 03_dump_ird2.c
│   ├── 04_dump_idr3.c
│   ├── 05_dump_idr4.c
│   ├── 06_dump_idr5.c
│   ├── 07_dump_idr6.c
│   ├── 08_dump_iidr.c
│   ├── 09_dump_aidr.c
│   ├── 10_check_offset.c
│   ├── 11_s1_translation.c
│   ├── 12_s2_translation.c
│   └── 13_s1s2_translation.c
├── LICENSE
├── Makefile
├── README.md
└── linker.ld
```

## Building

Prerequisites:

- Arm GNU Toolchain for AArch64 bare-metal (`aarch64-none-elf-gcc`, `aarch64-none-elf-ld`,
  `aarch64-none-elf-objcopy`)
- `FVP_Base_RevC-2xAEMvA` for running the image

Make sure the toolchain is on `PATH`:

```bash
export PATH=/path/to/arm-gnu-toolchain/bin:$PATH
make
```

The build produces:

- `image_smmu.axf` - ELF image
- `image_smmu.hex` - Intel HEX image used by `make run`

If the toolchain is not on `PATH`, pass the prefix explicitly:

```bash
make CROSS_COMPILE=/usr/local/src/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-elf/bin/aarch64-none-elf-
```

`-DDEBUG` is defined unconditionally in the Makefile, so the detailed register dump and test
messages are always enabled.

## Running

```bash
make run
```

`make run` boots `image_smmu.hex`, runs to a fixed cycle limit, and prints UART0 output directly
to stdout. It exits cleanly after the tests finish, so it can be used in automation.

A successful run ends with:

```text
S1 translation PASS
S2 translation PASS
S1+S2 translation PASS
=== ALL TEST CASE PASS ===
```

The complete output also includes the current exception level, ID register dumps, decoded fields,
and the checked register offsets.

## Adding a Test

1. Create `test_case/NN_name.c` using the next available number.
2. Add the test function declaration to `include/testcase.h`.
3. Call the function from `main` in `src/kernel.c`.
4. Build and run with `make run`.

The Makefile wildcard picks up every `test_case/*.c` automatically. Tests that need real SMMU
translation can reuse the helpers and address layout in `src/smmu_test.c`.

## Design Notes and Limitations

- The suite targets the FVP Base RevC model and its documented reset values.
- Translation tests currently cover non-secure StreamID 0 only. The secure SMMU register bank is
  not exercised.
- The platform `SMMUv3TestEngine` control frames are used directly; the PCIe test-engine
  endpoints in the default FVP tree do not expose usable BARs.
- Translation tests poll the command queue and test-engine status instead of using interrupts.
- Only the FIQ vector is wired; IRQ, synchronous, and SError entries spin.
- UART output and assertions are self-contained; no semihosting or hosted C library I/O is used.
- There is no host-side test runner. Verification is performed by building and booting the image
  under FVP.

## License

See [LICENSE](LICENSE).
