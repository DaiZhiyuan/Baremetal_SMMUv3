CROSS_COMPILE := aarch64-none-elf-

CC      := $(CROSS_COMPILE)gcc
AS      := $(CROSS_COMPILE)gcc
LD      := $(CROSS_COMPILE)gcc
OBJCOPY := $(CROSS_COMPILE)objcopy

ARCHFLAGS := -march=armv8-a
ASFLAGS   := -gdwarf-3 -c $(ARCHFLAGS)
SMMU_DEBUG ?= 0

CFLAGS    := -gdwarf-3 -c $(ARCHFLAGS) -I"./include" -O1 -DDEBUG \
             -ffreestanding -ffunction-sections -fdata-sections \
             -fno-stack-protector -fno-unwind-tables

ifeq ($(SMMU_DEBUG),1)
CFLAGS += -DSMMU_DEBUG
endif
LDFLAGS   := -nostartfiles -specs=nosys.specs \
             -Wl,-T,linker.ld -Wl,--entry=start64 \
             -Wl,--gc-sections -Wl,--build-id=none

START_OBJS := startup.o vector.o gicv3.o
CORE_OBJS  := kernel.o pl011_uart.o interrupts.o smmu_test.o

TESTCASE_SRCS := $(wildcard test_case/*.c)
TESTCASE_OBJS := $(TESTCASE_SRCS:.c=.o)

OBJS := $(START_OBJS) $(CORE_OBJS) $(TESTCASE_OBJS)

vpath %.s asm
vpath %.c src

.PHONY: all clean run debug

all: image_smmu.axf image_smmu.hex

clean:
	@rm -f image_smmu.axf image_smmu.hex *.o $(TESTCASE_OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<

image_smmu.axf: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) $(OBJS) -o $@

image_smmu.hex: image_smmu.axf
	$(OBJCOPY) -O ihex $< $@

run: image_smmu.hex
	@FVP_Base_RevC-2xAEMvA -q -a ./image_smmu.hex \
		-C bp.secure_memory=false \
		-C cache_state_modelled=0 \
		-C pci.enable_device_id_checks=0 \
		--cyclelimit 50000000 \
		-C bp.pl011_uart0.unbuffered_output=1 \
		-C bp.pl011_uart0.out_file=/dev/stdout

debug:
	@$(MAKE) clean
	@$(MAKE) run SMMU_DEBUG=1
