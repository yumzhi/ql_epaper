Development of epaper application based on EC600N-CN.

## 1.固件生成

在`SDK`根目录下运行`build.bat firmware`即可，生成的固件包位于`target`目录下。

采用一键编译时由于先执行了清除操作，而在编译时未能生成`bootloader`镜像`boot33.bin`，导致最终未能得到固件包。

未能生成`bootloader`镜像的原因待查。当前未修改`bootloader`，因此不需要单独编译`bootloader`，生成固件时将使用`ql-config\quec-project\aboot\images`目录中的`boot33.bin`镜像。

## 2.固件烧录

安装USB驱动：`Quectel_ASR_Series_UMTS&LTE_Windows_USB_Driver_Customer_V1.0.8`。

连接USB接口，打开QFlash，选择固件，点击`start`。

[注意]在下载时最好断开串口连接，先启动下载，然后快速连接USB下载线。

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



