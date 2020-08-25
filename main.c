#include "main.h"

int
main(void)
{
	uint8_t data[256], rx;
	uint32_t crc_data;

	const uint32_t addr = 0x08000800;
	const char file[20];
	const char port[20];
	printf("Enter file name\n");
	scanf("%s\n", file);
	printf("Enter port address\n");
	scanf("%s\n", port);

	port_handle hport;

	serial_init(&hport, port);

	// First send the address in which the code is to be flashed along with its CRC
	//Check for ACK 
	//If ACK proceed with reading the file and storing it in the buffer, else resend address
	//Check ACK else resend buffer

	/*break_down(addr, data);
	crc_data = crc(data, 4);
	break_down(crc_data, (data + 5));
	
	serial_send(&hport, &data, 8);
	if(serial_read(&hport, &rx, 1)){
		printf("Address CRC failed");
		exit(0);
	}

	fops_fcpy(data, file);
	uint32_t data_crc = crc(data, 224);
	break_down(data_crc, (data + 225));

	serial_send(&hport, data, 256);*/
}

void break_down(uint32_t data, uint8_t* ptr)
{
	uint32_t temp = data;

	temp = temp & 0xFF000000UL;
	temp = (temp >> 24);
	ptr[0] = (uint8_t) temp;

	temp = data;
	temp = temp & 0x00FF0000UL;
	temp = (temp >> 16);
	ptr[1] = (uint8_t) temp;

	temp = data;
	temp = temp & 0x0000FF00UL;
	temp = (temp >> 8);
	ptr[2] = (uint8_t) temp;

	temp = data;
	temp = temp & 0x000000FFUL;
	ptr[3] = (uint8_t) temp;
}


uint32_t crc(uint8_t* buf, uint32_t size)
{
	int i, n; 
	uint32_t crc = 0xFFFFFFFF;
	for(n=0; n<size; n++)
	{
		uint32_t data = buf[n];
		crc = crc ^ data;
		for(i=0; i<32; i++)
		{
			if(crc & 0x80000000){
				crc = (crc << 1) ^ 0x04C11DB7;
			}
			else{
				crc = (crc << 1);
			}
		}
	}
	return crc;
}
