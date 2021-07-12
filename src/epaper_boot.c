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

