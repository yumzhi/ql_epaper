#include <stdio.h>
#include "ql_debug.h"
#include "ql_rtos.h"
#include "ql_spi.h"
#include "ql_data_call.h"
#include "ql_sim.h"
#include "sockets.h"
#include "ql_dev.h"
#include "ql_nw.h"
#include "ql_application.h"
#include "gh_epaper.h"

#define INVALID_SOCKET -1

#define DATA_CALL_PROFILE_IDX 2
#define IP_PROTOCOL 0 

#define INPUT_GPIO GPIO_PIN_NO_11
#define STATUS_LED  GPIO_PIN_NO_7

#define SOCKET_RECV_BUF_MAX_LEN 1024
#define UART_RECV_BUF_MAX_LEN 1024

ql_task_t gh_epaper_task_ref;

int data_call_successed_flag = 0;

typedef enum 
{
	QL_SUCCESS,
	QL_GENERIC_FAILURE,
}QL_ERROR_CODE_E;

QL_ERROR_CODE_E check_sim_state(void)
{
	int ret=0,card_status=-1,retry_num=0;
RETRY_CHECK:
	ret=ql_sim_get_card_status(&card_status);
	printf("The sim state: %d\n", card_status);
	if(ret==0)
	{
		switch(card_status)
		{
			case QL_SIM_STATUS_READY:
				return QL_SUCCESS;
			default:
				if(retry_num<60)
				{
					retry_num++;
					if((card_status==QL_SIM_STATUS_NOT_INSERTED)&&(retry_num==30))
					{
						ql_dev_set_modem_fun(0,0);
						ql_rtos_task_sleep_s(3); //ÐÝÃß3s
						ql_dev_set_modem_fun(1,0);
						ql_rtos_task_sleep_s(2); //ÐÝÃß2s
					}
					ql_rtos_task_sleep_s(1); //ÐÝÃß1s
					goto RETRY_CHECK;
				}
				break;
		}
	}
	return QL_GENERIC_FAILURE;
}

QL_ERROR_CODE_E wait_network_register(int time)
{
	int ret=0,retry_num=0;
	QL_NW_REG_STATUS_INFO_T  reg_status;
RETRY_CHECK:
	ret=ql_nw_get_reg_status(&reg_status);
	printf("The nw state: %d\n", reg_status.data_reg.state);
	if(ret==0)
	{
		switch(reg_status.data_reg.state)
		{
			case QL_NW_REG_STATE_HOME_NETWORK:
			case QL_NW_REG_STATE_ROAMING:
				return QL_SUCCESS;
			default:
				ql_rtos_task_sleep_s(1); //ÐÝÃß1s
				if(retry_num<time)
				{
					retry_num++;
					goto RETRY_CHECK;
				}
				break;
		}
	}
	return QL_GENERIC_FAILURE;
}

static void datacall_status_callback(int profile_idx, int status)
{
	struct ql_data_call_info info = {0};
	char ip_addr_str[64] = {0};
	if(status)
	{
		data_call_successed_flag=1;
		ql_get_data_call_info(profile_idx, IP_PROTOCOL, &info);
		printf("info.profile_idx: %d\n", info.profile_idx);
		printf("info.ip_version: %d\n", info.ip_version);
		
		if(info.ip_version)
		{
			printf("info.v6.state: %d\n", info.v6.state);
			printf("info.v6.reconnect: %d\n", info.v6.reconnect);

			inet_ntop(AF_INET6, &info.v6.addr.ip, ip_addr_str, sizeof(ip_addr_str));
			printf("info.v6.addr.ip: %s\n", ip_addr_str);

			inet_ntop(AF_INET6, &info.v6.addr.pri_dns, ip_addr_str, sizeof(ip_addr_str));
			printf("info.v6.addr.pri_dns: %s\n", ip_addr_str);

			inet_ntop(AF_INET6, &info.v6.addr.sec_dns, ip_addr_str, sizeof(ip_addr_str));
			printf("info.v6.addr.sec_dns: %s\n", ip_addr_str);
		}
		else
		{
			printf("info.v4.state: %d\n", info.v4.state);
			printf("info.v4.reconnect: %d\n", info.v4.reconnect);

			inet_ntop(AF_INET, &info.v4.addr.ip, ip_addr_str, sizeof(ip_addr_str));
			printf("info.v4.addr.ip: %s\n", ip_addr_str);

			inet_ntop(AF_INET, &info.v4.addr.pri_dns, ip_addr_str, sizeof(ip_addr_str));
			printf("info.v4.addr.pri_dns: %s\n", ip_addr_str);

			inet_ntop(AF_INET, &info.v4.addr.sec_dns, ip_addr_str, sizeof(ip_addr_str));
			printf("info.v4.addr.sec_dns: %s\n", ip_addr_str);
		}
		
	}
	else
	{
		data_call_successed_flag=0;
		printf("Data call failed! profile_idx:%d\n", info.profile_idx);
	}
}

