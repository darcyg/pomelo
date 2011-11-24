/**
* scr_lcd.c -- 液晶显示
* 
* 
* 创建时间: 2010-5-20
* 最后修改时间: 2010-7-10
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include "scr_lcd.h"
#include "scr_top_flag.h"
#include "include/debug.h"
#include "include/sys/schedule.h"
#include "include/sys/sigin.h"
#include "include/sys/timeal.h"  
#include "include/screen/scr_show.h"
#include "include/sys/gpio.h"

static int fd_lcd = -1;
unsigned char myasc[4096];
static unsigned char hzk_gb2312[GB2312_HZK_SIZE];  
static unsigned char lcd_buffer[LCD_BUFFER_SIZE];
static unsigned char topm_buf[20][16] = {{0},{0}};
static unsigned char  DISINVS = 0;
static const unsigned char page_munber[88]=
{
0x70,0x88,0x88,0x88,0x88,0x88,0x88,0x70,
0x20,0x60,0x20,0x20,0x20,0x20,0x20,0x70,
0x70,0x88,0x88,0x10,0x20,0x40,0x80,0xF8,
0x70,0x88,0x08,0x30,0x08,0x08,0x88,0x70,
0x10,0x30,0x50,0x50,0x90,0x78,0x10,0x18,
0xF8,0x80,0x80,0xF0,0x08,0x08,0x88,0x70,
0x70,0x90,0x80,0xF0,0x88,0x88,0x88,0x70,
0xF8,0x90,0x10,0x20,0x20,0x20,0x20,0x20,
0x70,0x88,0x88,0x70,0x88,0x88,0x88,0x70,
0x70,0x88,0x88,0x88,0x78,0x08,0x48,0x70,
0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,
};

static const unsigned char asc10x8[160]={
0x00,0x00,0x00,0x00,0x78,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x78,0x00,0x00,0x00,0x00,///0
0x00,0x00,0x00,0x00,0x30,0x70,0x30,0x30,0x30,0x30,0x30,0x78,0x00,0x00,0x00,0x00,///1
0x00,0x00,0x00,0x00,0x78,0xCC,0xCC,0x18,0x30,0x60,0xC0,0xFC,0x00,0x00,0x00,0x00,///2
0x00,0x00,0x00,0x00,0x78,0xCC,0x0C,0x38,0x0C,0x0C,0xCC,0x78,0x00,0x00,0x00,0x00,///3
0x00,0x00,0x00,0x00,0x18,0x38,0x78,0x78,0xD8,0x7C,0x18,0x1C,0x00,0x00,0x00,0x00,///4
0x00,0x00,0x00,0x00,0xFC,0xC0,0xC0,0xF8,0x0C,0x0C,0xCC,0x78,0x00,0x00,0x00,0x00,///5
0x00,0x00,0x00,0x00,0x78,0xD8,0xC0,0xF8,0xCC,0xCC,0xCC,0x78,0x00,0x00,0x00,0x00,///6
0x00,0x00,0x00,0x00,0xFC,0xD8,0x18,0x30,0x30,0x30,0x30,0x30,0x00,0x00,0x00,0x00,///7
0x00,0x00,0x00,0x00,0x78,0xCC,0xCC,0x78,0xCC,0xCC,0xCC,0x78,0x00,0x00,0x00,0x00,///8
0x00,0x00,0x00,0x00,0x78,0xCC,0xCC,0xCC,0x7C,0x0C,0x6C,0x78,0x00,0x00,0x00,0x00,///9
};
static const unsigned char screen_logo[3200] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xC0,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78,0x0F,
0xF8,0xF0,0xFF,0xFF,0xFF,0xF0,0x30,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x78,0x3F,0xFC,0xF1,0xEF,0xFF,0xFF,0xF0,0x30,0xFE,0x03,0xC0,0x07,0x87,
0x07,0x0E,0x00,0x00,0x00,0x00,0xF8,0x7F,0xFD,0xF7,0xDF,0xFF,0xFF,0xF0,0xFE,0xFE,
0x07,0xE0,0x7F,0xB7,0x07,0x0E,0x00,0x00,0x00,0x00,0xF0,0xFF,0xFD,0xEF,0x9E,0x01,
0xE0,0x00,0xFE,0xCE,0x07,0xF0,0x3E,0x77,0x07,0x7F,0xE0,0x00,0x00,0x00,0xF0,0xFF,
0xFD,0xFE,0x1E,0x01,0xE0,0x00,0xC6,0xCE,0x1E,0x7C,0x06,0x3F,0x1F,0xFF,0xE0,0x00,
0x00,0x01,0xF1,0xFF,0xFF,0xFC,0x3F,0xF9,0xFF,0x80,0xFE,0xFE,0x3C,0x3F,0x06,0x1F,
0x1F,0x8E,0x00,0x00,0x00,0x01,0xE1,0xFF,0xFF,0xFC,0x3F,0xFB,0xFF,0x80,0xFE,0xFE,
0xFF,0xFF,0xFF,0x87,0x1F,0x8E,0x00,0x00,0x00,0x01,0xE1,0xFF,0xFF,0xFC,0x3F,0xFB,
0xFF,0x80,0xC6,0xCE,0xFF,0xFB,0xFF,0xF7,0x07,0x7F,0xC0,0x00,0x00,0x03,0xE3,0xFF,
0xFF,0xBE,0x78,0x07,0xC0,0x00,0xFE,0xCE,0x41,0xC0,0x0F,0x77,0x07,0xFF,0xC0,0x00,
0x00,0x03,0xC3,0xFF,0xFF,0xBE,0x78,0x07,0x80,0x00,0xFE,0xFE,0x1F,0xFC,0x1F,0xBF,
0x1F,0xB9,0x80,0x00,0x00,0x03,0xFD,0xFF,0xF7,0x9F,0x7F,0xF7,0xFF,0x80,0xD8,0xFE,
0x1F,0xFC,0x1F,0x97,0xDF,0x1B,0x80,0x00,0x00,0x07,0xFF,0xFF,0xEF,0x1F,0xFF,0xFF,
0xFF,0x80,0xDD,0xCE,0x1D,0xDC,0x3F,0xFF,0xDF,0x1F,0x80,0x00,0x00,0x07,0xFE,0xFF,
0x8F,0x0F,0xFF,0xFF,0xFF,0x00,0xDF,0xCE,0x1D,0xDC,0x77,0xFF,0x07,0x1F,0x00,0x00,
0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x8E,0x0F,0xF8,0x76,0xE7,
0x07,0x1F,0x00,0x00,0x00,0x00,0x00,0x3F,0xBD,0xDF,0x8E,0xF4,0x00,0x01,0xF7,0x8E,
0x7F,0xFF,0x26,0x07,0x0F,0x7F,0xE0,0x00,0x00,0x01,0xFF,0xD3,0xFF,0xFF,0x9B,0xBD,
0xFF,0xC0,0xC7,0x9E,0x7F,0xFF,0x0E,0x07,0x0E,0xFB,0xE0,0x00,0x00,0x01,0xFF,0xD3,
0xFF,0xFF,0x9F,0xE9,0xFF,0xC0,0x03,0x1C,0x00,0x00,0x0E,0x07,0x0C,0xE0,0xE0,0x00,
0x00,0x00,0x00,0x13,0xBD,0xD7,0xFE,0xE8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x00,0x1E,0x00,0x00,0xE0,
0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x00,
0x1E,0x00,0x00,0xF0,0x00,0x10,0xC0,0x00,0x01,0xC0,0x00,0x70,0x00,0x03,0x80,0x00,
0x00,0x00,0x70,0x00,0x1E,0x00,0x00,0x78,0x00,0x38,0xE0,0x00,0x19,0xC0,0x0C,0x70,
0x00,0x73,0xC0,0x00,0x00,0x00,0x70,0x00,0x0E,0x00,0x00,0x78,0x00,0x3C,0xE0,0x00,
0x1D,0xC0,0x0C,0x70,0x00,0x71,0xC0,0x00,0x00,0x00,0x70,0x00,0x0E,0x70,0x00,0x38,
0x00,0x3C,0xE0,0x00,0x3F,0xC0,0x0E,0x70,0x00,0x71,0xC0,0x00,0x00,0x00,0x70,0x01,
0x87,0xF0,0x00,0x18,0x00,0x3C,0xE0,0x02,0x1F,0x80,0x0E,0x70,0x00,0x71,0xC0,0x00,
0x00,0x0C,0x70,0x01,0xFF,0xF0,0x00,0x00,0x00,0x38,0xE0,0x0F,0x1F,0x80,0x0E,0xF0,
0x00,0x73,0xD0,0x00,0x00,0x1C,0x70,0x01,0xFF,0xF8,0x00,0x00,0x00,0x38,0xE0,0x07,
0x0F,0xC0,0x0E,0xF8,0x00,0xF3,0xF8,0x00,0x00,0x0E,0xF0,0x03,0xFC,0x78,0x00,0x0E,
0x00,0x38,0xF0,0x07,0x9F,0xE0,0x1E,0xF8,0x00,0xE3,0xF8,0x00,0x00,0x0F,0xF0,0x03,
0xF8,0xF0,0x00,0x1F,0x00,0x70,0xFC,0x03,0xFF,0xE0,0x1C,0xFC,0x00,0xEF,0xF0,0x00,
0x00,0x0F,0xE0,0x03,0xB8,0xE0,0x03,0xFF,0x80,0x71,0xFC,0x03,0xFF,0xC0,0x1D,0xFC,
0x00,0xFF,0xC0,0x00,0x00,0x07,0xE6,0x03,0xBB,0xC0,0x03,0xFF,0x80,0x77,0xF8,0x01,
0x3F,0xC0,0x1D,0xDE,0x01,0xFF,0x80,0x00,0x00,0x06,0xFE,0x03,0xBF,0x80,0x03,0xFF,
0x00,0x77,0xE0,0x00,0x0F,0x80,0x39,0xCE,0x01,0xC3,0x80,0x00,0x00,0x06,0xFF,0x03,
0xBF,0x80,0x03,0x8F,0x00,0xF3,0xC0,0x00,0x7F,0xC0,0x3B,0xCF,0x01,0xC3,0xE0,0x00,
0x00,0x03,0xFF,0x03,0x7F,0x80,0x03,0x8E,0x00,0xF1,0xCE,0x03,0x7F,0xC0,0x3B,0x9F,
0x81,0xCB,0xF0,0x00,0x00,0x3F,0xFF,0x00,0x7F,0x80,0x03,0x8E,0x00,0xF1,0xFF,0x07,
0xFE,0x00,0x7B,0x9F,0xC3,0xCF,0xF0,0x00,0x00,0x7F,0xE7,0x00,0xF7,0x00,0x03,0x8E,
0x01,0xF1,0xFF,0x0F,0xFE,0xC0,0x7F,0xFF,0xE3,0xCF,0xF0,0x00,0x00,0x3F,0xC7,0x00,
0xFF,0x00,0x03,0xBF,0x01,0xF7,0xFF,0x1F,0xFF,0xE0,0xFF,0xDF,0xE3,0xCE,0x70,0x00,
0x00,0x3D,0xC7,0x00,0xFF,0x00,0x03,0xFF,0x01,0xFF,0xF0,0x1F,0xFF,0xE0,0xFF,0xDC,
0x07,0xCF,0xF0,0x00,0x00,0x01,0xC7,0x01,0xDE,0x00,0x03,0xFE,0x03,0xFF,0xC0,0x07,
0x7F,0xF0,0xFF,0xFC,0x07,0xCF,0xF0,0x00,0x00,0x03,0xC7,0x01,0x9E,0x00,0x03,0xF8,
0x03,0xFF,0x80,0x07,0x7E,0xE1,0xDD,0xFC,0x07,0xCF,0xF0,0x00,0x00,0x03,0xEF,0x00,
0x1F,0x00,0x07,0x80,0x03,0xF1,0xB8,0x07,0x3E,0xE1,0x9D,0xFC,0x0E,0xCE,0x70,0x00,
0x00,0x03,0xEF,0x00,0x3F,0x80,0x07,0x80,0x07,0x73,0xF8,0x07,0x1E,0xE1,0x99,0xDC,
0x0D,0xCF,0x70,0x00,0x00,0x03,0xFF,0x00,0x3B,0xC0,0x07,0x00,0x06,0x73,0xFC,0x07,
0x3F,0xE0,0x19,0xDC,0x0D,0xCF,0xF0,0x00,0x00,0x07,0x7E,0x00,0x79,0xF0,0x07,0x00,
0x00,0x73,0xFC,0x07,0x3F,0xE0,0x19,0xDC,0x01,0xCF,0xF0,0x00,0x00,0x07,0x7E,0x00,
0x71,0xFC,0x07,0x00,0x00,0x73,0xFC,0x07,0x7F,0xE0,0x39,0xDC,0x01,0xCF,0x70,0x00,
0x00,0x0F,0x6E,0x00,0xE3,0xFF,0x07,0x00,0x00,0x73,0xB8,0x07,0x7F,0xC0,0x39,0xDC,
0x01,0xCE,0x70,0x00,0x00,0x0E,0x0E,0x01,0xFF,0xFF,0x07,0x00,0x00,0x71,0x38,0x03,
0xEE,0xC0,0x39,0xDC,0x01,0xCF,0xF0,0x00,0x00,0x0E,0x0E,0x07,0xFF,0xFC,0x07,0x00,
0x00,0x70,0x38,0x03,0xCE,0x00,0x39,0xDC,0x01,0xCF,0xF0,0x00,0x00,0x1C,0x0E,0x07,
0xFF,0xD8,0x0F,0x00,0x00,0x73,0x70,0x0F,0xCE,0x00,0x39,0x9C,0x01,0xCF,0x70,0x00,
0x00,0x1C,0xDE,0x0E,0x73,0x80,0x0E,0x00,0x00,0x73,0xF0,0x1F,0xFE,0x00,0x3B,0x9C,
0x01,0xCE,0x70,0x00,0x00,0x3C,0xFC,0x1C,0x73,0x80,0x0E,0x00,0x00,0x71,0xF0,0x1F,
0xFE,0x00,0x3B,0x9C,0x01,0xCE,0xFE,0x00,0x00,0x78,0x7C,0x08,0x73,0x80,0x0E,0x00,
0x00,0x71,0xE0,0x0E,0x7F,0xFC,0x3B,0x9C,0x01,0xCF,0xFE,0x00,0x00,0x70,0x7C,0x00,
0x7F,0x80,0x1C,0x00,0x00,0x70,0xF0,0x0C,0x3F,0xFC,0x3B,0x1C,0x01,0xFF,0xFE,0x00,
0x00,0xE0,0x3C,0x00,0x7F,0x80,0x1C,0x00,0x00,0x70,0xF0,0x00,0x0F,0xF0,0x3F,0x1C,
0x01,0xFF,0xFE,0x00,0x00,0xE0,0x3C,0x00,0x7F,0x80,0x3C,0x00,0x00,0x30,0x70,0x00,
0x07,0xF0,0x3E,0x18,0x01,0xFE,0x00,0x00,0x00,0xC0,0x38,0x00,0x78,0x00,0x38,0x00,
0x00,0x30,0x70,0x00,0x03,0xC0,0x38,0x18,0x01,0xC0,0x00,0x00,0x00,0x00,0x30,0x00,
0x70,0x00,0x70,0x00,0x00,0x00,0x30,0x00,0x00,0x00,0x18,0x18,0x00,0xC0,0x00,0x00,
0x00,0x00,0x20,0x00,0x30,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,
0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
/**
* @brief		得到文件长度
* @param pf		文件索引指针
* @return		返回的文件长度
*/
static int inline GetFilelen(FILE *pf)
{
	long lstart, lend;

	fseek(pf, 0, SEEK_END);  ///重定位流上的文件指针
	lend = ftell(pf);       ///得到文件当前位置相对于文件首的偏移字节数
	fseek(pf, 0, SEEK_SET);
	lstart = ftell(pf);

	return(lend-lstart);
}

