#ifndef _GH_EPAPER_H
#define _GH_EPAPER_H

#include <stdio.h>
#include "ql_gpio.h"
#include "ql_spi.h"


/* Switch of Debug */
#define DEBUG_LOG TRUE

#if (defined DEBUG_LOG) && (DEBUG_LOG == TRUE)
#define gh_debug_log(format, ...) 	    printf("[%s Line:%d]: "format"\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define gh_debug_log(format, ...)
#endif

// LUT_DEPG73_OTP

#define FR_100HZ 0x3A
#define FR_86HZ  0x32
#define FR_71HZ  0x2A
#define FR_50HZ  0x3C

#define VS_3V6  0x03
#define VS_4V   0x05
#define VS_4V2  0x06
#define VS_4V4  0x07
#define VS_4V6  0x08
#define VS_4V8  0x09
#define VS_5V   0x0a
#define VS_5V2  0x0b
#define VS_5V4  0x0c
#define VS_5V6  0x0D
#define VS_5V8  0x0E
#define VS_6V   0x0f
#define VS_6V2  0x10
#define VS_6V4  0x11
#define VS_7V   0x14
#define VS_7V2  0x15
#define VS_7V4  0x16
#define VS_7V6  0x17
#define VS_7V8  0x18
#define VS_8V   0x19
#define VS_9V   0x1E
#define VS_10V  0x23
#define VS_11V  0x28
#define VS_15V  0x3c

#define  TEMP_0  0
#define  TEMP_1  0x02
#define  TEMP_2  0x04
#define  TEMP_3  0x06
#define  TEMP_4  0x08
#define  TEMP_5  0x0a
#define  TEMP_6  0x0c
#define  TEMP_10 0x14
#define  TEMP_15 0x1e
#define  TEMP_20 0x28
#define  TEMP_25 0x32
#define  TEMP_30 0x3c
#define  TEMP_35 0x46
#define  TEMP_40 0x50
#define  TEMP_45 0x5a
#define  TEMP_50 0x64

// IO definitions for Epaper
// SPI
#define EPAPER_SPI_CK	GPIO_PIN_NO_33 	// 时钟
#define EPAPER_SPI_DA 	GPIO_PIN_NO_35 	// 数据
#define EPAPER_SPI_DC	GPIO_PIN_NO_6   // GPIO_PIN_NO_36 	// D/C
#define EPAPER_SPI_CS1	GPIO_PIN_NO_34 	// 片选
#define EPAPER_SPI_CS2	GPIO_PIN_NO_31 	// 片选

// Buttons
#define EPAPER_BUTTON1 	GPIO_PIN_NO_5 	// 按键1：赞成
#define EPAPER_BUTTON2 	GPIO_PIN_NO_17 	// 按键2：反对
#define EPAPER_BUTTON3 	GPIO_PIN_NO_19 	// 按键3：纸笔/呼叫
#define EPAPER_BUTTON4 	GPIO_PIN_NO_16 	// 按键4：签到

// Others
#define EPAPER_RST 		GPIO_PIN_NO_32 	// 复位
#define EPAPER_BUSY1	GPIO_PIN_NO_11 	// BUSY检测
#define EPAPER_BUSY2 	GPIO_PIN_NO_10 	// BUSY检测
#define EPAPER_LINK_LED	GPIO_PIN_NO_18 	// 信号灯

// Peripheral configurations
// SPI parameters
#define EPAPER_SPI_PORT QL_SPI_PORT1	// SPI通道
#define EPAPER_SPI_MODE QL_SPI_MODE0	// SPI模式
#define EPAPER_SPI_FREQ	QL_SPI_CLK_1_625MHZ // SPI时钟频率

// Epaper information
#define ALLSCREEN_GRAGHBYTES    30720	
#define EACHBAR_GRAGHBYTES      6080 	//(640*384/5)/8
#define LEFTBAR_GRAGHBYTES      6400 	//(640*384/5)/8
#define ALLLINE_NUM    			384
#define EACHLINE_BYTES 			80	 	//640/8

void gh_epaper_power_on(void);
void gh_key_timer_setup(void); // 创建并启动定时器用于按键检测
void gh_key_timer_stop(void);  // 停止用于按键检测的定时器

#endif
