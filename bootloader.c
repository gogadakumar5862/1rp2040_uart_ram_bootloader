#include <stdint.h>

#define PUT32(a,v) (*(volatile unsigned int*)(a)) = (v)
#define GET32(a)   (*(volatile unsigned int*)(a))

#define APP_ADDR 0x20000000
#define MAX_FW   (200 * 1024)

/******** UART + CLOCK HW DEFINITIONS *********/

#define RESETS_BASE 0x4000C000
#define RESETS_RESET       (RESETS_BASE + 0x0)
#define RESETS_RESET_DONE  (RESETS_BASE + 0x8)

#define IO_BANK0_BASE 0x40014000
#define IO_BANK0_GPIO00_CTRL (IO_BANK0_BASE + 0x04)
#define IO_BANK0_GPIO01_CTRL (IO_BANK0_BASE + 0x0C)

#define XOSC_BASE 0x40024000
#define XOSC_CTRL   (XOSC_BASE + 0x00)
#define XOSC_STATUS (XOSC_BASE + 0x04)
#define XOSC_STARTUP (XOSC_BASE + 0x0C)

#define CLOCKS_BASE 0x40008000
#define CLK_REF_CTRL (CLOCKS_BASE + 0x30)
#define CLK_REF_DIV  (CLOCKS_BASE + 0x34)
#define CLK_SYS_CTRL (CLOCKS_BASE + 0x3C)
#define CLK_PERI_CTRL (CLOCKS_BASE + 0x48)

#define UART0_BASE 0x40034000
#define UART_DR   (UART0_BASE + 0x00)
#define UART_FR   (UART0_BASE + 0x18)
#define UART_IBRD (UART0_BASE + 0x24)
#define UART_FBRD (UART0_BASE + 0x28)
#define UART_LCRH (UART0_BASE + 0x2C)
#define UART_CR   (UART0_BASE + 0x30)



// SIO
#define SIO_BASE        0xD0000000
#define SIO_GPIO_OUT    (SIO_BASE + 0x10)
#define SIO_GPIO_OE_SET (SIO_BASE + 0x24)

// IO BANK GPIO25 Control
#define IO_BANK0_GPIO25_CTRL (IO_BANK0_BASE + 0xCC)


static void led_init(void)
{
    // Set GPIO25 to SIO function (FUNC = 5)
    PUT32(IO_BANK0_GPIO25_CTRL, 5);

    // Enable output on GPIO25
    PUT32(SIO_GPIO_OE_SET, (1 << 25));
}

static void led_toggle(void)
{
    // XOR output bit
    PUT32(SIO_BASE + 0x1C, (1 << 25));
}

static void delay(void)
{
    for(volatile int i = 0; i < 500000; i++);
}



/******** UART FUNCTIONS *********/

void uart_putc(uint8_t c)
{
    while (GET32(UART_FR) & (1 << 5));
    PUT32(UART_DR, c);
}

uint8_t uart_getc_blocking(void)
{
    while (GET32(UART_FR) & (1 << 4));
    return GET32(UART_DR) & 0xFF;
}

void uart_puts(char *s)
{
    while (*s) uart_putc(*s++);
}

/******** CLOCK + UART INIT *********/

static void clocks_init(void)
{
    PUT32(XOSC_CTRL, 0xAA0);
    PUT32(XOSC_STARTUP, 0xC4);
    PUT32(XOSC_CTRL, 0xFAB000);
    while (!(GET32(XOSC_STATUS) & 0x80000000));

    PUT32(CLK_REF_CTRL, 2);
    PUT32(CLK_SYS_CTRL, 0);
	PUT32(CLK_REF_DIV, ( 1 << 8 ));
    PUT32(CLK_PERI_CTRL, (1 << 11) | (4 << 5));
}

static void reset_peripherals(void)
{
	// Assert reset
    PUT32(RESETS_RESET, (1 << 5) | (1 << 8) | (1 << 22));
    PUT32(RESETS_RESET + 0x3000, (1 << 5) | (1 << 8) | (1 << 22));
    while (!(GET32(RESETS_RESET_DONE) & (1 << 22)));
}

/* Configures UART0 to 9600 8N1 */
static void uart_init(void)
{
    PUT32(UART_IBRD, 78);
    PUT32(UART_FBRD, 8);
    PUT32(UART_LCRH, (3 << 5));
    PUT32(UART_CR, (1 << 9) | (1 << 8) | (1<<0));

    PUT32(IO_BANK0_GPIO00_CTRL, 2);
    PUT32(IO_BANK0_GPIO01_CTRL, 2);
}

/******** EXTERNAL YMODEM *********/
int ymodem_receive(uint8_t*, uint32_t, uint32_t*);
extern void uart_putc(uint8_t);
extern uint8_t uart_getc_blocking(void);

/******** JUMP FUNCTION *********/

void jump_to_app(void)
{
    uint32_t msp = *(uint32_t*)APP_ADDR;
    uint32_t pc  = *(uint32_t*)(APP_ADDR + 4);

    __asm volatile("msr msp, %0"::"r"(msp));
    ((void (*)(void))pc)();
}

/******** MAIN *********/

__attribute__((section(".boot.entry"))) int main(void)
{
    clocks_init();
    reset_peripherals();
    uart_init();

	led_init();

	for(int i = 0; i < 5; i++)
	{
    	led_toggle();
    	delay();
	}


    uart_puts("\r\nRP2040 RAM BOOTLOADER\r\n");
    uart_puts("Send firmware using YMODEM...\r\n");

    uint32_t rx = 0;
    int res = ymodem_receive((uint8_t*)APP_ADDR, MAX_FW, &rx);

    if (res == 0)
    {
        uart_puts("Load OK. Jumping...\r\n");
        jump_to_app();
    }
    else
    {
        uart_puts("YMODEM Failed\r\n");
    }

    while (1);
}