/**
* @brief           复制汉字库到缓冲区
* @param filename  复制源文件索引指针
* @param buf       复制目的缓冲区指针
* @param maxlen    允许复制的最大长度
* @return          返回的文件指针
*/
static void CopyHzk(char *filename, unsigned char *buf, int maxlen)   
{
	FILE *pf;
	int i;

	pf = fopen(filename, "rb");  
	if(NULL == pf) {
		PrintLog(0, "can not open %s.\r\n", filename);
		return;
	}

	i = GetFilelen(pf);   
	PrintLog(0, "read %s (%d Bytes)\r\n", filename, i); 
	if(i > maxlen) {                                             
		PrintLog(0, "file too large.\r\n");  
		fclose(pf);   
		return; 
	} 
    i = fread(buf, 1, 0x40000, pf);
 
	fclose(pf);  ///关闭文件 
}
/**
* @brief           复制ASCII库到缓冲区
* @param filename  复制源文件索引指针
* @param buf       复制目的缓冲区指针
* @param maxlen    允许复制的最大长度
* @return          返回的文件指针
*/
static void CopyAsc(char *filename, unsigned char *buf, int maxlen)   
{
	FILE *pf;
	int i;

	pf = fopen(filename, "rb");  
	if(NULL == pf) {
		PrintLog(0, "can not open %s.\r\n", filename);
		return;
	}

	i = GetFilelen(pf);   
	PrintLog(0, "read %s (%d Bytes)\r\n", filename, i); 
	if(i > maxlen) {                                             
		PrintLog(0, "file too large.\r\n");  
		fclose(pf);   
		return; 
	} 
    i = fread(buf, 1, 4096, pf);
	///i = fread(buf, 1, i, pf);  ///将文件流读入到buf进行保存 
 
	fclose(pf);  ///关闭文件 
}

