	}

	tcflush(port, TCIOFLUSH);
	close(port);

	return 1;
}

int config_port(char* str)
{
	//Open the port successfully else exit 
	int	port = open(str, O_RDWR | O_NOCTTY, 0777);
	if(port < 0){
		perror("Error Opening Serial Port");
		printf("Exiting program!!!\n");
		exit(0);
	}
	printf("%s opened successfully\n", str);

	//termios structure
	struct termios pConfig;
	tcgetattr(port, &pConfig);

	//change only necessary parameters
	
	//set baud rate
	cfsetispeed(&pConfig, B9600);	//input speed
	cfsetospeed(&pConfig, B9600);	//output speed

	pConfig.c_cflag &= ~PARENB;	//No Parity Bit Enabled
	
	pConfig.c_cflag &= ~CSTOPB;	//Stop bits zero

	pConfig.c_cflag |= CS8;	//Data bits 8

	pConfig.c_cflag &= ~CRTSCTS;	//No Hardware Flow Control

	pConfig.c_cflag |= CREAD | CLOCAL;	//Receiver Turn On

	pConfig.c_iflag &= ~(IXON | IXOFF | IXANY);	//No software Flow Control

	pConfig.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);	//Non-Canonical Mode

	//Poll once
	pConfig.c_cc[VMIN] = 0;		
	pConfig.c_cc[VTIME] = 0;	

	//set the attributes immediately
	tcsetattr(port, TCSANOW, &pConfig);

	return port;	//return file descriptor
}

void handshake(int port)
{
	//Handshake with STM32
	
	int rxStatus = 0;
	
	
	printf("Initiating handshake with STM32\n");
	
	//Sending data 0xDEAD
	memcpy(txBuffer, "\xDE\xAD", 2);
	tcflush(port, TCIOFLUSH);
	write_count = write(port, txBuffer, 2);	//Handshake attempt 1
	if(write_count < 0){	//Error in sending handshake to STM32
		perror("Error");
		printf("Restransmitting...");
		tcflush(port, TCOFLUSH);
		write_count = write(port, txBuffer, 2);	//Handshake attempt 2
		if(write_count < 0){
			perror("Error in transmission");
			printf("Exiting the program!!!");	//Error in attempt 2 handshake and exit program
			exit(0);
		}
	}

	printf("Sent %d bytes\n", write_count);

	rxStatus = rx_Status();	//wait for reply from STM32

	if(rxStatus == RX_TIMEOUT_ERR)
		exit(0);
	
	if(rxStatus == RX_ACK_FAIL){
		printf("Handshake ACK failed...\n");
		printf("Exiting Program!!!\n");
		exit(0);
	}

	//Handshake successful
	printf("Handshake Successful!!!\n");
	printf("Proceeding to send Bin size\n");
}

void tx_binInfo(int port, char* bin, char* flash_base)
{
	int rxStatus = 0;

	/***Sending Bin file size***/

	/*FILE* fp = fopen(bin, "rb");	//Open bin file in binary mode

	if(errno){	//Error opening bin file
		perror("Error");
		printf("Exiting program!!!\n");
		exit(0);
	}

	perror("File Open Status");

	fseek(fp, 0, SEEK_END);	//Compute bin file size
	file_len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	printf("Total length of the file: %d\n", file_len);

	int conv_len = snprintf(NULL, 0, "{data:%d}", file_len);
	sprintf((char*)txBuffer, "%d", file_len);

	write_count = 0;
	tcflush(port, TCIOFLUSH); //flush before write

	write_count = write(port, &txBuffer[0], conv_len);	//sending length of data that follows

	write_count = 0;	//send bin file size
	write_count = write(port, txBuffer, conv_len);	//Writing file size
	if(write_count < 0){	
		perror("Error");
		printf("Restransmitting...");
		write_count = write(port, txBuffer, conv_len);		
		if(write_count < 0){
			perror("Error in transmission");
			printf("Exiting the program!!!");	
			exit(0);
		}
	}

	rxStatus = rx_Status();		//wait for reply from STM32

	if(rxStatus == RX_TIMEOUT_ERR)
		exit(0);

	if(rxStatus == RX_ACK_FAIL){
		printf("Binary Size ACK fail...\n");
		printf("Exiting Program !!!");
		exit(0);
	}*/

	/******************************************/
	
	rxStatus = 0;	//rxStatus reset

	strcpy((char*)txBuffer, flash_base);	//copy flash base addr to txBuffer

	tcflush(port, TCIOFLUSH);	//flush before write
	write_count = write(port, txBuffer, conv_len);	//Writing Flash Base addr
	if(write_count < 0){	
		perror("Error");
		printf("Restransmitting...");
		write_count = write(port, txBuffer, conv_len);		
		if(write_count < 0){
			perror("Error in transmission");
			printf("Exiting the program!!!");	
			exit(0);
		}
	}

	rxStatus = rx_Status();		//wait for reply from STM32

	if(rxStatus == RX_TIMEOUT_ERR)
		exit(0);

	if(rxStatus == RX_ACK_FAIL){
		printf("Flash Base ACK fail...\n");
		printf("Exiting Program !!!");
		exit(0);
	}

	//Transfer successful
	printf("Bin size and Flash base addr sent\n");
	printf("Proceeding to initiate flashing process\n");

	fclose(fp);	//Close the bin file
}

int file_send(int port, char* bin)
{
	FILE* fp = fopen(bin, "rb");	//Open bin file in binary mode

	if(errno){		//Error opening bin file
		perror("Error");
		printf("Exiting program!!!\n");
		exit(0);
	}

	//reads 255 bytes worth of data or how much ever is remaining
	if(data_len > 255)
		data_read = 255;
	else
		data_read = data_len;
		
	read_len = fread((void*)txBuffer, 1, data_read, fp);
	data_len = data_len - read_len;

	//calculate CRC and append at end of buffer
	crc(read_len, (int)read_len);
	txbuffer[255] = crc_val;
	
	tcflush(port, TCIOFLUSH);
	write_count = write(port, txBuffer, 256);

	return rxStatus();
}

int rx_Status(void)
{

	//reset read count 
	read_count = 0;

	clock_t start =  clock();	//Start time
	while(time_lapse(clock(), start) < 10){
		read_count = read(port, rxBuffer, 2);
		if(read_count)
			break;
	}

	//Reception Timeout
	if(read_count < 1){
		printf("Timeout...\n");
		printf("Exiting program!!!\n");
		return RX_TIMEOUT_ERR;
	}

	//ACK failed
	if(strncmp((char*)rxBuffer, (char*)ack, 2)){
		return RX_ACK_FAIL;
	}

	return RX_ACK_SUCCESS;
}

uint8_t crc(uint32_t data, int read_len)
{
	crc_val = 0xFF;
	uint8_t data;	//CRC of the data being calculated

	for(int n=0; n<read_len; n++)
	{
		uint8_t data =	data[n];

		for(int i=0; i<8; i++)
		{
			if(crc_val ^ 0x80)
				crc_val = (crc_val << 1) ^ poly;
			else
				crc_val = (crc_val << 1);
		}
	}

	return crc_val;
}

int time_lapse(clock_t end, clock_t start)
{
	int time = 0;
	time = (int)((end - start)/CLOCKS_PER_SEC);
	return time;
}

/***************End of File***************/
