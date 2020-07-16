#include "fops.h"

uint32_t fops_fsize(char* file_name) 
{
	FILE* file;
	uint32_t flen;
	file = fopen(file_name, "rb");
	if(!file){
		printf("File does not exist\r\n");
		return 0;
	}
	fseek(file, 0, SEEK_END);
	flen = ftell(file);
	fseek(file, 0, SEEK_SET);
	fclose(file);
	return (flen);
}

void fops_fcpy(uint8_t* buf, char* file_name)
{
	FILE* file;
	file = fopen(file_name, "rb");
	fread(buf, 1, 224, file);
	fclose(file);
}

