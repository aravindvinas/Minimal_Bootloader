#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#define FILE_OPEN_ERROR     0x00
#define GET_ATTR_ERROR      0X01
#define SET_ATTR_ERROR      0x02
#define SERIAL_INIT_OK      0x03
#define SERIAL_TX_ERROR     0x04
#define SERIAL_TX_OK        0x05
#define SERIAL_RX_ERROR     0x06
#define SERIAL_RX_OK        0x07

typedef struct{
    int fdesc;
    struct termios tty;
}port_handle;

uint8_t serial_init(port_handle* hport, const char* port);
uint8_t serial_send(port_handle* hport, char* buf, int len);
uint8_t serial_read(port_handle* hport, char* buf, int len);
void serial_exit(port_handle* hport);

#endif
