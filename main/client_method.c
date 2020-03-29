/*
 * @Author: lumos
 * @Date: 2018-12-04 11:55:26
 * @LastEditors: lumos
 * @LastEditTime: 2018-12-04 14:30:28
 * @Description: file content
 * @Email: a1439458305@163.com
 */

#include "client_method.h"

#define STD_LOCAL_LOG_LEVEL STD_LOG_DEBUG

int scan_device()
{
	STD_LOGI("scan_device");
    Send_WhoIs(-1, -1);
	sleep(3);
	return 0;
}


int read_device_info(bac_device_t *device)
{
	STD_ERROR_GOTO_FAIL(dread_object_count(device));
	
	bac_object_t object;
	for(int n = 0; n < device->object_count; n++)
	{
		STD_ERROR_GOTO_FAIL(dread_object_type_id(device, n, &object));
		STD_ERROR_GOTO_FAIL(bac_object_add(device, object.type_id.type, object.type_id.instance));
	}
    object_traverse(device, traversal_present_cb);
	
	return 0;	

FAIL:
	bac_print_error(device);
	return -1;
}

#if PRODUCT_TEST
void mem_test(uint32_t device_id)
{
	bac_device_add(device_id);
	bac_device_t *device = assert_device_id(device_id);
	read_device_info(device);
	bac_print_device(device);
	object_traverse(device, add_monitor_object);
}
#endif