/*
 * @Author: lumos
 * @Date: 2018-12-03 15:04:30
 * @LastEditors: lumos
 * @LastEditTime: 2018-12-03 17:36:47
 * @Description: file content
 * @Email: a1439458305@163.com
 */
#ifndef BAC_ERROR_H
#define BAC_ERROR_H

#include "std_common.h"





typedef struct bac_error_t{
 
}bac_error_t;

void bac_error_set(bac_error_t *in);
void bac_error_get(bac_error_t *out);

#endif