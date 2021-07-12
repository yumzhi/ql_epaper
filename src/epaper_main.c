#include <stdio.h>
#include "ql_debug.h"
#include "ql_rtos.h"
#include "ql_spi.h"
#include "ql_application.h"

ql_task_t ql_epaper_task_ref;

void ql_epaper_task(void *arg)
{
	int i = 0;
	
	while(i<9)
	{
		ql_rtos_task_sleep_s(5);
		printf("I am in sub task...\r\n");
		i++;
	}
}

static void epaper_main(void * argv)
{
	int ret = -1;
	
	unsigned char buf_in[256] = {0};
	unsigned char buf_out[128] = "=========This is the spi test !!===========\r\n";

	QlOSStatus status = ql_rtos_task_create(&ql_epaper_task_ref, 4096, 100, "ql_epaper_task", ql_epaper_task, NULL);
	qL_debug_log_mode_e mode = ql_get_debug_log_mode();
	printf("qL_debug_log_mode_e=%d\r\n",mode);
	
	ql_debug_log_enable();
	printf("Hello, world!\r\n");

	ql_debug_log_disable();
	printf("Hello, world!!\r\n");
	
	ql_debug_log_enable();
	printf("Hello, world!!!\r\n");
	
	ql_spi_init(QL_SPI_PORT0, QL_SPI_MODE0,QL_SPI_CLK_1_625MHZ);
	
	while(1)
	{
		// task主函数驻留
		printf("ql_spi_buf_out:  %s \r\n ",buf_out);	
		ret= ql_spi_write_read(QL_SPI_PORT0, buf_in, buf_out, sizeof(buf_out));
		if(ret != 0)
		{
			printf("ql_spi_read error, ret = %d \r\n",ret);
			break;
		}
		
		printf("ql_spi_buf_in:  %s \r\n",buf_in);
		ql_rtos_task_sleep_s(3);
		printf("I still alive...\r\n");
	}
}

application_init(epaper_main, "epaper_main", 4, 0);
