// This file provides API for open/close, read/write and config 
// of USB ports for serial communication 

#ifndef __PORT_H
#define __PORT_H

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <termios.h> 
#include <unistd.h>
#include <errno.h>

typedef struct 
{
    int desc;
    uint8_t rxBuffer[10];
    uint8_t txBuffer[255];
}port;


/**
 * API for opening of port with pathname path and descriptor desc
 * returns 1 on success and -1 on failure and corresponding errno will be updated
 */
int p_open(const char *path);

/**
 * API for closing of port with descriptor desc
 * returns 1 on success and -1 on failure and corresponding errno will be updated
 */
int p_close(int pd, const char *path);

/**
 * API for configuring of port with descriptor desc
 * returns 1 on success and -1 failure
 */
int p_config(int pd);

/**
 * API for reading cnt bytes from port with descriptor desc
 * returns 1 on success -1 on improper write and 0 on failure
 */
int p_read(int pd, uint8_t *buf, uint8_t cnt);

/**
 * API for reading cnt bytes from port with descriptor desc
 * returns 1 on success, -1 on improper write and 0 on failure
 */
int p_write(int pd, uint8_t *buf, uint8_t cnt);

#endif