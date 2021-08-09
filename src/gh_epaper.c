#include "gh_epaper.h"
#include "ql_rtos.h"
#include "image.h"

unsigned char vcom_value;
unsigned char pll_value;
unsigned char temprature_value[2];

typedef union
{
	unsigned long T_ulong;
	long  T_long;
	struct
	{
		struct
		{
			unsigned char T_Lbyte;
			unsigned char T_Hbyte;
		} T_Lword;
		struct
		{
			unsigned char T_Lbyte;
			unsigned char T_Hbyte;
		} T_Hword;
	} T_DoubleWords;
} T_nt32BM;

void gh_epaper_io_init(void)
{
	// Buttons
	ql_gpio_init(EPAPER_BUTTON1, PIN_DIRECTION_IN, PIN_PULL_DISABLE, PIN_LEVEL_LOW);
	ql_gpio_init(EPAPER_BUTTON2, PIN_DIRECTION_IN, PIN_PULL_DISABLE, PIN_LEVEL_LOW);
	ql_gpio_init(EPAPER_BUTTON3, PIN_DIRECTION_IN, PIN_PULL_DISABLE, PIN_LEVEL_LOW);
	ql_gpio_init(EPAPER_BUTTON4, PIN_DIRECTION_IN, PIN_PULL_DISABLE, PIN_LEVEL_LOW);

	// Epaper link led
	ql_gpio_init(EPAPER_LINK_LED, PIN_DIRECTION_OUT, PIN_PULL_DISABLE, PIN_LEVEL_LOW);
	ql_gpio_set_level(EPAPER_LINK_LED, 1);
	// BUSY
	ql_gpio_init(EPAPER_BUSY1, PIN_DIRECTION_OUT, PIN_PULL_DISABLE, PIN_LEVEL_LOW);
	ql_gpio_init(EPAPER_BUSY2, PIN_DIRECTION_OUT, PIN_PULL_DISABLE, PIN_LEVEL_LOW);

	// CS
	ql_gpio_init(EPAPER_SPI_CS2, PIN_DIRECTION_OUT, PIN_PULL_DISABLE, PIN_LEVEL_LOW);
	// D/C
	ql_gpio_init(EPAPER_SPI_DC, PIN_DIRECTION_OUT, PIN_PULL_DISABLE, PIN_LEVEL_LOW);

	// RST
	ql_gpio_init(EPAPER_RST, PIN_DIRECTION_OUT, PIN_PULL_DISABLE, PIN_LEVEL_LOW);
}

void gh_epaper_io_deinit(void)
{
	ql_gpio_deinit(EPAPER_BUTTON1);
	ql_gpio_deinit(EPAPER_BUTTON2);
	ql_gpio_deinit(EPAPER_BUTTON3);
	ql_gpio_deinit(EPAPER_BUTTON4);
	ql_gpio_deinit(EPAPER_LINK_LED);
	ql_gpio_deinit(EPAPER_BUSY1);
	ql_gpio_deinit(EPAPER_BUSY1);
	ql_gpio_deinit(EPAPER_RST);
}

void gh_epaper_driver_init(void)
{
	int ret = -2;
	gh_epaper_io_init();
	ret = ql_spi_init(EPAPER_SPI_PORT, EPAPER_SPI_MODE, EPAPER_SPI_FREQ);
	if (-1 == ret) {
		gh_debug_log("SPI initialize failed!");
		return;
	} else if (0 == ret) {
		gh_debug_log("SPI initialize successful!");
	}
}

void gh_epaper_set_level(GPIO_PIN_NUMBER_E gpio_num, PIN_LEVEL_E level)
{
	ql_gpio_set_level(gpio_num, level);
}

void gh_epaper_spi_write_byte(unsigned char data)
{
	ql_spi_write(EPAPER_SPI_PORT, &data, 1);
}

void gh_epaper_write_data(unsigned char data)
{
	gh_epaper_set_level(EPAPER_SPI_DC, PIN_LEVEL_HIGH);
	gh_epaper_spi_write_byte(data);
}

void gh_epaper_write_command(unsigned char command)
{
	gh_epaper_set_level(EPAPER_SPI_DC, PIN_LEVEL_LOW);
	gh_epaper_spi_write_byte(command);
}

