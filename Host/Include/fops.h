#ifndef __FOPS_H
#define __FOPS_H

#include<stdio.h>
#include<stdint.h>

uint32_t fops_fsize(char* file_name);
void fops_fcpy(uint8_t* buf, char* file_name);

#endif

