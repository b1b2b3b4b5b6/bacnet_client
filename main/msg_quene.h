/*
 * @Author: lumos
 * @Date: 2018-12-03 15:04:30
 * @LastEditors: lumos
 * @LastEditTime: 2018-12-03 17:37:05
 * @Description: file content
 * @Email: a1439458305@163.com
 */
#ifndef MSG_QUENE_H
#define MSG_QUENE_H

#include "std_common.h"
#include "fcntl.h"
#include "sys/stat.h"
#include "mqueue.h"

#include "bac_device.h"

typedef struct bac_msg{
	int type;
	uint8_t *ptr;
	int len;
	uint8_t invoke_id;
}bac_msg_t;

typedef enum msg_type
{
	ERROR_ACK = 0,
	MREAD_ACK,
	READ_ACK,
	WRITE_ACK,
	SUBCRIBE_ACK,
	SCOV_ACK,
}
msg_type_t;

int msg_init();
int msg_send(bac_msg_t *ptr);
int msg_receive(bac_msg_t *ptr, int time_s);
void print_bac_msg(bac_msg_t *bac_msg);
void free_bac_msg(bac_msg_t *bac_msg);
int check_ack_type(bac_msg_t *bac_msg, int ack_type);
bac_msg_t *wait_device_ack(bac_device_t *device, int ack_type);

#endif