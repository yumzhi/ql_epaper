Development of epaper application based on EC600N-CN.

## 1.固件生成

在`SDK`根目录下运行`build.bat firmware`即可，生成的固件包位于`target`目录下。

采用一键编译时由于先执行了清除操作，而在编译时未能生成`bootloader`镜像`boot33.bin`，导致最终未能得到固件包。

未能生成`bootloader`镜像的原因待查。当前未修改`bootloader`，因此不需要单独编译`bootloader`，生成固件时将使用`ql-config\quec-project\aboot\images`目录中的`boot33.bin`镜像。

## 2.固件烧录

安装USB驱动：`Quectel_ASR_Series_UMTS&LTE_Windows_USB_Driver_Customer_V1.0.8`。

连接USB接口，打开QFlash，选择固件，点击`start`。

[注意]在下载时最好断开串口连接，先启动下载，然后快速连接USB下载线。
如果识别不了下载端口，在启动下载软件`QFlash`后重新插拔一次`USB`端口。设备管理器中显示的端口名称为`Quectel Download Port`。

## 3.驱动开发

### 1.在`APP`中添加`task`。

1. 在`ql-sdk\ql-application\threadx\interface`下新建自定义目录`ql_epaper`。

2. 在`ql_epaper`中新建`src`和`inc`目录分别用于存放源文件和头文件。

3. 在`src`目录中新建源文件`epaper_main.c`，用于创建`task`并启动`task`主函数。

   ```c
   #include "ql_debug.h"
   #include "ql_application.h"
   #include <stdio.h>
   
   static void epaper_main(void * argv)
   {
   	qL_debug_log_mode_e mode = ql_get_debug_log_mode();
   
   	ql_debug_log_enable();
   	printf("Hello, world!\r\n");
   
   	ql_debug_log_disable();
   	printf("Hello, world!!\r\n");
   	
   	ql_debug_log_enable();
   	printf("Hello, world!!!\r\n");
   	
   	while(1)
   	{
   		// task主函数驻留
   	}
   }
   
   application_init(epaper_main, "epaper_main", 1, 1);
   ```

4. 在`src`目录中新建Makefile文件。

   ```makefile
   #-------------------------------------------------------------------------------
   # Configure variable
   #-------------------------------------------------------------------------------
   TOP_DIR:=$(TOP_DIR)
   ROOT_DIR:=$(TOP_DIR)/../..
   MAKEFILE_PATH :=$(subst $(strip \),/,$(abspath $(lastword $(MAKEFILE_LIST))))
   
   #-------------------------------------------------------------------------------
   # Configure source code files
   #-------------------------------------------------------------------------------
   SRC_FILES+= \
     epaper_boot.c \
     epaper_main.c \
   
   
   #-------------------------------------------------------------------------------
   # the include directories
   #-------------------------------------------------------------------------------
   INC_DIRS+= \
     -I${TOP_DIR}/ \
     -I${TOP_DIR}/interface/ql_epaper/inc \
     -I${TOP_DIR}/common/include \
     -I${TOP_DIR}/interface/os/inc \
     -I${TOP_DIR}/interface/driver/inc \
   
   #-------------------------------------------------------------------------------
   # Configure compile flag for C
   #-------------------------------------------------------------------------------
   U_FLAGS:=
   
   #-------------------------------------------------------------------------------
   # Configure compile flag for C++
   #-------------------------------------------------------------------------------
   U_CPPFLAGS:=
   
   #-------------------------------------------------------------------------------
   # Configure default macro define
   #-------------------------------------------------------------------------------
   DFLAGS+= \
     -D__TM_ZONE=tm_zone \
     -D__TM_GMTOFF=tm_gmtoff \
   
   #-------------------------------------------------------------------------------
   # Configure link library
   #-------------------------------------------------------------------------------
   U_LIBS:=
   
   #-------------------------------------------------------------------------------
   # include mk file
   #-------------------------------------------------------------------------------
   include ${TOP_DIR}/config/common/makefile.mk
   ```

   [注]如果需要使用`spi`、`rtos`等驱动时，则需要在`INC_DIRS`中添加对应驱动头文件的位置。

5. 将新建目录`interface\ql_epaper\src`添加到`ql-sdk\ql-application\threadx\makefile` 中的`COMMPILE_DIRS`变量中。

   ```makefile
   #-------------------------------------------------------------------------------
   # Configure compile directories
   #-------------------------------------------------------------------------------
   COMMPILE_DIRS:= \
     interface/third_party/ql_src/mbedtls \
     interface/at/src \
     interface/third_party/ql_src/http \
     interface/third_party/mqtt/src \
     interface/third_party/ql_src/ssl \
     interface/ql_epaper/src \
   ```

