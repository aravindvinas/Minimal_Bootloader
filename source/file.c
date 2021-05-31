#include "file.h"

int f_size(const char *loc, long int *size)
{
    FILE *fp = fopen(loc, "rb");
    if(fp == NULL)
    {
        printf("Unable to open file\n");
        perror("Error");
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    *size = ftell(fp);
    fclose(fp);
    return 1;
}

int f_read(const char *loc, int len, unsigned char *buf)
{
    FILE *fp = fopen(loc, "rb");
    if(fp == NULL)
    {
        printf("Unable to open file\n");
        perror("Error");
        return -1;
    }

    fread(buf, 1, len, fp);
    fclose(fp);
    return 1;
}
