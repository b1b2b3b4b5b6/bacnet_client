/*
 * @Author: lumos
 * @Date: 2018-12-03 17:40:35
 * @LastEditors: lumos
 * @LastEditTime: 2018-12-04 12:05:10
 * @Description: file content
 * @Email: a1439458305@163.com
 */
#include "handle_device.h"

#define STD_LOCAL_LOG_LEVEL STD_LOG_DEBUG

int dread_object_count(bac_device_t *device)
{
    STD_ERROR_GOTO_FAIL(read_device_prop(device, PROP_OBJECT_LIST, 0, &device->object_count));
	device->object_count -= 1; //to except device, itself is a object ,index is 1, 
	STD_LOGD("object count: %d", device->object_count);
	return 0;

FAIL:
	return -1;
}

int dread_object_type_id(bac_device_t *device, uint32_t object_index, bac_object_t *object)
{
	/*+2, 0 is index of object count, 1 is index of device*/
    STD_ERROR_GOTO_FAIL(read_device_prop(device, PROP_OBJECT_LIST, object_index + 2, &object->type_id)); 	
	STD_LOGD("get object type: %d, id: %d", object->type_id.type, object->type_id.instance);
	return 0;

FAIL:
	return -1;
}


