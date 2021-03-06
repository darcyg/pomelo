/**
* uart.c -- UART驱动接口
* 
* 
* 创建时间: 2010-4-23
* 最后修改时间: 2010-4-24
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

#include "include/debug.h"
#include "include/sys/uart.h"

static int SpeedArray[] = {	B115200, B57600, B38400, B19200, B9600, 
							B4800, B2400, B1200, B600, B300, };
static int NameArray[] = {	115200, 57600, 38400,  19200,  9600,  
							4800,  2400,  1200,  600, 300, } ;

//static int FidUart[UART_PORTNUM] = {-1, -1, -1, -1, -1,};
static int FidUart[UART_PORTNUM] = {-1, -1, -1, -1, -1, -1, -1};

#define UART_RCVBUF_SIZE		2048
struct uart_rcvst {
	unsigned char buf[UART_RCVBUF_SIZE];
	int len;
	int head;
};
static struct uart_rcvst UartRecvBuffer[UART_PORTNUM];

/**
* @brief 打开一个UART端口
* @param port 端口号, 0~4
* @return 0成功, 否则失败
*/
int UartOpen(unsigned int port)
{
	char dev[16];
	int fd;

	//AssertLogReturn(port>=UART_PORTNUM, 1, "invalid port(%d)\n", port);
	

	if(-1 != FidUart[port]) 
	{
		close(FidUart[port]);
		FidUart[port] = -1;
	}

	//if(port == (UART_PORTNUM-1)) strcpy(dev, "/dev/ttyS0");
	//else sprintf(dev, "/dev/ttyS%d", port+1);
	//else sprintf(dev, "/dev/ttyS%d", port);

	sprintf(dev, "/dev/ttyS%d", port);

	//RDWR | O_NOCTTY | O_NONBLOCK
	fd = open(dev, O_RDWR|O_NONBLOCK);	
	//fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if(-1 == fd) 
	{
		printf("can not open uart %d\n", port);
		return 1;
	}
	else
	{
		printf("open uart %d port succ\n", port);
	}

	FidUart[port] = fd;
	printf("UartOpen fd =%d\n", fd);
	UartRecvBuffer[port].len = 0;

	return 0;
}

/**
* @brief 关闭一个已打开的UART端口
* @param port 端口号, 0~4
*/
void UartClose(unsigned int port)
{
	AssertLogReturnVoid(port>=UART_PORTNUM, "invalid port(%d)\n", port);
	AssertLogReturnVoid(-1 == FidUart[port], "invalid fid(%d)\n", FidUart[port]);

	close(FidUart[port]);
	FidUart[port] = -1;
}

/**
* @brief 设置UART端口波特率
* @param port 端口号, 0~4
* @param baud 波特率
*/
void UartSetBaudrate(unsigned int port, int baud)
{
	int i; 
	int status; 
	struct termios opt;
	int fd;

	AssertLogReturnVoid(port>=UART_PORTNUM, "invalid port(%d)\n", port);
	fd = FidUart[port];
	AssertLogReturnVoid(-1 == fd, "invalid fid(%d)\n", fd);

	if(tcgetattr(fd, &opt) != 0) {
		ErrorLog("tcgetattr fail\n");
		return;
	}

	for ( i= 0;  i < sizeof(SpeedArray) / sizeof(int);  i++) { 
		if  (baud == NameArray[i]) {     
			tcflush(fd, TCIOFLUSH);     
			cfsetispeed(&opt, SpeedArray[i]);  
			cfsetospeed(&opt, SpeedArray[i]);   
			status = tcsetattr(fd, TCSANOW, &opt);  
			if  (status != 0) {        
				ErrorLog("tcsetattr fail\n");
				return;     
			}    
			tcflush(fd,TCIOFLUSH);   
		}
	}
}

