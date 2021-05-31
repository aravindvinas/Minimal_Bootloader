#ifndef __FILE_H
#define __FILE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

/**
 *API to fetch size of file located at loc
 *returns 1 on success and -1 on failure and corresponding errno will be displayed
 */
int f_size(const char *loc, long int *size);

/**
 *API to read len bytes of a file located at loc and store it at buffer buf
 *returns 1 on success and -1 on failure and corresponding errno will be displayed
 */
int f_read(const char *loc, int len, unsigned char *buf);

#endif