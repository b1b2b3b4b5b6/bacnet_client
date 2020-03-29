/*
 * @Author: lumos
 * @Date: 2018-12-03 18:05:56
 * @LastEditors: lumos
 * @LastEditTime: 2018-12-03 18:06:14
 * @Description: file content
 * @Email: a1439458305@163.com
 */

#include "handle_object.h"

#define STD_LOCAL_LOG_LEVEL STD_LOG_DEBUG

int traversal_present_cb(bac_object_t *object)
{
    return read_object_prop(object, PROP_PRESENT_VALUE, -1);
}