void gh_epaper_write_data2(unsigned char data)
{
	gh_epaper_set_level(EPAPER_SPI_CS1, PIN_LEVEL_HIGH);
	gh_epaper_set_level(EPAPER_SPI_CS1, PIN_LEVEL_LOW);
	gh_epaper_set_level(EPAPER_SPI_DC, PIN_LEVEL_HIGH);
	ql_rtos_task_sleep_ms(5);
	gh_epaper_spi_write_byte(data);
	ql_rtos_task_sleep_ms(5);
	gh_epaper_set_level(EPAPER_SPI_CS1, PIN_LEVEL_HIGH);
}

void gh_epaper_write_command2(unsigned char command)
{
	gh_epaper_set_level(EPAPER_SPI_CS1, PIN_LEVEL_HIGH);
	gh_epaper_set_level(EPAPER_SPI_CS1, PIN_LEVEL_LOW);
	gh_epaper_set_level(EPAPER_SPI_DC, PIN_LEVEL_LOW);
	ql_rtos_task_sleep_ms(5);
	gh_epaper_spi_write_byte(command);
	ql_rtos_task_sleep_ms(5);
	gh_epaper_set_level(EPAPER_SPI_CS1, PIN_LEVEL_HIGH);
}

void gh_epaper_write_command3(uint8_t command)
{
	gh_epaper_set_level(EPAPER_SPI_CS1, PIN_LEVEL_HIGH);
	gh_epaper_set_level(EPAPER_SPI_CS1, PIN_LEVEL_LOW);
	gh_epaper_set_level(EPAPER_SPI_DC, PIN_LEVEL_LOW);
	ql_rtos_task_sleep_ms(5);
	gh_epaper_spi_write_byte(command);
	ql_rtos_task_sleep_ms(5);
	// gh_epaper_set_level(EPAPER_SPI_CS1, PIN_LEVEL_HIGH);
}

/*
 * 等待屏幕空闲
 * gpio_num: EPAPER_BUSY1 or EPAPER_BUSY2
 */
void gh_epaper_read_busy(GPIO_PIN_NUMBER_E gpio_num)
{
	int gpio_level = 0;
	while (gpio_level == 0)
	{
		ql_gpio_get_level(gpio_num, (PIN_LEVEL_E *)&gpio_level);
		gh_debug_log("The panel is busy.");
		ql_rtos_task_sleep_ms(200);
	}
}

void gh_epaper_flash_sleep(void)
{
	ql_rtos_task_sleep_ms(5);
	gh_epaper_spi_write_byte(0xb9);
	ql_rtos_task_sleep_ms(5);
}

void gh_epaper_flash_wake(void)
{
	ql_rtos_task_sleep_ms(5);
	gh_epaper_spi_write_byte(0xab);
	ql_rtos_task_sleep_ms(5);
}

unsigned char gh_spi_read_byte(void)
{
	unsigned char data = 0;
	ql_spi_read(EPAPER_SPI_PORT, &data, 1);
	return data;
}

void gh_epaper_get_temperature(void)
{
	gh_epaper_write_command2(0x41);
	gh_epaper_write_data2(0x00);
	gh_epaper_read_busy(EPAPER_BUSY1);
	gh_epaper_write_command3(0x40);

	temprature_value[0] = gh_spi_read_byte();
	temprature_value[1] = gh_spi_read_byte();

	gh_debug_log("TEMP1:%d\n", temprature_value[0]);
	gh_debug_log("TEMP2:%d\n", temprature_value[1]);
}

void gh_epaper_read_bytes_sflash(T_nt32BM dst, unsigned char * userbyte, unsigned int num)
{
	//	SFLASH_BEGIN();
	ql_rtos_task_sleep_ms(10);
	gh_epaper_spi_write_byte(0x03);
	gh_epaper_spi_write_byte(dst.T_DoubleWords.T_Hword.T_Lbyte);
	gh_epaper_spi_write_byte(dst.T_DoubleWords.T_Lword.T_Hbyte);
	gh_epaper_spi_write_byte(dst.T_DoubleWords.T_Lword.T_Lbyte);

	while (num--) { // stay in loop until length = 0
		*userbyte++ = gh_spi_read_byte();
	}

	//	SFLASH_STOP();
	ql_rtos_task_sleep_ms(5);
}