#if 1
/**
* @brief           显示一行汉字或ascii
* @param str       显示缓冲区首地址
* @param len       显示缓冲区首长度
* @param x         显示的x横坐标
* @param y         显示的y纵坐标
*/
void DisplayNormal(const unsigned char *str,unsigned char len,unsigned char x,unsigned char y)
//void DisplayNormal(const unsigned char *str,unsigned char len,unsigned char y,unsigned char x)
{
	unsigned char *pfill;
	unsigned char i;
	unsigned int pos, j;

	y /= 8;

	pfill = lcd_buffer + (unsigned int)x*20 + (unsigned int)y;

	for(i=0; i<len; i++) 
	{
		if((str[0]< 161) || (str[1] < 161)) 
		{
			pos = (unsigned int)str[0] * 14;//得到字体在字库中的偏移量
			str++;//移至下一个
			if(DISINVS==0)
			{
	            		for(j=0; j<1; j++) 
				{
					pfill[j*ONE_LINE_BYTE] = 0;//j用来控制要显示的那个字的层
				}
				for(; j < 15; j++) 
				{
					pfill[j*ONE_LINE_BYTE] = myasc[pos + j-1];
	            		}
				for(; j<16; j++) 
				{
					pfill[j*ONE_LINE_BYTE] = 0;
				}
            		}
			else
			{
				for(j=0; j<1; j++) 
				{
					pfill[j*ONE_LINE_BYTE] = 0xff;
				}
				for(j = 0; j < 16; j++) 
				{
					pfill[j*ONE_LINE_BYTE] = (~myasc[pos + j-1]);
				}
				for(; j<16; j++) 
				{
					pfill[j*ONE_LINE_BYTE] = 0xff;
				}
			}
			pfill += 1;
		}
		else
		{
			pos = (unsigned int)(str[0]-161)*94 + (unsigned int)(str[1]-161);
			pos *= 28;
			str += 2;
			i++;
			if(DISINVS==0)
			{
				for(j=0; j<1; j++)
				{
					pfill[j*ONE_LINE_BYTE] = 0;
					pfill[j*ONE_LINE_BYTE+1] = 0;
				}
				for(; j < 15; j++) 
				{
					pfill[j*ONE_LINE_BYTE] = hzk_gb2312[pos + (j-1) * 2];
					pfill[j*ONE_LINE_BYTE+1] = hzk_gb2312[pos + (j-1) * 2 + 1];
	            		}
				for(; j<16; j++)
				{
					pfill[j*ONE_LINE_BYTE] = 0;
					pfill[j*ONE_LINE_BYTE+1] = 0;
				}
			}
			else
			{
				for(j=0; j<1; j++) 
				{
					pfill[j*ONE_LINE_BYTE] = 0xff;
					pfill[j*ONE_LINE_BYTE+1] = 0xff;
				}
				for(; j < 15; j++) 
				{
					pfill[j*ONE_LINE_BYTE] = (~hzk_gb2312[pos + (j-1) * 2]);
					pfill[j*ONE_LINE_BYTE+1] = (~hzk_gb2312[pos + (j-1) * 2 + 1]);
	            		}
				for(; j<16; j++) 
				{
					pfill[j*ONE_LINE_BYTE] = 0xff;
					pfill[j*ONE_LINE_BYTE+1] = 0xff;
				}
			}
			pfill += 2;
		}
	}
	DISINVS = 0;
} 

