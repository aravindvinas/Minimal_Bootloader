#include <stdlib.h>
#include <stdint.h>
#include <errno.h>


//file total length
uint32_t file_len;

int 
main(int argc, char* argv[])
{
	//Packet buffer
	uint8_t packet[256];
	//left out data
	uint32_t data_len = 0;
	//amount of data to read
	uint32_t data_read = 0;
	//amount of data read
	uint32_t read_len = 0;

	//get total file length	
	FILE* fp = fopen(argv[1], "rb");

	//if file opening encounters error
	if(errno){
		perror("Error");
		printf("Exiting program!\r\n");
		exit(0);
	}

	perror("Open Status");

	//Compute file length
	fseek(fp, 0, SEEK_END);
	file_len = ftell(fp);
	data_len = file_len;
	fseek(fp, 0, SEEK_SET);
	printf("Total length of the file: %d\n", file_len);

	//reading the file
	while(data_len){
		if(data_len > 244)
			data_read = 244;
		else
			data_read = data_len;

		read_len = fread((void*)packet, 1, data_read, fp);
		data_len = data_len - read_len;

		printf("Data read: %d  |  Data yet to read: %d\n", read_len, data_len);
	}

	fclose(fp);
	
	return 1;
}