QL_ERROR_CODE_E start_data_call(void)
{
	int ret = 0;
	data_call_successed_flag = 0;
	//printf("start_data_call:%d\n", __LINE__);
	ret = ql_wan_start(datacall_status_callback);
	if (ret != 0)
		return QL_GENERIC_FAILURE;
	ql_set_auto_connect(DATA_CALL_PROFILE_IDX, TRUE);
	//printf("start_data_call:%d\n", __LINE__);
	if (ret != 0)
		return QL_GENERIC_FAILURE;
	ql_start_data_call(DATA_CALL_PROFILE_IDX, IP_PROTOCOL, "cmnet", NULL, NULL, 0);
	//printf("start_data_call:%d\n", __LINE__);
	if (ret != 0)
		return QL_GENERIC_FAILURE;
	return QL_SUCCESS;
}

void gh_network_check(void)
{
	int ret = 0;

	//检查SIM卡状态
	ret = check_sim_state();
	if (ret != 0) {
		printf("*** The sim is not available ! ***\n");
		return;
	}

	//等待模块注上移动网络 60s
	ret = wait_network_register(60);
	if (ret > 0) {
		printf("*** Network register failed ! ***\n");
		return;
	}

	//启动模块数据拨号
	ret = start_data_call();
	if (ret > 0) {
		printf("*** Start data failed ! ***\n");
		return;
	}

	//等待模块拨号成功
	int wait_count = 10;
	while ((data_call_successed_flag == 0) && (wait_count >= 0)) {
		ql_rtos_task_sleep_s(1); //休眠1s
		wait_count--;
	}
}

void gh_epaper_task(void *arg)
{
	int i = 0;

	while (i < 9)
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

	gh_network_check();

	QlOSStatus status = ql_rtos_task_create(&gh_epaper_task_ref, 4096, 100, "gh_epaper_task", gh_epaper_task, NULL);
	printf("status=%d\r\n", status);

	qL_debug_log_mode_e mode = ql_get_debug_log_mode();
	printf("qL_debug_log_mode_e=%d\r\n", mode);

	ql_debug_log_enable();
	printf("Hello, world!\r\n");

	ql_debug_log_disable();
	printf("Hello, world!!\r\n");

	ql_debug_log_enable();
	printf("Hello, world!!!\r\n");

	// ql_spi_init(QL_SPI_PORT0, QL_SPI_MODE0, QL_SPI_CLK_1_625MHZ);

	gh_epaper_power_on();
	gh_key_timer_setup(); // Setup the timer for key detection

	while (1) {
		// task主函数驻留
		#if 0
		printf("ql_spi_buf_out:  %s \r\n ", buf_out);
		ret = ql_spi_write_read(QL_SPI_PORT0, buf_in, buf_out, sizeof(buf_out));
		if (ret != 0) {
			printf("ql_spi_read error, ret = %d \r\n", ret);
			break;
		}

		printf("ql_spi_buf_in:  %s \r\n", buf_in);
		ql_rtos_task_sleep_s(3);
		printf("I still alive...\r\n");
		#endif
	}
}

application_init(epaper_main, "epaper_main", 4, 0);