#endif


#if 0
void DisplayNormal(const unsigned char *str,unsigned char len,unsigned char x,unsigned char y)
//void DisplayNormal(const unsigned char *str,unsigned char len,unsigned char y,unsigned char x)
{
	unsigned char *pfill;
	unsigned char i;
	unsigned int pos, j;

	y /= 8;

	pfill = lcd_buffer + (unsigned int)x*20 + (unsigned int)y;

	for(i=0; i<len; i++) 
	{
		if((str[0]< 161) || (str[1] < 161)) 
		{
			pos = (unsigned int)str[0] * 14;
			str++;
			if(DISINVS==0)
			{
	            		for(j=0; j<1; j++) 
				{
					pfill[j*ONE_LINE_BYTE] = 0;
				}
				for(; j < 15; j++) 
				{
					pfill[j*ONE_LINE_BYTE] = myasc[pos + j-1];
	            		}
				for(; j<16; j++) 
				{
					pfill[j*ONE_LINE_BYTE] = 0;
				}
            		}
			else
			{
				for(j=0; j<1; j++) 
				{
					pfill[j*ONE_LINE_BYTE] = 0xff;
				}
				for(j = 0; j < 16; j++) 
				{
					pfill[j*ONE_LINE_BYTE] = (~myasc[pos + j-1]);
				}
				for(; j<16; j++) 
				{
					pfill[j*ONE_LINE_BYTE] = 0xff;
				}
			}
			pfill += 1;//纵向向右移动一个字节
		}
		else
		{
			pos = (unsigned int)(str[0]-161)*94 + (unsigned int)(str[1]-161);
			pos *= 28;
			str += 2;
			i++;
			if(DISINVS==0)
			{
				for(j=0; j<1; j++)
				{
					pfill[j*ONE_LINE_BYTE] = 0;
					pfill[j*ONE_LINE_BYTE+1] = 0;
				}
				for(; j < 15; j++) 
				{
					pfill[j*ONE_LINE_BYTE] = hzk_gb2312[pos + (j-1) * 2];
					pfill[j*ONE_LINE_BYTE+1] = hzk_gb2312[pos + (j-1) * 2 + 1];
	            		}
				for(; j<16; j++)
				{
					pfill[j*ONE_LINE_BYTE] = 0;
					pfill[j*ONE_LINE_BYTE+1] = 0;
				}
			}
			else
			{
				for(j=0; j<1; j++) 
				{
					pfill[j*ONE_LINE_BYTE] = 0xff;
					pfill[j*ONE_LINE_BYTE+1] = 0xff;
				}
				for(; j < 15; j++) 
				{
					pfill[j*ONE_LINE_BYTE] = (~hzk_gb2312[pos + (j-1) * 2]);
					pfill[j*ONE_LINE_BYTE+1] = (~hzk_gb2312[pos + (j-1) * 2 + 1]);
	            		}
				for(; j<16; j++) 
				{
					pfill[j*ONE_LINE_BYTE] = 0xff;
					pfill[j*ONE_LINE_BYTE+1] = 0xff;
				}
			}
			pfill += 2;
		}
	}
	DISINVS = 0;
} 
#endif







