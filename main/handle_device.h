/*
 * @Author: lumos
 * @Date: 2018-12-03 17:40:25
 * @LastEditors: lumos
 * @LastEditTime: 2018-12-03 17:40:42
 * @Description: file content
 * @Email: a1439458305@163.com
 */
#ifndef HANDLE_DEVICE_H
#define HANDLE_DEVICE_H


#include "bac_read.h"

int dread_object_count(bac_device_t *device);
int dread_object_type_id(bac_device_t *device, uint32_t object_index, bac_object_t *object);

#endif