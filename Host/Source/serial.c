#include "serial.h"

uint8_t serial_init(port_handle* hport, const char* port)
{
    hport->fdesc = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
    if(hport->fdesc < 0)
        return FILE_OPEN_ERROR;
    fcntl(hport->fdesc, F_SETFL, 0);

    if(tcgetattr(hport->fdesc, &(hport->tty)) != 0)
        return GET_ATTR_ERROR;

    hport->tty.c_cflag |= (CLOCAL | CREAD);
    hport->tty.c_cflag &= ~CBAUD;
    cfsetispeed(&(hport->tty), B115200);

    hport->tty.c_cflag &= ~CSIZE;
	hport->tty.c_cflag |= CS8;
	hport->tty.c_cflag |= (PARENB | PARODD);
	hport->tty.c_cflag |= 0;
	hport->tty.c_cflag &= ~CRTSCTS;
	hport->tty.c_cflag |= HUPCL;
	hport->tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	hport->tty.c_iflag |= (INPCK | ISTRIP);
	hport->tty.c_iflag &= ~(IXON | IXOFF | IXANY);
	hport->tty.c_oflag &= ~OPOST;
	hport->tty.c_cc[VMIN ] = 0;
	hport->tty.c_cc[VTIME] = 30;

    if(tcsetattr(hport->fdesc, TCSANOW, &hport->tty))
        return SET_ATTR_ERROR;
    
    return SERIAL_INIT_OK;
}

uint8_t serial_send(port_handle* hport, char* buf, int len)
{
    int dlen = len;
    int txlen = 0;
    const void* buf_ptr = buf;

    while(dlen > 0){
        txlen = write(hport->fdesc, buf_ptr, dlen);
        if(txlen < 0)
            return SERIAL_TX_ERROR;
        dlen -= txlen;
        buf_ptr += txlen;
    }

    return SERIAL_TX_OK;
}

uint8_t serial_read(port_handle* hport, char* buf, int len)
{
    int dlen = len;
    int rxlen = 0;
    const void* buf_ptr = buf;

    while(dlen > 0){
        rxlen = read(hport->fdesc, buf_ptr, dlen);
        if(rxlen < 0)
            return SERIAL_RX_ERROR;
        dlen -= rxlen;
        buf_ptr += rxlen;
    
    return SERIAL_RX_OK;
    }

}

void serial_exit(port_handle* hport)
{
    tcflush(hport->fdesc, TCIOFLUSH);
    close(hport->fdesc);
}