/**
* @brief 设置UART端口属性
* @param port 端口号, 0~4
* @param databits 数据位, 5~8
* @param stopbits 停止位, 1~2
* @param parity 校验位
*/
void UartSetParity(unsigned int port, int databits,int stopbits, char parity)
{ 
	struct termios options; 
	int fd;

	AssertLogReturnVoid(port>=UART_PORTNUM, "invalid port(%d)\n", port);
	fd = FidUart[port];
	AssertLogReturnVoid(-1 == fd, "invalid fid(%d)\n", fd);

	if(tcgetattr(fd, &options) != 0) {
		ErrorLog("tcgetattr fail\n");
		return;
	}

	options.c_iflag = 0;

	options.c_cflag &= ~CSIZE; 
	switch (databits) { /*璁剧疆鏁版嵁浣嶆暟*/
	case 5: options.c_cflag |= CS5; break;
	case 6: options.c_cflag |= CS6; break;	
	case 7:	options.c_cflag |= CS7; break;
	case 8: options.c_cflag |= CS8; break;   
	default: ErrorLog("invalid databits(%d)\n", databits); return;   
	}

	switch (parity) {
	case 'n':
	case 'N':    
		options.c_cflag &= ~PARENB;   /* Clear parity enable */
		options.c_iflag &= ~INPCK;     /* Enable parity checking */ 
		break;  
	case 'o':   
	case 'O':     
		options.c_cflag |= (PARODD | PARENB); /* 璁剧疆涓哄鏁堥獙*/  
		options.c_iflag |= INPCK;             /* Disnable parity checking */ 
		break;  
	case 'e':  
	case 'E':   
		options.c_cflag |= PARENB;     /* Enable parity */    
		options.c_cflag &= ~PARODD;   /* 杞崲涓哄伓鏁堥獙*/     
		options.c_iflag |= INPCK;       /* Disnable parity checking */
		break;
	case 'S': 
	case 's':  /*as no parity*/   
	    options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;break;  
	default: ErrorLog("invalid parity(%d)\n", parity); return;
	}

	/* 璁剧疆鍋滄浣�*/  
	switch (stopbits) {
	case 1: options.c_cflag &= ~CSTOPB; break;  
	case 2: options.c_cflag |= CSTOPB; break;
	default: ErrorLog("invalid stopbits(%d)\n", stopbits); return;
	}

	options.c_iflag  &= ~(INLCR|IGNCR|ICRNL|IUCLC);  //add 2007-10-19
	options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
	options.c_oflag  &= ~OPOST;   /*Output*/

	tcflush(fd, TCIFLUSH);
	options.c_cc[VTIME] = 0; /* 璁剧疆瓒呮椂15 seconds*/   
	options.c_cc[VMIN] = 0; /* Update the options and do it NOW */

	if(tcsetattr(fd, TCSANOW, &options) != 0) {
		ErrorLog("tcsetattr fail\n");
		return;
	}
}

/**
* @brief 设置UART端口波特率和属性
* @param port 端口号, 0~4
* @param baud 波特率
* @param databits 数据位, 5~8
* @param stopbits 停止位, 1~2
* @param parity 校验位
*/
void UartSet(unsigned int port, int baud, int databits, int stopbits, char parity)
{
	UartSetBaudrate(port, baud);
	UartSetParity(port, databits, stopbits, parity);
}

/**
* @brief 向UART端口发送数据
* @param port 端口号, 0~4
* @param buf 发送缓存区
* @param len 缓存区长度
* @param 0成功, 否则失败
*/
int UartSend(unsigned int port, const unsigned char *buf, int len)
{
	int fd;

	AssertLogReturn(port>=UART_PORTNUM, 1, "invalid port(%d)\n", port);
	fd = FidUart[port];
	//printf("UartSend port = %d fd = %d",port,fd);
	AssertLogReturn(-1 == fd, 1, "invalid fid(%d)\n", fd);
	AssertLog(len<=0, "invalid len(%d)\n", len);

	write(fd, (char *)buf, len);

	return 0;
}

/**
* @brief 从UART端口接收数据
* @param port 端口号, 0~4
* @param buf 接收缓存区
* @param len 缓存区长度
* @return 失败返回-1, 成功返回接收到的字节数, 返回0表示未接收到数据
*/
int UartRecv(unsigned int port, unsigned char *buf, int len)
{
	int fd, rtn, i;
	unsigned char *precv;

	AssertLogReturn(port>=UART_PORTNUM, -1, "invalid port(%d)\n", port);
	fd = FidUart[port];

	if(-1 == fd)
	{
		printf("UartRecv............................................");
		printf("port = %d fd = %d",port,fd);
	}
	AssertLogReturn(-1 == fd, -1, "invalid fid(%d)\n", fd);
	AssertLog(len<=0, "invalid len(%d)\n", len);

	if(UartRecvBuffer[port].len <= 0) 
	{
		UartRecvBuffer[port].len = 0;
		UartRecvBuffer[port].head = 0;
		rtn = read(fd, UartRecvBuffer[port].buf, UART_RCVBUF_SIZE);
		if((rtn <=0) || (rtn > UART_RCVBUF_SIZE)) return 0;
		UartRecvBuffer[port].len = rtn;
	}

	if(UartRecvBuffer[port].len > len) rtn = len;
	else rtn = UartRecvBuffer[port].len;

	precv =  &UartRecvBuffer[port].buf[UartRecvBuffer[port].head];
	for(i=0; i<rtn; i++) *buf++ = *precv++;
	//memcpy(buf, &UartRecvBuffer[port].buf[UartRecvBuffer[port].head], rtn);
	UartRecvBuffer[port].len -= rtn;
	UartRecvBuffer[port].head += rtn;

	return rtn;
}

/**
* @brief 获取UART端口的文件号(ppp使用)
* @param port 端口号, 0~4
* @return 失败返回-1, 成功返回文件号
*/
int UartGetFid(unsigned int port)
{
	AssertLogReturn(port>=UART_PORTNUM, -1, "invalid port(%d)\n", port);

	return(FidUart[port]);
}

/*void UartTest(void)
{
	int i;
	unsigned char buf[8];

	UartOpen(0);
	UartSet(0, 19200, 8, 1, 'E');

	for(i=0; i<1000000; i++) UartRecv(0, buf, 8);

	exit(0);
}
//测试结果: 整个程序耗时6.24s, 系统调用耗时5.64s, 因此系统调用耗时大概为5.6us
*/
