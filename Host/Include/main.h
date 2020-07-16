#ifndef __MAIN_H
#define __MAIN_H

#include <stdint.h>
#include "fops.h"
#include "serial.h"

uint32_t crc(uint8_t* buf, uint32_t size);

#endif
