CC=armclang
AS=armclang
LD=armlink

ASFLAGS= -gdwarf-3 -c --target=aarch64-arm-none-eabi
CFLAGS=  -gdwarf-3 -c --target=aarch64-arm-none-eabi -I"./include" -O1

# Select build rules based on Windows or Unix
ifdef WINDIR
DONE=@if exist $(1) if exist $(2) echo Build completed.
RM=if exist $(1) del /q $(1)
SHELL=$(WINDIR)\system32\cmd.exe
else
ifdef windir
DONE=@if exist $(1) if exist $(2) echo Build completed.
RM=if exist $(1) del /q $(1)
SHELL=$(windir)\system32\cmd.exe
else
DONE=@if [ -f $(1) ]; then if [ -f $(2) ]; then echo Build completed.; fi; fi
RM=rm -f $(1)
endif
endif

TESTCASE_SRCS := $(wildcard test_case/*.c)
TESTCASE_OBJS := $(TESTCASE_SRCS:.c=.o)

all: image_smmu.axf
	$(call DONE,$(EXECUTABLE))

clean:
	@$(call RM, image_smmu.axf)
	@$(call RM, *.o)
	@$(call RM, $(TESTCASE_OBJS))

$(TESTCASE_OBJS) : %.o : %.c
	$(CC) ${CFLAGS} -o $@ -c $^

interrupts.o: ./src/interrupts.c
	$(CC) ${CFLAGS} ./src/interrupts.c

pl011_uart.o: ./src/pl011_uart.c
	$(CC) ${CFLAGS} ./src/pl011_uart.c

kernel.o: ./src/kernel.c
	$(CC) ${CFLAGS} ./src/kernel.c

gicv3.o: ./asm/gicv3.s
	$(AS) ${ASFLAGS} ./asm/gicv3.s

vector.o: ./asm/vector.s
	$(AS) ${ASFLAGS} ./asm/vector.s

startup.o: ./asm/startup.s
	$(AS) ${ASFLAGS} ./asm/startup.s

image_smmu.axf: startup.o vector.o gicv3.o kernel.o pl011_uart.o interrupts.o $(TESTCASE_OBJS) scatter.txt
	$(LD) --scatter=scatter.txt startup.o vector.o gicv3.o kernel.o pl011_uart.o interrupts.o $(TESTCASE_OBJS) -o image_smmu.axf --entry=start64

run:
	@FVP_Base_RevC-2xAEMvA -a ./image_smmu.axf -C bp.secure_memory=false -C cache_state_modelled=0