### 2.在`APP`中添加启动项。

在上述`src`目录中新建源文件`epaper_boot.c`。

```c
/**  
  @file
  epaper_boot.c

  @brief
  quectel boot example program in customer app.

*/
/*============================================================================
  Copyright (c) 2017 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
 =============================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


WHEN        WHO            WHAT, WHERE, WHY
----------  ------------   ----------------------------------------------------
24/12/2019  Nebula.li      create
=============================================================================*/

#include "ql_boot.h"
#include <stdio.h>

static void boot_step1(void * argv)
{
	printf("boot step1\r\n");
}

static void boot_step2(void * argv)
{
	printf("boot step2\r\n");
}

user_boot_init(boot_step1, 1);

user_kernel_init(boot_step2);
```

### 3.编译`APP`。

在`SDK`根目录`ql-sdk`下执行`build.bat app`命令。另：建议在编译之前先运行`build.bat app clean`命令清理一遍，然后再重新编译。

### 4.生成固件

在`SDK`根目录`ql-sdk`下执行`build.bat firmware`命令。

[注意]生成固件时务必关闭`QFlash`，否则会因为`QFlash`占用了原有的固件包而导致不能生成新的固件包。

### 5.运行

断开USB连接，重新给模组上电即可。

[注]如果运行时串口和`USB`共用一个`USB HUB`，很可能会因为供电不足导致串口或者`USB`异常。建议下载程序时断开串口，运行时断开USB并单独供电。

## 4.电子纸驱动移植

1.SPI通信

SPI波特率

SPI时钟频率

SPI通道：QL_SPI_PORT1 //GPIO33-GPIO36

| SPI信号 | 引脚号 | 管脚名     | GPIO   |
| -------- | ------ | ---------- | ------ |
| CLK      | 1      | SPI_CLK    | GPIO_33|
| SDA      | 2      | SPI_RXD    | GPIO_35|
| D/C      | 3      | SPI_TXD    | GPIO_36|
| CS1      | 4      | SPI_CS     | GPIO_34|

2.按键驱动

| 按键序号 | 引脚号 | 管脚名     | GPIO   |
| -------- | ------ | ---------- | ------ |
| SW1      | 51     | W_DISABLE# | GPIO_5 |
| SW2      | 58     | PCM_SYNC   | GPIO_17|
| SW3      | 59     | PCM_DIN    | GPIO_19|
| SW4      | 61     | PCM_CLK    | GPIO_16|

3.其他管脚

| 信号  | 引脚号 | 管脚名   | GPIO    |
| ----- | ------ | -------- | ------- |
| ADC   | 19     | ADC      | ADC0    |
| BUSY1 | 56     | I2C_SDA  | GPIO_11 |
| BUSY2 | 57     | I2C_SCL  | GPIO_10 |
| LINK  | 60     | PCM_DOUT | GPIO_18 |
| RST#  | 69     | GPIO1    | GPIO_32 |
| CS2   | 70     | GPIO2    | GPIO_31 |

[注]管脚号对应关系见`EC600N-CN Open 文档_0518\3-软件开发\14-其他\EC600N-CN_QuecOpen_GPIO_Assignment_Speadsheet-_Preliminary_20210207.xlsx`。

4.添加定时器

```c
// Setup a timer for key detection
static ql_timer_t gh_key_timer = NULL;

static void gh_key_timer_expired_cb(u32 argv)
{
	// 定时器
	static int cnt = 0;
	cnt++;
	// gh_epaper_key_scan();
}

void gh_key_timer_setup(void)
{
	int ret = 0;
	ret = ql_rtos_timer_create(&gh_key_timer);
	if (ret) {
		gh_debug_log("Timer create failed!");
	} else {
		gh_debug_log("Timer create successful![%d]", ret);
	}

	ret = ql_rtos_timer_start(gh_key_timer, 1000, 1, gh_key_timer_expired_cb, 0);
	if (ret) {
		gh_debug_log("Timer start failed!");
	} else {
		gh_debug_log("Timer start successful![%d]", ret);
	}
}

void gh_key_timer_stop(void)
{
	ql_rtos_timer_stop(gh_key_timer);
}
```

[注]定时器回调函数`gh_key_timer_expired_cb`中不能调用任何等待或者阻塞接口，例如利用输出日志到串口的`printf()`函数。







