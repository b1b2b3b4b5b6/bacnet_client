/*
 * @Author: lumos
 * @Date: 2018-12-03 01:33:02
 * @LastEditors: lumos
 * @LastEditTime: 2018-12-04 14:24:25
 * @Description: file content
 * @email: a1439458305@163.com
 */
#include "bac_device.h"

#define STD_LOCAL_LOG_LEVEL STD_LOG_DEBUG

static bac_device_t *root_device = NULL;


int bac_object_add(bac_device_t *device, int type, uint32_t id)
{
	if(device->root_object == NULL)
	{
		device->root_object = calloc(1, sizeof(bac_object_t));
		device->root_object->type_id.type = type;
		device->root_object->type_id.instance = id;
		device->root_object->device = device;
		STD_LOGI("add root object type: %d, id: %d", type, id);
		return 0;		
	}
	bac_object_t *object;
	object = device->root_object;

	for(;;)
	{
		if(object->type_id.instance == id && object->type_id.type == type)
			return -1;

		if(object->next == NULL)
		{
			object->next = calloc(1, sizeof(bac_object_t));
			object->next->type_id.type = type;
			object->next->type_id.instance = id;
			object->next->device = device;
			STD_LOGD("add object type: %d, id: %d", type, id);
			return 0;
		}
		object = object->next;
	}	
}

bac_object_t *get_object_by_type_id(bac_device_t *device, uint16_t type, uint32_t id)
{
	bac_object_t *temp;
	temp = device->root_object;	
	for(;;)
	{
		if(temp == NULL)	
		return NULL;

		if(temp->type_id.type == type && temp->type_id.instance == id)
			return temp;
		 temp = temp->next;
	}	
}

int object_traverse(bac_device_t *device, int (cb(bac_object_t *)))
{
	if(device->root_object == NULL)
		return 0;
	bac_object_t *object;
	object = device->root_object;

	for(;;)
	{
		STD_ERROR_GOTO_FAIL(cb(object));
		object = object->next;
		if(object == NULL)
			return 0;
	}

FAIL:
	STD_LOGE("object cb fail, type: %s, id : %d", bactext_object_type_name(object->type_id.type), object->type_id.instance);
	return -1;
}


bac_device_t *get_device_by_id(int device_id)
{
	bac_device_t *temp;
	temp = root_device;
	for(;;)
	{
		if(temp == NULL)	
			return NULL;
		if(temp->device_id == device_id)
			return root_device;
		 temp = temp->next;
	}
}

bac_device_t *get_device_by_invoke_id(int invoke_id)
{
	bac_device_t *temp;
	temp = root_device;	
	for(;;)
	{
		if(temp == NULL)	
		return NULL;

		if(temp->invoke_id == invoke_id)
			return temp;
		 temp = temp->next;
	}
}

bac_device_t *get_device_by_address(BACNET_ADDRESS * src)
{
	bac_device_t *temp;
	temp = root_device;	
	for(;;)
	{
		if(temp == NULL)	
		return NULL;

		if(address_match(&temp->address, src))
			return temp;
		 temp = temp->next;
	}
}

bac_device_t *get_device_by_id_src(int invoke_id, BACNET_ADDRESS * src)
{
	bac_device_t *temp;
	temp = root_device;	
	for(;;)
	{
		if(temp == NULL)	
		return NULL;

		if(address_match(&temp->address, src) && temp->invoke_id == invoke_id)
			return temp;
		 temp = temp->next;
	}	
}

int bac_device_add(uint32_t device_id)
{
	bac_device_t *temp;
	if(root_device == NULL)
	{
		root_device = calloc(1, sizeof(bac_device_t));
		root_device->type = OBJECT_DEVICE;
		root_device->device_id = device_id;
		STD_LOGI("add root device id: %d", device_id);
		return 0;
	}

	temp = root_device;
	for(;;)
	{
		if(device_id == temp->device_id)
			return -1;

		if(temp->next == NULL)
		{
			temp->next = calloc(1, sizeof(bac_device_t));
			temp->next->type = OBJECT_DEVICE;
			temp->next->device_id = device_id;
			STD_LOGD("add device id: %d", device_id);
			return 0;
		}
		temp = temp->next;
	}
}