static char HaveAlarmInfo = 0;
static char AlarmInfo[21];
static int AlarmInfoCount = 0;
static unsigned char AlarmFlagErc = 0;

#if 0
static void ShowAlarmInfo(void)
{
	int len;

	len = strlen(AlarmInfo);
	if(len > 20 || len == 0) return;

	DisplayNormal(AlarmInfo, len, BTM_LINE_ROW+1, 0); 
}
#endif

/**
* @brief           显示告警
* @param str       显示缓冲区首地址
*/
void ScreenDisplayAlarm(const char *str)
{
	int len;

	len = strlen(str);

	if(len > 20 || len == 0) {
		ErrorLog("alarm info too long\n");
		return;
	}

	HaveAlarmInfo = 1;
	AlarmInfoCount = 0;
	strcpy(AlarmInfo, str);
} 




/**
* @brief     写缓冲区数据到lcd
*/
void DisplayLcdBuffer(void)
{

	int i ,j,k,w,n,cnt;
	unsigned char lcd_buffer_tmp[LCD_BUFFER_SIZE];
	unsigned char lcd_buffer_tmp1[LCD_BUFFER_SIZE];

	memset(lcd_buffer_tmp,0x00,LCD_BUFFER_SIZE);
	memset(lcd_buffer_tmp1,0x00,LCD_BUFFER_SIZE);
	w=7;
	k=0;
	i = 1;
	cnt = 0;
	for(n=1;n<=20;n++)
	{
		i=1;
		for(w=0;w<8;w++)	
		{
			i=1;
			while(i<160)
			{
				for(j=7;j>=0;j--)	
				{
					if(lcd_buffer[k]&(0x01<<j))
					{
						lcd_buffer_tmp[i * 20 - n] |= (0x01<<w);
					}
					i++;
					if(((i-1)%8) == 0)
					{
						k++;
					}
				}
			}
			cnt++;
		}
	}

	write(fd_lcd,lcd_buffer_tmp, LCD_BUFFER_SIZE); 
}







void set_screen_logo(void)
{
	int i ,j,k,w,n,cnt;
	unsigned char lcd_buffer_tmp[LCD_BUFFER_SIZE];
	memset(lcd_buffer_tmp,0x00,LCD_BUFFER_SIZE);

	w=7;
	k=0;
	i = 1;
	cnt = 0;
	for(n=1;n<=20;n++)
	{
		i=1;
		//w =7;
		//for(w=7;w>=0;w--)
		for(w=0;w<8;w++)	
		{
			i=1;
			while(i<160)
			{
				///for(j=0;j<8;j++)
				for(j=7;j>=0;j--)	
				{
					if(screen_logo[k]&(0x01<<j))
					{
						lcd_buffer_tmp[i * 20 - n] |= (0x01<<w);
					}
					i++;
					if(((i-1)%8) == 0)
					{
						k++;
					}
				}
			}
			cnt++;
		}
	}


	//printf("k = %d\n",k);
	//printf("cnt = %d\n",cnt);

	write(fd_lcd,lcd_buffer_tmp, LCD_BUFFER_SIZE); 
}


