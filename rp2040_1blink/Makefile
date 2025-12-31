NAME    = c-blink
CPU     = cortex-m0plus
ARMGNU  = arm-none-eabi

AFLAGS  = --warn --fatal-warnings -mcpu=$(CPU) -g
CFLAGS  = -mcpu=$(CPU) -nostartfiles -ffreestanding -g -O0 -fpic -mthumb -c
LDFLAGS = -nostdlib

PICO_FAMILY_ADDR = 0x20001000

all: $(NAME).bin $(NAME).uf2

$(NAME)-asm.o: $(NAME)-asm.s
	$(ARMGNU)-as $(AFLAGS) $(NAME)-asm.s -o $(NAME)-asm.o

$(NAME)-main.o: $(NAME)-main.c
	$(ARMGNU)-gcc $(CFLAGS) $(NAME)-main.c -o $(NAME)-main.o

$(NAME).elf: boot_stage2.ld $(NAME)-asm.o $(NAME)-main.o
	$(ARMGNU)-ld $(LDFLAGS) -T boot_stage2.ld $(NAME)-asm.o $(NAME)-main.o -o $(NAME).elf
	$(ARMGNU)-objdump -D $(NAME).elf > $(NAME).list

$(NAME).bin: $(NAME).elf
	$(ARMGNU)-objcopy -O binary $(NAME).elf $(NAME).bin

$(NAME).uf2: $(NAME).bin
	picotool uf2 convert $(NAME).bin $(NAME).uf2 -o $(PICO_FAMILY_ADDR) --family rp2040

clean:
	rm -f *.bin *.o *.elf *.list *.uf2