bac_device_t *assert_device_id(uint32_t device_id)
{
	bac_device_t *device;
	device = get_device_by_id(device_id);
	if(device == NULL)
	{
		STD_LOGE("datebase can not find device : %d ", device_id);
		return NULL;
	}

    if (address_get_by_device(device->device_id, &device->max_apdu, &device->address))
		return device;
	else
	{
		STD_LOGE("stack can not find device: id %d", device->device_id);
		return NULL;
	}

}

int check_device_address(bac_device_t *device)
{
    if (address_get_by_device(device->device_id, &device->max_apdu, &device->address))
	{
		device->error_type = NO_ERROR;
		return 0;
	}
		
	else
	{
		STD_LOGE("device address check fail, id %d, address "MACSTR, device->device_id, MAC2STR(device->address.mac));
		device->error_type = BAC_LOSE;
		return -1;		
	}
}

int device_traverse(int (cb(bac_device_t *)))
{
	if(root_device == NULL)
		return 0;
	bac_device_t *device;
	device = root_device;

	for(;;)
	{
		STD_ERROR_GOTO_FAIL(cb(device));
		device = device->next;
		if(device == NULL)
			return 0;
	}

FAIL:
	STD_LOGE("devcie cb fail, id : %d", device->device_id);
	return -1;
}

void bac_device_init()
{
}

void bac_print_error(bac_device_t *device)
{
	switch(device->error_type)
	{
		case NO_ERROR:
			STD_LOGE("NO ERROR");
			break;
		case BAC_TIME_OUT:
			STD_LOGE("BAC_TIME_OUT");
			break;
		case BAC_ERROR:
			STD_LOGE("BAC_ERROR");
			break;
		case BAC_ABORT:
			STD_LOGE("BAC_ABORT");
			break;
		case BAC_REGECT:
			STD_LOGE("BAC_REGECT");
			break;
		case BAC_DECODE:
			STD_LOGE("BAC_DECODE");
			break;
		case BAC_ACK_TYPE:
			STD_LOGE("BAC_ACK_TYPE");
			break;
		case BAC_ANALYZE:
			STD_LOGE("BAC_ANALYZE");
			break;
		default:
			STD_LOGE("undefined error");
			exit(-1);
			break;
	}
}



int bac_print_object(bac_object_t *object)
{
	//printf("<<object>>\r\n");
	printf("type: %15s | id: %7d | ", bactext_object_type_name(object->type_id.type), object->type_id.instance);
	
	switch(object->type_id.type)
	{
		case OBJECT_ANALOG_VALUE:
			printf("value: %f | ", object->present_value.float_value);
			break;
		case OBJECT_ANALOG_INPUT:
			printf("value: %f | ", object->present_value.float_value);
			break;
		case OBJECT_ANALOG_OUTPUT:
			printf("value: %f | ", object->present_value.float_value);
			break;

		case OBJECT_BINARY_INPUT:
			printf("value: %d | ", object->present_value.bool_value);
			break;
		case OBJECT_BINARY_OUTPUT:
			printf("value: %d | ", object->present_value.bool_value);
			break;
		case OBJECT_BINARY_VALUE:
			printf("value: %d | ", object->present_value.bool_value);
			break;
		default:
			STD_LOGE("unsupport object type: %s", bactext_object_type_name(object->type_id.type));
			exit(-1);
			break;
	}
	printf("\r\n");
	//printf(">>object<<\r\n");
	return 0;
}

void bac_print_device(bac_device_t *device)
{
	object_traverse(device, bac_print_object);
}

