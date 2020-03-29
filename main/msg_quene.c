/*
 * @Author: lumos
 * @Date: 2018-12-03 15:04:30
 * @LastEditors: lumos
 * @LastEditTime: 2018-12-03 17:37:03
 * @Description: file content
 * @Email: a1439458305@163.com
 */
#include "msg_quene.h"

#define MSG_QUENE "/bacnet_client"
#define MSG_COUNT 10
#define MSG_SIZE sizeof(bac_msg_t) 
#define MSG_PRI 5

#define STD_LOCAL_LOG_LEVEL STD_LOG_DEBUG
mqd_t mqd;

int msg_init()
{
	struct mq_attr attr = {0};
	attr.mq_maxmsg = MSG_COUNT;
	attr.mq_msgsize = MSG_SIZE;
	mq_unlink(MSG_QUENE);
	mqd = mq_open(MSG_QUENE, O_RDWR | O_CREAT, 0777, &attr);
	if(mqd == -1)
	{
		STD_LOGE("msg open fail");
		return -1;
	}
	STD_LOGI("msg init success");
	return 0;
}

int msg_send(bac_msg_t *ptr)
{
	int res;
	res = mq_send(mqd, (char *)ptr, MSG_SIZE, MSG_PRI);
	if(res != 0)
	{
		STD_LOGE("msg send fail");
		free_bac_msg(ptr);
	}
	else
	{
		STD_LOGV("msg send success");
	}
	return res;
}

int msg_receive(bac_msg_t *ptr, int time_s)
{
	uint pri;
	ssize_t res = 0;
    struct timespec tv;
	clock_gettime(CLOCK_REALTIME, &tv);
	tv.tv_sec += time_s;
	res = mq_timedreceive(mqd, (char *)ptr, MSG_SIZE, &pri, &tv);
	if(res == -1)
	{
		STD_LOGE("msg receive fail");
		return -1;
	}	
	else
	{
		STD_LOGV("msg receive success");
		return 0;
	}
} 

void print_bac_msg(bac_msg_t *bac_msg)
{
	STD_LOGV("type: %-4d | decode error: %-4d ",
			bac_msg->type,
			bac_msg->len);
}

void free_bac_msg(bac_msg_t *bac_msg)
{
	if(bac_msg == NULL)
		return;

	BACNET_READ_PROPERTY_DATA *data = (BACNET_READ_PROPERTY_DATA *)bac_msg->ptr;

	if(data == NULL)
	{
		STD_FREE(bac_msg);
		return;
	}

	if(data->application_data == NULL)
	{
		STD_FREE(data);
		STD_FREE(bac_msg);
		return;
	}
	STD_FREE(data->application_data);
	STD_FREE(data);
	STD_FREE(bac_msg);
}

int check_ack_type(bac_msg_t *bac_msg, int ack_type)
{
	if(bac_msg->type == ack_type)
		return 0;

	switch(ack_type)
	{
		case READ_ACK:
			STD_LOGE("NEED READ_ACK");
			break;
		case WRITE_ACK:
			STD_LOGE("NEED WRITE_ACK");
			break;	
		case SUBCRIBE_ACK:
			STD_LOGE("NEED SUBCRIBE_ACK");
			break;
		default:
			STD_LOGE("NEED undefined ack type");
			exit(-1);
			break;				
	}

	switch(bac_msg->type)
	{
		case READ_ACK:
			STD_LOGE("RETURN READ_ACK");
			break;
		case WRITE_ACK:
			STD_LOGE("RETURN WRITE_ACK");
			break;	
		case SUBCRIBE_ACK:
			STD_LOGE("RETURN SUBCRIBE_ACK");
			break;	
		default:
			STD_LOGE("RETURN undefined ack type");
			exit(-1);
			break;				
	}
	return -1;
}

bac_msg_t *wait_device_ack(bac_device_t *device, int ack_type)
{
	bac_msg_t *bac_msg = NULL;
	if(device->invoke_id <= 0)
	{
		STD_LOGE("invoke: %d faild, free it", device->invoke_id);
		device->error_type = BAC_SEND;
		goto FAIL;
	}
	STD_LOGD("wait invoke id: %d",device->invoke_id);

	bac_msg = calloc(sizeof(bac_msg_t), 1);
	int res = msg_receive(bac_msg, WAIT_TIME_OUT);
	
	if(res == -1)
	{
		if (tsm_invoke_id_failed(device->invoke_id))
		{
			STD_LOGE("invoke: %d faild, free it", device->invoke_id);
			device->error_type = BAC_TIME_OUT;
			tsm_free_invoke_id(device->invoke_id);
		}	
		else
		{
			STD_LOGE("msg fail, but invoke not fail");
			exit(-1);
		}
		device->invoke_id = 0;
		goto FAIL;
	}
	if(res == 0)
	{
		STD_LOGD("receive invoke id: %d, ok",device->invoke_id);
		if(bac_msg->type == ERROR_ACK)
		{
			STD_LOGE("server return error");
			goto FAIL;
		}
		if(check_ack_type(bac_msg, ack_type) != 0)
		{
			device->error_type = BAC_ACK_TYPE;
			goto FAIL;
		}
		return bac_msg;
	}	

FAIL:
	free_bac_msg(bac_msg);
	return NULL;
}