/*
 *							BOOTLOADER PROTOCOL
 *
 * Handshake with STM32:
 *			Send 0xdead and receive 0xbeef
 *			Receive timeout 10sec; resend three times; exit(0)
 *
 * Binary Size and Flash base addr:
 *		Send binary size and receive ack (0xacd)
 *		Receive timeout 10sec; resend three times; exit(0)
 *
 * Initiate Tx signal:
 *		Receive ack(0xacd) 
 *		Receive timeout 10sec; No resend; exit(0)
 *
 * Send 248 bytes of hex + 8 bytes CRC:
 *		Receive ACK for CRC
 *			Receive timeout 10sec; resend 3 times; exit(0)
 *		Flash write ACK
 *			Receive timeout 10sec; no resend; exit(0)
 *
 * Finish transmitting binary
 *
 * Send signal to jump user code
 *
 * After jump to user code close serial connection
 * exit(0)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

uint32_t file_len;		//Total file length
const unsigned char* ack = "\xBE\xEF";
unsigned char txBuffer[256];	//Transfer Buffer	
unsigned char rxBuffer[10];		//Receive Buffer	
int port;
int read_count = 0;			//serial read count
int write_count = 0;		//serial write count

int config_port(char* str);
void handshake(int port);
void tx_binInfo(int port, char* bin, char* flash_base);
int time_lapse(clock_t end, clock_t start);	//Calculate time between Start and End

//argv[1] is binary file
//argv[2] is serial port addr
//argv[3] is Binary file name
//argv[4] is Flash base addr
int 
main(int argc, char* argv[])
{
	printf("*************************\n");
	printf("*                       *\n");
	printf("*     Flash Loader      *\n");
	printf("*                       *\n");
	printf("*************************\n");
	
	port = config_port(argv[2]);				//Configure Serial Port
	printf("Serial configured successfully\n");

	handshake(port);		//Handshake with STM32

	tx_binInfo(port, argv[3], argv[4]);		//Send bin size and flash base addr
	

	//reading the file
/*	while(data_len){
		if(data_len > 256)
			data_read = 256;
		else
			data_read = data_len;

		read_len = fread((void*)packet, 1, data_read, fp);
		data_len = data_len - read_len;

		printf("Data read: %d  |  Data yet to read: %d\n", read_len, data_len);
		for(int i=0; i<read_len; i++)
		{
			printf("%x", packet[i]);
			printf("\n");
		}

	}

	fclose(fp);
*/
	
	return 1;
}

int config_port(char* str)
{
	//Open the port successfully else exit 
	int	port = open(str, O_RDWR | O_NOCTTY, 0777);
	if(port < 0){
		perror("Error");
		printf("Exiting program!!!");
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

	//Min data received and maximum wait time
	pConfig.c_cc[VMIN] = 1;		//1 byte min
	pConfig.c_cc[VTIME] = 0;	//port timeout infinity

	//set the attributes immediately
	tcsetattr(port, TCSANOW, &pConfig);

	return port;	//return file descriptor
}

void handshake(int port)
{
	//Handshake with STM32
	
	
	printf("Initiating handshake with STM32\n");
	
	//Sending data 0xDEAD
	memcpy(txBuffer, "\xDE\xAD", 2);
	write_count = write(port, txBuffer, 2);			//Handshake attempt 1
	if(write_count < 0){							//Error in sending handshake to STM32
		perror("Error");
		printf("Restransmitting...");
		write_count = write(port, txBuffer, 2);		//Handshake attemp 2
		if(write_count < 0){
			perror("Error in transmission");
			printf("Exiting the program!!!");		//Error in attempt 2 handshake and exit program
			exit(0);
		}
	}

	clock_t start =  clock();	//Start time
	while(time_lapse(clock(), start) < 10){
		read_count = read(port, rxBuffer, 2);
		if(read_count)
			break;
	}

	//Reception Timeout
	if(!read_count){
		perror("Error receiving handshake");
		printf("Timeout...");
		printf("Exiting program!!!");
		exit(0);
	}

	//Handshake ACK failed
	if(strncmp(rxBuffer, ack, 2)){
		printf("Handshake Failed...");
		printf("Exiting Program!!!");
		exit(0);
	}

	//Handshake successful
	printf("Handshake Successful!!!\n");
	printf("Proceeding to send Bin size\n");
}

void tx_binInfo(int port, char* bin, char* flash_base)
{
	FILE* fp = fopen(bin, "rb");	//Open bin file in binary mode

	if(errno){		//Error opening bin file
		perror("Error");
		printf("Exiting program!!!\n");
		exit(0);
	}

	perror("File Open Status");

	fseek(fp, 0, SEEK_END);		//Compute bin file size
	file_len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	printf("Total length of the file: %d\n", file_len);

	int conv_len = snprintf(NULL, 0, "{data:%d}", file_len);
	sprintf(txBuffer, "%d", file_len);

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

	strcpy(txBuffer, flash_base);
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
	
	clock_t start =  clock();	//Start time
	while(time_lapse(clock(), start) < 10){
		read_count = read(port, rxBuffer, 2);
		if(read_count)
			break;
	}

	//Reception Timeout
	if(!read_count){
		perror("Error receiving handshake");
		printf("Timeout...");
		printf("Exiting program!!!");
		exit(0);
	}

	//ACK failed
	if(strncmp(rxBuffer, ack, 2)){
		printf("ACK Failed...");
		printf("Exiting Program!!!");
		exit(0);
	}

	//Transfer successful
	printf("Bin size and Flash base addr sent\n");
	printf("Proceeding to initiate flashing process\n");

	fclose(fp);	//Close the bin file
}

int time_lapse(clock_t end, clock_t start)
{
	int time = 0;
	time = (int)((end - start)/CLOCKS_PER_SEC);
	return time;
}