void gh_epaper_get_flash_data(void)
{
	unsigned char value_tmp = 0;
	T_nt32BM temp_dest_addr;
	unsigned char temp_vcom[1];
	unsigned char temp_pll[1];

	gh_epaper_get_temperature();

	gh_debug_log("T0 T1 is second: %x, %x \r\n", temprature_value[0], temprature_value[1]);
	//19 80 //
	value_tmp = (temprature_value[0] << 1) + (temprature_value[1] >> 7); //1980 // 0X33
	gh_debug_log("value_tmp: %x \r\n", value_tmp);
	value_tmp = 0x35;//TEMP_25;

	if ((value_tmp & 0x80) || (value_tmp < TEMP_3)) {
		gh_debug_log("Get flash data");
		gh_epaper_write_command(0x65);
		gh_epaper_write_data(0x01);
		gh_epaper_flash_wake();
		temp_dest_addr.T_ulong = 0x61DA; ////25050;
		gh_epaper_read_bytes_sflash(temp_dest_addr, temp_pll, 1);
		pll_value = temp_pll[0];
		//gh_debug_log("Set pll_value data_second %x\r\n", pll_value);	//t0 17 t1 80

		temp_dest_addr.T_ulong = 0x61E4; ////25060;
		gh_epaper_read_bytes_sflash(temp_dest_addr, temp_vcom, 1);
		vcom_value = temp_vcom[0];
		gh_epaper_write_command(0x65);
		gh_epaper_write_data(0x00);
	} else if ((TEMP_3 <= value_tmp) && (value_tmp < TEMP_6)) {
		gh_debug_log("Get flash data(TEMP_3<=value_tmp&value_tmp<TEMP_6)");
		gh_epaper_write_command(0x65);
		gh_epaper_write_data(0x01);
		gh_epaper_flash_wake();
		temp_dest_addr.T_ulong = 0x61DB; ///25051;
		gh_epaper_read_bytes_sflash(temp_dest_addr, temp_pll, 1);
		pll_value = temp_pll[0];
		//gh_debug_log("Set pll_value data_second %x\r\n", pll_value);	//t0 17 t1 80

		temp_dest_addr.T_ulong = 0x61E5; ///25061;
		gh_epaper_read_bytes_sflash(temp_dest_addr, temp_vcom, 1);
		vcom_value = temp_vcom[0];
		gh_epaper_write_command(0x65);
		gh_epaper_write_data(0x00);
	} else if ((TEMP_6 <= value_tmp) && (value_tmp < TEMP_10)) {
		gh_debug_log("Get flash data(TEMP_6<=value_tmp&value_tmp<TEMP_10)");
		gh_epaper_write_command(0x65);
		gh_epaper_write_data(0x01);
		gh_epaper_flash_wake();
		temp_dest_addr.T_ulong = 0x61DC; ///25052;
		gh_epaper_read_bytes_sflash(temp_dest_addr, temp_pll, 1);
		pll_value = temp_pll[0];
		//gh_debug_log("Set pll_value data_second %x\r\n", pll_value);	//t0 17 t1 80

		temp_dest_addr.T_ulong = 0x61E6; ///25062;
		gh_epaper_read_bytes_sflash(temp_dest_addr, temp_vcom, 1);
		vcom_value = temp_vcom[0];
		gh_epaper_write_command(0x65);
		gh_epaper_write_data(0x00);
	} else if ((TEMP_10 <= value_tmp) && (value_tmp < TEMP_15)) {
		gh_debug_log("Get flash data(TEMP_10<=value_tmp&value_tmp<TEMP_15)");
		gh_epaper_write_command(0x65);
		gh_epaper_write_data(0x01);
		gh_epaper_flash_wake();
		temp_dest_addr.T_ulong = 0x61DD; ///25053;
		gh_epaper_read_bytes_sflash(temp_dest_addr, temp_pll, 1);
		pll_value = temp_pll[0];
		//gh_debug_log("Set pll_value data_second %x\r\n", pll_value);	//t0 17 t1 80

		temp_dest_addr.T_ulong = 0x61E7; ///25063;
		gh_epaper_read_bytes_sflash(temp_dest_addr, temp_vcom, 1);
		vcom_value = temp_vcom[0];
		gh_epaper_write_command(0x65);
		gh_epaper_write_data(0x00);
	} else if ((TEMP_15 <= value_tmp) && (value_tmp < TEMP_20)) {
		gh_debug_log("Get flash data(TEMP_15<=value_tmp&value_tmp<TEMP_20)");
		gh_epaper_write_command(0x65);
		gh_epaper_write_data(0x01);
		gh_epaper_flash_wake();
		temp_dest_addr.T_ulong = 0x61DE; ///25054;
		gh_epaper_read_bytes_sflash(temp_dest_addr, temp_pll, 1);
		pll_value = temp_pll[0];
		//gh_debug_log("Set pll_value data_second %x\r\n", pll_value);	//t0 17 t1 80

		temp_dest_addr.T_ulong = 0x61E8; ///25064;
		gh_epaper_read_bytes_sflash(temp_dest_addr, temp_vcom, 1);
		vcom_value = temp_vcom[0];
		gh_epaper_write_command(0x65);
		gh_epaper_write_data(0x00);
	} else if ((TEMP_20 <= value_tmp) && (value_tmp < TEMP_25)) {
		gh_debug_log("Get flash data(TEMP_20<=value_tmp&value_tmp<TEMP_25)");
		gh_epaper_write_command(0x65);
		gh_epaper_write_data(0x01);
		gh_epaper_flash_wake();
		temp_dest_addr.T_ulong = 0x61DF; ///25055;
		gh_epaper_read_bytes_sflash(temp_dest_addr, temp_pll, 1);
		pll_value = temp_pll[0];
		//gh_debug_log("Set pll_value data_second %x\r\n", pll_value);	//t0 17 t1 80

		temp_dest_addr.T_ulong = 0x61E9; ///25065;
		gh_epaper_read_bytes_sflash(temp_dest_addr, temp_vcom, 1);
		vcom_value = temp_vcom[0];
		gh_epaper_write_command(0x65);
		gh_epaper_write_data(0x00);
	} else if (value_tmp >= TEMP_25) { //
		gh_debug_log("Get flash data(value_tmp>=TEMP_25)");
		gh_epaper_write_command(0x65);
		gh_epaper_write_data(0x01);
		gh_epaper_flash_wake();
		temp_dest_addr.T_ulong = 0x61E0; ///25056;
		gh_epaper_read_bytes_sflash(temp_dest_addr, temp_pll, 1);
		pll_value = temp_pll[0];
		//pll_value=0x3A;
		//gh_debug_log("Set pll_value data_second %x\r\n", pll_value);	//t0 17 t1 80

		temp_dest_addr.T_ulong = 0x61EA; ///25066;
		gh_epaper_read_bytes_sflash(temp_dest_addr, temp_vcom, 1);
		vcom_value = temp_vcom[0];
		vcom_value = 0x24;

		gh_epaper_write_command(0x65);
		gh_epaper_write_data(0x00);
	}

}

