# Baremetal_SMMUv3

This project demonstrates the use of the System Memory Management Unit (SMMU) in a baremetal environment.

memory map
==========
Peripheral|Start address| End address | Size | Security
---|---|---|---|---
Trusted ROM|0x00_0000_0000|0x00_03FF_FFFF|64MB|S
Trusted SRAM|0x00_0400_0000|0x00_0407_FFFF|512KB|S
UART0, PL011|0x00_1C09_0000|0x00_1C09_FFFF|64KB|S/NS
SMMUv3|0x00_2B40_0000|0x00_2B4F_FFFF|1MB|S/NS
GICv3 GICD|0x00_2F00_0000|0x00_2F00_FFFF|64KB|S/NS
GICv3 GITS|0x00_2F02_0000|0x00_2F03_FFFF|128KB|S/NS
GICv3 GICR|0x00_2F10_0000|0x00_2F1F_FFFF|1MB|S/NS

SPI GIC assignments
===================
IRQ ID| SPI offset | Device
---|---|---
37 | 5  | UART0, PL011.
103 | 71 | SMMUv3 non-secure combined interrupt.
104 | 72 | SMMUv3 secure combined interrupt.
105 | 73 | SMMUv3 secure event queue.
106 | 74 | SMMUv3 non-secure event queue.
107 | 75 | SMMUv3 PRI queue.
108 | 76 | SMMUv3 secure command queue sync.
109 | 77 | SMMUv3 non-secure command queue sync.
110 | 78 | SMMUv3 secure GERROR.
111 | 79 | SMMUv3 non-secure GERROR.

File list
=========
```
 <root>
├── asm
│   ├── gicv3.s
│   ├── startup.s
│   └── vector.s
├── include
│   ├── pl011_uart.h
│   ├── smmuv3.h
│   └── testcase.h
├── LICENSE
├── Makefile
├── README.md
├── scatter.txt
├── src
│   ├── interrupts.c
│   ├── kernel.c
│   └── pl011_uart.c
└── test_case
    ├── 01_dump_ird0.c
    ├── 02_dump_ird1.c
    ├── 03_dump_ird2.c
    ├── 04_dump_idr3.c
    ├── 05_dump_idr4.c
    ├── 06_dump_idr5.c
    ├── 07_dump_idr6.c
    ├── 08_dump_iidr.c
    ├── 09_dump_aidr.c
    └── 10_check_offset.c
```

Code of line
============
```
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C                               13             83             63            301
C/C++ Header                     3             19             12            215
Assembly                         3             42             45            150
make                             1             14              1             47
-------------------------------------------------------------------------------
SUM:                            20            158            121            713
-------------------------------------------------------------------------------
```

Building
========
    - Run "make DEBUG=TRUE"

> Optionally, adding "DEBUG=TRUE" results in additional logging messages being printed.


Running
=======
```
● make run
terminal_0: Listening for serial connection on port 5000
terminal_1: Listening for serial connection on port 5001
terminal_2: Listening for serial connection on port 5002
terminal_3: Listening for serial connection on port 5003

Current Exception Level: [3]

SMMU_IDR0: 0x80fe6bf
         ST_LEVEL: 1
         TERM_MODEL: 0
         STALL_MODEL: 0
         ATSRECERR: 0
         TTENDIAN: 0
         VATOS: 0
         CD2L: 1
         VMID16: 1
         VMW: 1
         PRI: 1
         ATOS: 1
         SEV: 1
         MSI: 1
         ASID16: 0
         NS1ATS: 0
         ATS: 1
         HYP: 1
         HTTU: 2
         BTM: 1
         COHACC: 1
         TTF: 3
         S1P: 1
         S2P: 1
SMMU_IDR1: 0xe739d10
         ECMDQ: 0
         TABLES_PRESET: 0
         QUEUES_PRESET: 0
         REL: 0
         ATTR_TYPES_OVR: 1
         ATTR_PERMS_OVR: 1
         CMDQS: 19
         EVENTQS: 19
         PRIQS: 19
         SSIDSIZE: 20
         SIDSIZE: 16
SMMU_IDR2: 0x0
         BA_VATOS: 0
SMMU_IDR3: 0x14
         PTWNNC: 0
         E0PD: 0
         BBML: 0
         RIL: 0
         STT: 0
         FWB: 0
         MPAM: 0
         PSS: 0
         XNX: 1
         PBHA: 0
         HAD: 1
SMMU_IDR4: 0x0 [IMPLEMENTATION_DEFINED]
SMMU_IDR5: 0x1005d
         STALL_MAX: 1
         VAX: 0
         GRAN64K: 1
         GRAN16K: 0
         GRAN4K: 1
         OAS: 5
SMMU_IDR6: 0x0
         CMDQ_CONTROL_PAGE_LOG2NUMP: 0
         CMDQ_CONTROL_PAGE_LOG2NUMQ: 0
SMMU_IIDR: 0x0
         ProductID: 0
         Variant: 0
         Revision: 0
         Implementer: 0
SMMU_AIDR: 0x1
         ArchMajorRev: 0
         ArchMinorRev: 1
SMMU_IDR0 offset:0x0
SMMU_IDR1 offset:0x4
SMMU_IDR2 offset:0x8
SMMU_IDR3 offset:0xc
SMMU_IDR4 offset:0x10
SMMU_IDR5 offset:0x14
SMMU_IIDR offset:0x18
SMMU_AIDR offset:0x1c
SMMU_IDR6 offset:0x190
=== ALL TEST CASE PASS ===
```