/**
* @brief    设置反显标志
*/
void SetInvs(unsigned char flag)
{
	if(flag)
		DISINVS = 1;
	else 
		DISINVS = 0;
}

/**
* @brief 清除显示缓冲区
*/

void ClearLcdScreen(void)
{
	memset(lcd_buffer, 0, LCD_BUFFER_SIZE);
}


/**
* @brief	取得按键值
* @return	得到的按键值
*/
void ClearLcdBackLightConut(void);
static unsigned char lcd_bak_light = 0;
char LcdGetKey(void)                                                                                                                                    
{   
	char key_num;
	
    	key_num = 0;
	key_num = SiginGetChar();
	if(0 != key_num) 
	{
		ClearLcdBackLightConut();
		LcdBakLight(1);
		lcd_bak_light = 1;
		//printf("key_num = %d\n",key_num);
	}
	
	return key_num;
}

char GetLcdBakLightState(void)   
{
	return lcd_bak_light;
}


void CloseLcdBakLightState(void)   
{
	lcd_bak_light = 0;
}

/**
* @brief 打开或关闭背景灯
* @param ison 操作方式, 1-打开, 0-关闭
*/
void LcdBakLight(int ison)
{
	if(ison) 
	{
		set_io(AT91_PIN_PB30,1);//打开背景灯命令
	}
	else 
	{
		set_io(AT91_PIN_PB30,0);
	}
}

extern unsigned char loopshowfalg;
extern unsigned char check_major_met_data;
extern unsigned char GprsSigQuality(void);
extern unsigned char BatCapacity(void);
/**
* @brief 显示最上面一行的图标和时间
*/
extern unsigned char Scrp_Head;

static char ScrUpChannel = 'G';

static void ShowChannel(void)
{
	int i, j;

	for(i=0;i<15;i++)
		topm_buf[2][i]=mod_topm[ScrUpChannel-65][i*2+1];
	for(i=0;i<15;i++)
		topm_buf[3][i]=mod_topm[ScrUpChannel-65][i*2];
	for(i=0;i<16;i++)
		for(j=2;j<=3;j++)
    		lcd_buffer[20*i+j]=topm_buf[j][i];
}

#if 0
/**
* @brief 显示告警标志
* @param flag 1-显示标志, 0-清除标志
*/
static void ShowAlarmFlag(int flag)
{
	int i, j;

	if(flag) {
		for(i=0;i<16;i++)
			topm_buf[5][i]=almpte[2*i];
		for(i=0;i<16;i++)
			topm_buf[6][i]=almpte[2*i+1];
		for(i=0;i<16;i++) {
			for(j=5;j<=6;j++) lcd_buffer[20*i+j]= topm_buf[j][i];
		}
	}
	else {
		for(i=0;i<16;i++)
			topm_buf[5][i]=0;
		for(i=0;i<16;i++)
			topm_buf[6][i]=0;
		for(i=0;i<16;i++) {
			for(j=5;j<=6;j++) lcd_buffer[20*i+j]= topm_buf[j][i];
		}
	}
}
#endif
void ShowTopFlag(void)
{
#define BTM_LIN_START		(BTM_LINE_ROW * 20)
struct wr_slines_t {
	unsigned short x;
	unsigned short len;
	unsigned char *buffer;
};

	unsigned int i;
	//struct wr_slines_t wrconf;

	for(i = 0;i < 20;i++) lcd_buffer[BTM_LIN_START+i] = 0xff;
	ScreenSetHeadFlag(SCRHEAD_FLAG_SIG, GprsSigQuality());
	//ScreenSetHeadFlag(SCRHEAD_FLAG_BAT, BatCapacity());
	ShowChannel();
	ShowTime();
#if 0
/*
	if(AlarmFlagErc) ShowAlarmFlag(1);
	else ShowAlarmFlag(0);

	wrconf.x = 0;
	wrconf.len = 16*20;
	wrconf.buffer = lcd_buffer;

	ioctl(fd_lcd, 20, &wrconf);

	if(HaveAlarmInfo) {
		ShowAlarmInfo();
		wrconf.x = BTM_LINE_ROW+1;
		wrconf.len = 16*20;  
		wrconf.buffer = lcd_buffer + (BTM_LINE_ROW+1)*20;
		
		ioctl(fd_lcd, 20, &wrconf);

		AlarmInfoCount++;
		if(AlarmInfoCount > 666) { // 10 min
			AlarmInfoCount = 0;
			HaveAlarmInfo = 0;
			memset(lcd_buffer+(BTM_LINE_ROW+1)*20, 0, 16*20);
		}
	}
*/
	//write(fd_lcd,lcd_buffer_tmp, LCD_BUFFER_SIZE); 
#endif
	DisplayLcdBuffer();
}
/**
* @brief 	反显某行
* @param x 	反显的起始横坐标
*/

void  SetLineInvs(unsigned char x)
{	
	unsigned char i,j;
	int lcd_buffer_pos=x*20;
	
	for(i = 0;i < 16;i++)
		for(j = 0;j < 20;j++)
			lcd_buffer[20*i+j+lcd_buffer_pos] = (~lcd_buffer[20*i+j+lcd_buffer_pos]);
}

