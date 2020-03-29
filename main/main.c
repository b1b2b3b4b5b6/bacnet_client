/*
 * @Author: lumos
 * @Date: 2018-12-03 15:04:30
 * @LastEditors: lumos
 * @LastEditTime: 2018-12-04 14:30:38
 * @Description: file content
 * @Email: a1439458305@163.com
 */

#include "std_common.h"
#include "client_method.h"

int main(int argc, char *argv[])
{

    bac_client_init();
    scan_device();
    mem_test(10000);	
	pthread_exit(NULL);
    return 0;
}
