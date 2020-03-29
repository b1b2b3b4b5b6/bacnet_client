/*
 * @Author: lumos
 * @Date: 2018-12-03 15:04:30
 * @LastEditors: lumos
 * @LastEditTime: 2018-12-03 17:36:54
 * @Description: file content
 * @Email: a1439458305@163.com
 */
#ifndef CLIENT_READ_H
#define CLIENT_READ_H

#include "bac_device.h"
#include "msg_quene.h"


int read_object_prop(bac_object_t *object, int object_prop, uint32_t index);
int read_device_prop(bac_device_t *device, int device_prop, uint32_t index, void *ptr);

#endif