/*
void  SetLineInvs(unsigned char x,unsigned char y,unsigned char len)
{	
	unsigned char i,j;
	int lcd_buffer_pos=x*20;
	
	for(i = 0;i < 16;i++)
		for(j = (y/8);j < (y/8+len);j++)
			lcd_buffer[20*i+j+lcd_buffer_pos] = (~lcd_buffer[20*i+j+lcd_buffer_pos]);
}
*/
/**
* @brief           清除部分屏幕
* @param x         要清除的起始x横坐标
* @param len       要清除的行数
*/
void  CleanPartOfScreen(unsigned char x ,unsigned char len)
{

	unsigned char i;
	int lcd_buffer_pos=x*20;
	
	for(i = 0;i < len;i++)
		memset(&lcd_buffer[lcd_buffer_pos+i*320], 0, 320);
}


#if 0
/**
* @brief 显示时钟
*/
void ShowTime(void)
{		
	unsigned char ch;
	int i,j;
	sysclock_t clock;
	
	SysClockReadCurrent(&clock);
	ch=(clock.hour%24)/10;
	memcpy(topm_buf[14], &asc10x8[ch*16], 16);
	ch = clock.hour%10;
	memcpy(topm_buf[15], &asc10x8[ch*16], 16);
	
    for(i=0;i<16;i++)
    {
    	topm_buf[14][i]=(topm_buf[14][i]>>1);
		if((i==6)||(i==9))
			topm_buf[15][i]=(topm_buf[15][i]|0x01);
    }
    
	ch=(clock.minute %60)/10;
	memcpy(topm_buf[16], &asc10x8[ch*16], 16);
	ch = clock.minute%10;
	memcpy(topm_buf[17], &asc10x8[ch*16], 16);
	
    for(i=0;i<16;i++)
    {
    	topm_buf[16][i]=(topm_buf[16][i]>>1);
		if((i==6)||(i==9))
			topm_buf[17][i]=(topm_buf[17][i]|0x01);
	}
    
    ch=(clock.second % 60)/10;
	memcpy(topm_buf[18], &asc10x8[ch*16], 16);
	ch = clock.second%10;
	memcpy(topm_buf[19], &asc10x8[ch*16], 16);
	
	for(i=0;i<16;i++)
	{
		topm_buf[18][i]=(topm_buf[18][i]>>1);
	}
	    
	for(i=0;i<12;i++)
		for(j=12;j<20;j++)
    		lcd_buffer[20*i+j]=topm_buf[j][i];
}
#endif




/**
* @brief 显示时钟
*/
void ShowTime(void)
{		
	unsigned char ch;
	int i,j;
	sysclock_t clock;
	
	SysClockReadCurrent(&clock);
	ch=(clock.hour%24)/10;
	memcpy(topm_buf[16], &asc10x8[ch*16], 16);
	ch = clock.hour%10;
	memcpy(topm_buf[17], &asc10x8[ch*16], 16);
	
    for(i=0;i<16;i++)
    {
    	topm_buf[14][i]=(topm_buf[14][i]>>1);
		if((i==6)||(i==9))
			topm_buf[17][i]=(topm_buf[17][i]|0x01);
    }
    
	ch=(clock.minute %60)/10;
	memcpy(topm_buf[18], &asc10x8[ch*16], 16);
	ch = clock.minute%10;
	memcpy(topm_buf[19], &asc10x8[ch*16], 16);


	/*
    for(i=0;i<16;i++)
    {
    	topm_buf[16][i]=(topm_buf[16][i]>>1);
		if((i==6)||(i==9))
			topm_buf[17][i]=(topm_buf[17][i]|0x01);
	}
    
    ch=(clock.second % 60)/10;
	memcpy(topm_buf[18], &asc10x8[ch*16], 16);
	ch = clock.second%10;
	memcpy(topm_buf[19], &asc10x8[ch*16], 16);
	
	for(i=0;i<16;i++)
	{
		topm_buf[18][i]=(topm_buf[18][i]>>1);
	}
	*/
	    
	for(i=0;i<12;i++)
		for(j=12;j<20;j++)
    		lcd_buffer[20*i+j]=topm_buf[j][i];
}




