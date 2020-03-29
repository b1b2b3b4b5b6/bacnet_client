/*
 * @Author: lumos
 * @Date: 2018-12-03 17:36:19
 * @LastEditors: lumos
 * @LastEditTime: 2018-12-04 14:29:08
 * @Description: file content
 * @Email: a1439458305@163.com
 */


#include "bac_read.h"

#define STD_LOCAL_LOG_LEVEL STD_LOG_DEBUG



bac_msg_t *read_prop_void(void *target, int object_prop, uint32_t array_index)
{
	uint32_t object_instance;
	uint16_t *p_type = target;
	bac_device_t *device;
	bac_object_t *object;

	if(*p_type == OBJECT_DEVICE) 
	{
		device = target;
		object_instance = device->device_id; 
	}
	else
	{
		object = target;
		device = object->device;
		object_instance = object->type_id.instance;
	}
	  
    STD_ERROR_GOTO_FAIL(check_device_address(device));
	device->invoke_id = Send_Read_Property_Request_Address(&device->address, device->max_apdu, *p_type, object_instance, object_prop, array_index);
	return wait_device_ack(device, READ_ACK);
	
FAIL:
	return NULL;
}


static int analyze_value(bac_msg_t  *bac_msg, void *ptr, int ptr_len)
{
	int len = -1;
	BACNET_APPLICATION_DATA_VALUE value;
	BACNET_READ_PROPERTY_DATA *data = (BACNET_READ_PROPERTY_DATA *)bac_msg->ptr;
	len = bacapp_decode_application_data(data->application_data, data->application_data_len, &value);

	if(len <=0 )
	{
		STD_LOGE("decode value fail");
		goto FAIL;
	}

	switch (value.tag) {
		case BACNET_APPLICATION_TAG_NULL:
			STD_LOGE("do not support BACNET_APPLICATION_TAG_NULL");
			exit(-1);
			break;
		case BACNET_APPLICATION_TAG_BOOLEAN:
			*((bool *)ptr) = value.type.Boolean;
			break;
		case BACNET_APPLICATION_TAG_UNSIGNED_INT:
			*((int32_t *)ptr) = value.type.Unsigned_Int;
			break;
		case BACNET_APPLICATION_TAG_SIGNED_INT:
			*((int32_t *)ptr) = value.type.Signed_Int;
			break;
		case BACNET_APPLICATION_TAG_REAL:
			*((float *)ptr) = value.type.Real;
			break;

	#if defined (BACAPP_DOUBLE)
		case BACNET_APPLICATION_TAG_DOUBLE:
			*((float *)ptr) = (float)value.type.Double;
			break;
	#endif

		case BACNET_APPLICATION_TAG_OCTET_STRING:
			STD_LOGE("do not support BACNET_APPLICATION_TAG_OCTET_STRING");
			exit(-1);		
			break;
		case BACNET_APPLICATION_TAG_CHARACTER_STRING:
			STD_LOGE("do not support BACNET_APPLICATION_TAG_CHARACTER_STRING");
			exit(-1);
			break;
		case BACNET_APPLICATION_TAG_BIT_STRING:
			STD_LOGE("do not support BACNET_APPLICATION_TAG_BIT_STRING");
			exit(-1);
			break;
		case BACNET_APPLICATION_TAG_ENUMERATED:
			switch(ptr_len)
			{
				case 1:
					(*(uint8_t *)ptr) = value.type.Enumerated;
					break;
				case 2:
					(*(uint16_t *)ptr) = value.type.Enumerated;
					break;
				case 4:
					(*(uint32_t *)ptr) = value.type.Enumerated;
					break;
				default:
					STD_LOGE("no support ptr len");
					exit(-1);
					break;
			}
			break;
			
		case BACNET_APPLICATION_TAG_DATE:
			STD_LOGE("do not support BACNET_APPLICATION_TAG_DATE");
			exit(-1);
			break;
		case BACNET_APPLICATION_TAG_TIME:
			STD_LOGE("do not support BACNET_APPLICATION_TAG_TIME");
			exit(-1);
			break;
		case BACNET_APPLICATION_TAG_OBJECT_ID:
			memcpy(ptr, &(value.type.Object_Id), sizeof(BACNET_OBJECT_ID));
			break;
		default:
			break;
	}
	return 0;
FAIL:
	return -1;
}



static int analye_prop(bac_object_t *object, bac_msg_t *bac_msg)
{
	BACNET_READ_PROPERTY_DATA *data = (BACNET_READ_PROPERTY_DATA *)bac_msg->ptr;
	if(object->type_id.type != data->object_type || object->type_id.instance != data->object_instance)
		goto FAIL;

	void *ptr = NULL;
	int ptr_len = 0;
	STD_LOGD("analyze prop: %s", bactext_property_name(data->object_property));
	switch(data->object_property)
	{
		case PROP_PRESENT_VALUE:
			ptr = &object->present_value;
			switch(object->type_id.type)
			{
				case OBJECT_ANALOG_INPUT:
					ptr_len = 4;
					break;
				case OBJECT_ANALOG_OUTPUT:
					ptr_len = 4;
					break;
				case OBJECT_ANALOG_VALUE:
					ptr_len = 4;
					break;
				case OBJECT_BINARY_INPUT:
					ptr_len = 1;
					break;
				case OBJECT_BINARY_OUTPUT:
					ptr_len = 1;
					break;
				case OBJECT_BINARY_VALUE:
					ptr_len = 1;
					break;
				default:
					STD_LOGE("no support prop: %d, %s", data->object_property, bactext_property_name(data->object_property));
					exit(-1);
					break;
			}
			break;
		default:
			STD_LOGE("no support prop: %d, %s", data->object_property, bactext_property_name(data->object_property));
			exit(-1);
			break;
	}

	STD_ERROR_GOTO_FAIL(analyze_value(bac_msg, ptr, ptr_len));

	return 0;

	bac_device_t *device;	
FAIL:
	device = get_device_by_invoke_id(bac_msg->invoke_id);
	device->error_type = BAC_ANALYZE;
	return -1;
}

int read_object_prop(bac_object_t *object, int object_prop, uint32_t index)
{
	bac_msg_t *bac_msg = NULL;
	bac_msg = read_prop_void(object, object_prop, index);
	if(bac_msg == NULL)
		goto FAIL;

	STD_ERROR_GOTO_FAIL(analye_prop(object, bac_msg));
	free_bac_msg(bac_msg);
	return 0;
FAIL:
	free_bac_msg(bac_msg);
	return -1;
}

int read_device_prop(bac_device_t *device, int device_prop, uint32_t index, void *ptr)
{
	bac_msg_t *bac_msg = NULL;
	bac_msg = read_prop_void(device, device_prop, index);
	if(bac_msg == NULL)
		goto FAIL;

	STD_ERROR_GOTO_FAIL(analyze_value(bac_msg, ptr, -1));
	free_bac_msg(bac_msg);
	return 0;

FAIL:
	free_bac_msg(bac_msg);
	return -1;
}
