#ifndef YMODEM_H
#define YMODEM_H

#include <stdint.h>

int ymodem_receive(uint8_t *dst, uint32_t max, uint32_t *received);

#endif

