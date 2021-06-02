#include "port.h"

int p_open(const char *path)
{
   int temp = open(path, O_RDWR);
   if(temp <= 0)
   {
       printf("Error opening the port %s\n", path);
       perror("Error");
       return -1;
   }

   printf("Port %s opened successfully\n", path);
   return temp;
}

int p_close(int pd, const char *path)
{
    int temp = close(pd);
    if(temp == -1)
    {
        printf("Error closing the port %s\n", path);
        perror("Error");
        return -1;
    }

    return 1;
}

int p_config(int pd)
{
    struct termios tty;
    int temp = tcgetattr(pd, &tty);
    if(temp == -1)
    {
        printf("Port configuration failed!!!\n");
        perror("Error");
        return -1;
    }

    //input mode flags
    //Disable SW Flowcontrol and handling of special characters
    tty.c_iflag &= ~(BRKINT|ICRNL|IGNBRK|IGNCR|INLCR|ISTRIP|
                     IXANY|IXOFF|IXON|PARMRK);

    //output mode flags
    //No special character handling
    tty.c_oflag &= ~(OPOST|ONLCR|OCRNL|ONOCR|ONLRET|OFILL);

    //control mode flags
    //8bit transfer, read enable, CD signal disable
    //Stop and Parity bit disable, disable hardware flow control 
    tty.c_cflag |= (CS8|CREAD|CLOCAL);
    tty.c_cflag &= ~(CSTOPB|PARENB|CRTSCTS);

    //local mode flags
    //Disable Canonical mode, Echo and signals
    tty.c_lflag &= ~(ICANON|ECHO|ECHOE|ECHONL|ISIG);

    //Wait until said bytes are received or 1sec passes, whichever first
    tty.c_cc[VTIME] = 10;
    tty.c_cc[VMIN] = 0;

    //Port speed
    tty.c_ospeed = 115200;
    tty.c_ispeed = 115200;

    //Set Baud 
    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    temp = tcsetattr(pd, TCSANOW, &tty);
    if(temp == -1)
    {
        printf("Error setting all parameters\n");
        perror("Error");
        return -1;
    }
    return 1;
}

int p_read(int pd, uint8_t *buf, uint8_t cnt)
{
    int temp = read(pd, buf, cnt);
    if(temp == -1)
    {
        printf("Error reading port\n");
        perror("Error");
        return 0;
    }

    if(temp < cnt)
    {
        return -1;
    }

    return 1;
}

int p_write(int pd, uint8_t *buf, uint8_t cnt)
{
    int temp = write(pd, buf, cnt);
    if(temp == -1)
    {
        printf("Error writing tp port\n");
        perror("Error");
        return 0;
    }

    if(temp < cnt)
    {
        return -1;
    }

    return 1;
}