#include "ymodem.h"
#include <stdint.h>

/******** UART HOOKS ********/
extern void uart_putc(uint8_t c);
extern uint8_t uart_getc_blocking(void);

/******** Direct HW Access ********/
#define GET32(a) (*(volatile unsigned int *)(a))

#define UART0_BASE 0x40034000
#define UART_FR    (UART0_BASE + 0x18)

/******** YMODEM DEFINES ********/
#define SOH 0x01
#define STX 0x02
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18
#define CRC_REQ 'C'

#define PACKET_SIZE     128
#define PACKET_1K_SIZE  1024


static uint16_t crc16(const uint8_t *buf, int len)
{
    uint16_t crc = 0;
    while (len--)
    {
        crc ^= (*buf++) << 8;
        for (int i = 0; i < 8; i++)
            crc = (crc & 0x8000) ?
                  (crc << 1) ^ 0x1021 :
                  (crc << 1);
    }
    return crc;
}


/******** YMODEM RECEIVE ********/
int ymodem_receive(uint8_t *dst, uint32_t max, uint32_t *received)
{
    uint8_t packet[1031];
    uint8_t blk = 0;        // IMPORTANT: start from block 0
    uint32_t offset = 0;

    /***** Send 'C' continuously until sender responds *****/
    while (1)
    {
        uart_putc(CRC_REQ);

        // small delay
        for (volatile int i = 0; i < 500000; i++);

        // RX not empty?
        if (!(GET32(UART_FR) & (1 << 4)))
            break;
    }


    /******** RECEIVE LOOP ********/
    while (1)
    {
        uint8_t start = uart_getc_blocking();

        if (start == SOH || start == STX)
        {
            uint32_t size = (start == SOH) ? PACKET_SIZE : PACKET_1K_SIZE;

            uint8_t block   = uart_getc_blocking();
            uint8_t invblk  = uart_getc_blocking();

            if ((block + invblk) != 0xFF)
            {
                uart_putc(NAK);
                continue;
            }

            for (uint32_t i = 0; i < size; i++)
                packet[i] = uart_getc_blocking();

            uint16_t rx_crc =
                (uart_getc_blocking() << 8) |
                 uart_getc_blocking();

            uint16_t calc = crc16(packet, size);

            if (rx_crc != calc)
            {
                uart_putc(NAK);
                continue;
            }
			/*if (rx_crc != calc)
			{
    			// For Block0 some senders vary CRC — ignore mismatch
    			if (block == 0)
    			{
        			uart_putc(ACK);
        			uart_putc('C');
        			blk = 1;
        			continue;
    			}

    			uart_putc(NAK);
    			continue;
			}*/


            /******** BLOCK 0 (Header: filename + size) ********/
            if (block == 0)
            {
                uart_putc(ACK);
                uart_putc('C');     // VERY IMPORTANT
                blk = 1;
                continue;
            }

            /******** NORMAL DATA BLOCK ********/
            if (block == blk)
            {
                for (uint32_t i = 0; i < size && offset < max; i++)
                    dst[offset++] = packet[i];

                blk++;
            }

            uart_putc(ACK);
        }
        else if (start == EOT)
        {
			// First EOT --> reply NAK 
            uart_putc(NAK);

			// Wait for the second EOT 
			start= uart_getc_blocking();

			if(start == EOT)
			{
				uart_putc(ACK);
				break;            // Now Exit cleanly
        	}
			continue;
		}
		/*else if (start == EOT)
		{
			uart_putc(ACK); // acknowledge end of transfer
			break;
		}*/
        else if (start == CAN)
        {
            return -2;
        }
    }

    if (received)
        *received = offset;

    return 0;
}

