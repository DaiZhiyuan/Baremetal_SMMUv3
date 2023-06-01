CC=armclang
AS=armclang
LD=armlink

ASFLAGS= -gdwarf-3 -c --target=aarch64-arm-none-eabi
CFLAGS=  -gdwarf-3 -c --target=aarch64-arm-none-eabi -I"./headers" -O1

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

all: image_smmu.axf
	$(call DONE,$(EXECUTABLE))

clean:
	$(call RM,*.o)
	$(call RM,image_smmu.axf)

main.o: ./src/main.c
	$(CC) ${CFLAGS} ./src/main.c

startup.o: ./asm/startup.s
	$(AS) ${ASFLAGS} ./asm/startup.s

image_smmu.axf: startup.o main.o scatter.txt
	$(LD) --scatter=scatter.txt startup.o main.o -o image_smmu.axf --entry=start64

run:
	FVP_Base_RevC-2xAEMvA -a ./image_smmu.axf -C bp.secure_memory=false -C cache_state_modelled=0