/**
* @brief           显示图标
* @param flag      显示何种图标
* @param arg       图标的参数
*/
void ScreenSetHeadFlag(unsigned char flag,unsigned int arg)
{
	int i,j;
	unsigned char met;

	switch(flag)
	{
	case SCRHEAD_FLAG_SIG: 
		if(arg > 4) arg = 0;

		for(i=0;i<15;i++)
			topm_buf[0][i]=sig_topm[arg][i*2];
		for(i=0;i<15;i++)
			topm_buf[1][i]=sig_topm[arg][i*2+1];

		for(i=0;i<16;i++)
			for(j=0;j<=1;j++)
        		lcd_buffer[20*i+j]=topm_buf[j][i];
		break;

	case SCRHEAD_FLAG_CHN: 
		if(arg >= 'A' && arg <= 'Z') ScrUpChannel = (char)arg;
		break;

	case SCRHEAD_FLAG_ALM:         ///显示报警图标
		AlarmFlagErc = (unsigned char)arg;
		break;
			
	case SCRHEAD_FLAG_CLRALM:      ///清除报警图标
		if(AlarmFlagErc == (unsigned char)arg) AlarmFlagErc = 0;
		break;

	case SCRHEAD_FLAG_METID: 
		met = arg;
		if(arg>=0&&arg<=7)
		{
			for(i=0;i<16;i++)
			{
				topm_buf[8][i]=met_id[met][i*3];
				topm_buf[8][i]|=window[i*3];
			 }
			for(i=0;i<16;i++)
			{
				topm_buf[9][i]=met_id[met][i*3+1];
				topm_buf[9][i]|=window[i*3+1];
			 }
			for(i=0;i<16;i++)
			{
				topm_buf[10][i]=met_id[met][i*3+2];
		        topm_buf[10][i]|=window[i*3+2];
			 }
		}
		for(i=0;i<16;i++)
			for(j=8;j<=10;j++)
        		lcd_buffer[20*i+j]=topm_buf[j][i];
		break;

	case SCRHEAD_FLAG_BAT:
		for(i=0;i<15;i++)
			topm_buf[11][i]=bat_topm[arg][i*2];
		for(i=0;i<15;i++)
			topm_buf[12][i]=bat_topm[arg][i*2+1];
		for(i=0;i<16;i++)
			for(j=11;j<=12;j++)
        		lcd_buffer[20*i+j]=topm_buf[j][i];
		break;

	default: break;
	}

	///for(i=0;i<16;i++)
		///for(j=0;j<14;j++)
        	///lcd_buffer[20*i+j]=topm_buf[j][i];
}

/**
* @brief          显示页面号码
* @param number   要显示的数字
* @param x        显示的横坐标
*/
void DisplayPageNumber(int number,unsigned char x)
{
	unsigned char i,j;	
	
	for(i=14,j=0;i<=22&&j<8;i++,j++)
        lcd_buffer[20*i+x]=page_munber[number*8+j];
}

void DisplayReadMeter(int number,unsigned char x)
{
	unsigned char i,j;	
	
	for(i=14,j=BTM_LINE_ROW;i<=22&&j<BTM_LINE_ROW+8;i++,j++)
        lcd_buffer[20*i+x]=page_munber[number*8+j];
}


/**
* @brief        清除显示的号码
*/
void  CleanPageNum(void)
{
	unsigned char i,j;	
	
	for(j=16;j<=18;j++)
		for(i=13;i<=21;i++)
			lcd_buffer[20*i+j]=0;
    
}

/**
* @brief        清除轮显的表计号
*/
void CleanLoopMetNum(void)
{
	unsigned char i,j;
	for(i=0;i<16;i++)
		for(j=8;j<11;j++)
		{
        	topm_buf[j][i]=0;
				lcd_buffer[20*i+j]=0;
		}
}
/**
* @brief        清除图标显示缓冲区
*/
void ClearTopBuffer(void)
{
	unsigned char i,j;
	for(i=0;i<16;i++)
		for(j=0;j<20;j++)
		{
        		topm_buf[j][i]=0;
			lcd_buffer[20*i+j]=0;
		}
}

/**
* @brief 清除菜单显示缓存区
*/
void ClearMenuBuffer(void)
{
	memset(lcd_buffer+TOP_LINE_ROW*20, 0, (BTM_LINE_ROW-TOP_LINE_ROW)*20);
}

/**
* @brief 检查LCD状态
*/
void CheckLcd(void)
{
	static unsigned int orgstate = 0;
	static int firstflag = 1;
	static unsigned int state;

	if(firstflag) {
		firstflag = 0;
		ioctl(fd_lcd, 5, &orgstate);
		DebugPrint(0, "lcd org state = 0x%08X\n", orgstate);
	}

	ioctl(fd_lcd, 5, &state);
	//DebugPrint(0, "state = 0x%08X\n", state);

	if(state != orgstate) {
		PrintLog(LOGTYPE_ALARM, "!!lcd state incorrect, reset(0x%08X, 0x%08X)\n", orgstate, state);
		ScreenDisplayAlarm("屏幕复位!!");

		firstflag = 1;
		ioctl(fd_lcd, 2, 0);  //power off
		Sleep(30);
		ioctl(fd_lcd, 3, 0);  //power on
		Sleep(10);
		ioctl(fd_lcd, 4, 0);  //reset
	}
}

/**
* @brief	初始化lcd
* @return	返回0初始化lcd成功  返回1初始化lcd失败
*/

int InitLcd()
{
	//fd_lcd = open("/dev/lcd", O_RDWR);  
	//fd_lcd = open("/dev/lcd", O_RDWR);  	
	fd_lcd = open("/dev/fb0", O_RDWR);  
	
		

	if(-1 == fd_lcd) 
	{
		PrintLog(0, "can not open lcd driver.\r\n");
		return 1;
	}
	else
	{
		printf("open lcd driver succ\n");
	}

	CopyHzk("/home/nandflash/work/chs.bin", hzk_gb2312, GB2312_HZK_SIZE); 
	//CopyHzk("/home/test90.hzk", hzk_gb2312, GB2312_HZK_SIZE); 
	
	CopyAsc("/home/nandflash/work/14x8.hzk",myasc, ASCII_SIZE); 

	LcdBakLight(1);      //点亮lcd背景灯
	lcd_bak_light = 1;
	//printf("set_screen_logo...1\n");
	//set_screen_logo();
	//write(fd_lcd, screen_logo, 3200); 
	//usleep(3000000);  ///延时3秒
	///LcdBakLight(0);
	ClearLcdScreen();
	ClearTopBuffer();
	return 0;
}