void gh_epaper_panel_init(void)
{
	gh_epaper_set_level(EPAPER_RST, PIN_LEVEL_LOW);
	ql_rtos_task_sleep_ms(20);
	gh_epaper_set_level(EPAPER_RST, PIN_LEVEL_HIGH);
	ql_rtos_task_sleep_ms(100);

	gh_epaper_write_command(0x06);
	gh_epaper_write_data(0xd7);
	gh_epaper_write_data(0xd7);
	gh_epaper_write_data(0x2f); // 是否可以一次性写入？有必要一次性写入吗？

	ql_rtos_task_sleep_ms(5);
	gh_epaper_write_command(0x01);
	gh_epaper_write_data(0x37);
	gh_epaper_write_data(0x00);

	gh_epaper_write_command(0x04); //power on
	ql_rtos_task_sleep_ms(10);  //10MS
	gh_epaper_read_busy(EPAPER_BUSY1);

	gh_epaper_write_command(0x65);
	gh_epaper_write_data(0x01);
	gh_epaper_flash_wake();

	gh_epaper_write_command(0x65);
	gh_epaper_write_data(0x00);

	gh_epaper_write_command(0x00);
	gh_epaper_write_data(0x0f);	  //  640*480
	gh_epaper_write_data(0x00); //

	gh_epaper_write_command(0X50);  //border
	gh_epaper_write_data(0x77);  //W

	gh_epaper_write_command(0x61);	// resolution setting
	gh_epaper_write_data(0x02);
	gh_epaper_write_data(0x80);	  //640gate
	gh_epaper_write_data(0x01);
	gh_epaper_write_data(0x80);  //384source

	gh_epaper_get_flash_data();
	gh_epaper_write_command(0x30);
	gh_epaper_write_data(pll_value);
	gh_epaper_write_command(0x82);
	gh_epaper_write_data(vcom_value);

	gh_epaper_read_busy(EPAPER_BUSY1);
	gh_epaper_write_command(0xe5);
	gh_epaper_write_data(0x03);
	gh_epaper_read_busy(EPAPER_BUSY1);
}


