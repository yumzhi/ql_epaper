Development of epaper application based on EC600N-CN.

1.固件生成

在`SDK`根目录下运行`build.bat firmware`即可，生成的固件包位于`target`目录下。

采用一键编译时由于先执行了清除操作，而在编译时未能生成`bootloader`镜像`boot33.bin`，导致最终未能得到固件包。

未能生成`bootloader`镜像的原因待查。当前未修改`bootloader`，因此不需要单独编译`bootloader`，生成固件时将使用`ql-config\quec-project\aboot\images`目录中的`boot33.bin`镜像。

2.固件烧录

安装USB驱动：`Quectel_ASR_Series_UMTS&LTE_Windows_USB_Driver_Customer_V1.0.8`。

连接USB接口，打开QFlash，选择固件，点击`start`。



