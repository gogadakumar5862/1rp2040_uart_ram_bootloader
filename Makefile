NAME    = rp2040_ram_bootloader
CPU     = cortex-m0plus
ARMGNU  = arm-none-eabi
AFLAGS  = --warn --fatal-warnings -mcpu=$(CPU) -g
LDFLAGS = -nostdlib
CFLAGS  = -mcpu=$(CPU) -mthumb -ffreestanding -g -O0 -nostartfiles -c

# Change SDK path if needed
PICO_SDK_PATH = /home/gogada/pico-sdk
PICOTOOL      = picotool

# ========= BUILD TARGETS =========

all: $(NAME).uf2

# ---------- BOOT2 ----------
boot2.bin : boot2.s memmap_boot2.ld
	$(ARMGNU)-as $(AFLAGS) boot2.s -o boot2.o
	$(ARMGNU)-ld $(LDFLAGS) -T memmap_boot2.ld boot2.o -o boot2.elf
	$(ARMGNU)-objcopy -O binary boot2.elf boot2.bin

boot2_patch.o : boot2.bin
	$(PICO_SDK_PATH)/src/rp2040/boot_stage2/pad_checksum \
		-p 256 -s 0xFFFFFFFF boot2.bin boot2_patch.s
	$(ARMGNU)-as $(AFLAGS) boot2_patch.s -o boot2_patch.o

# ---------- APPLICATION ----------
OBJS = bootloader.o ymodem.o

bootloader.o: bootloader.c
	$(ARMGNU)-gcc $(CFLAGS) bootloader.c -o bootloader.o

ymodem.o: ymodem.c
	$(ARMGNU)-gcc $(CFLAGS) ymodem.c -o ymodem.o

# ---------- LINK ----------
$(NAME).bin : memmap.ld boot2_patch.o $(OBJS)
	$(ARMGNU)-ld $(LDFLAGS) -T memmap.ld boot2_patch.o $(OBJS) -o $(NAME).elf
	$(ARMGNU)-objdump -D $(NAME).elf > $(NAME).list
	$(ARMGNU)-objcopy -O binary $(NAME).elf $(NAME).bin

# ---------- UF2 ----------
$(NAME).uf2 : $(NAME).bin
	$(PICOTOOL) uf2 convert $(NAME).bin $(NAME).uf2 \
		-o 0x10000000 --family rp2040

clean:
	rm -f *.bin *.o *.elf *.list *.uf2 boot2_patch.* boot2.bin