void HalLcd_UploadGraghSector(unsigned char * datasMono, unsigned char * datasRed, unsigned int num)
{
	unsigned int i;
	unsigned char j, k;
	unsigned char tempvalue;
	//unsigned char tempOriginal;
	unsigned char tempMono, tmpRed;

	gh_epaper_read_busy(EPAPER_BUSY1);
	gh_epaper_write_command(0x10);


	for (i = 0; i < num; i++) {
		if (datasMono == NULL)
			tempMono = 0;
		else
			tempMono = * datasMono++;

		if (datasRed == NULL)
			tmpRed = 0;
		else
			tmpRed = * datasRed++;

		for (k = 0; k < 4; k++) {
			tempvalue = 0;

			for (j = 0; j < 2; j++) {
				tempvalue <<= 4;

				if (tempMono & 0x80)
					tempvalue &= 0xf0; //black
				else {
					if (tmpRed & 0x80)
						tempvalue |= 0x04; //red
					else
						tempvalue |= 0x03; //white
				}

				tempMono <<= 1;
				tmpRed <<= 1;
			}

			gh_epaper_write_data(tempvalue);
		}
	}

	while (i < ALLSCREEN_GRAGHBYTES) {
		gh_epaper_write_data(0x33);
		gh_epaper_write_data(0x33);
		gh_epaper_write_data(0x33);
		gh_epaper_write_data(0x33);
		i += 1;
	}
}


void DEP075_Flash_UpdateAndClose(void)
{
	gh_epaper_write_command(0x12);        //display
	gh_epaper_read_busy(EPAPER_BUSY1);

	gh_epaper_write_command(0x65);
	gh_epaper_write_data(0x01);
	gh_epaper_flash_sleep();
	gh_epaper_write_command(0x65);
	gh_epaper_write_data(0x00);
	gh_epaper_read_busy(EPAPER_BUSY1);
	gh_epaper_write_command(0x02);        //power off
	ql_rtos_task_sleep_ms(60);
	gh_epaper_read_busy(EPAPER_BUSY1);
	gh_epaper_write_command(0x07); //deep sleep
	gh_epaper_write_data(0xa5);
	ql_rtos_task_sleep_ms(500);
}


void gh_epaper_power_on(void)
{
	gh_epaper_driver_init(); // GPIO & SPI Initialization
	// gh_key_timer_setup();    // Setup the timer for key detection
	// gh_epaper_panel_init();  // EPD Panel Initialization
	gh_debug_log("Epaper one start updating\r\n");
	// HalLcd_UploadGraghSector((unsigned char *)image_content, (unsigned char *)image_content + 30720, ALLSCREEN_GRAGHBYTES);
	// DEP075_Flash_UpdateAndClose();
}

// Detection of the keys/buttons
void gh_epaper_key_scan(void)
{
	static int cnt = 0;
	cnt++;
	int gpio_level = 0;
	ql_gpio_get_level(EPAPER_BUTTON1, (PIN_LEVEL_E *)&gpio_level);
	if (gpio_level == 0) {
		ql_gpio_set_level(EPAPER_LINK_LED, cnt%2);
	}
}

// Setup a timer for key detection
static ql_timer_t gh_key_timer = NULL;

static void gh_key_timer_expired_cb(u32 argv)
{
	// 定时器回调函数中不能调用任何等待或者阻塞接口，如printf
	gh_epaper_key_scan();
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