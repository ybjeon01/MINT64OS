NASM32 = nasm
GCC32 = gcc -m32 -ffreestanding
LD32 = ld -melf_i386 -T ../elf_i386.x -nostdlib -e Main -Ttext 0x10200
OBJCOPY32 = objcopy -j .text -j .data -j .rodata -j .bss -S -O binary

OBJECTDIRECTORY = Temp
SOURCEDIRECTORY = Source

all : prepare Kernel32.bin

prepare:
	mkdir -p $(OBJECTDIRECTORY)

Kernel32.bin: $(OBJECTDIRECTORY)/EntryPoint.bin $(OBJECTDIRECTORY)/Kernel32.elf.bin
	cat $^ > $@

$(OBJECTDIRECTORY)/EntryPoint.bin: $(SOURCEDIRECTORY)/EntryPoint.s
	$(NASM32) -o $@ $<

$(OBJECTDIRECTORY)/Kernel32.elf.bin: ExecuteInternalBuild
	$(OBJCOPY32) $(OBJECTDIRECTORY)/Kernel32.elf $@

ExecuteInternalBuild: dep
	make -C $(OBJECTDIRECTORY) -f ../makefile Kernel32.elf

##########################################################
# below parts are called with cwd of Temp
##########################################################

dep:
	@echo ===    Make Dependency File    ===
	make -C $(OBJECTDIRECTORY) -f ../makefile InternalDependency
	@echo === Dependency Search Complete ===

ASSEMBLYSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.asm)
CSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.c)

InternalDependency:
	$(GCC32) -MM $(CSOURCEFILES) > Dependency.dep

ifeq (Dependency.dep, $(wildcard Dependency.dep))
include Dependency.dep
endif

ASSEMBLYOBJECTFILES = $(notdir $(patsubst %.asm, %.o, $(ASSEMBLYSOURCEFILES)))
CENTRYPOINTOBJECTFILE = Main.o
COBJECTFILES = $(subst Main.o, , $(notdir $(patsubst %.c, %.o, $(CSOURCEFILES))))

Kernel32.elf: $(CENTRYPOINTOBJECTFILE) $(COBJECTFILES) $(ASSEMBLYOBJECTFILES)
	$(LD32) -o $@ $^

%.o: ../$(SOURCEDIRECTORY)/%.c
	$(GCC32) -c $<

%.o: ../$(SOURCEDIRECTORY)/%.asm
	$(NASM32) -f elf32 -o $@ $<
	

clean:
	rm -f *.bin
	rm -f $(OBJECTDIRECTORY)/*.*


	