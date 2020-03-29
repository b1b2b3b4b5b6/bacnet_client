/*
 * @Author: lumos
 * @Date: 2018-12-03 16:45:32
 * @LastEditors: lumos
 * @LastEditTime: 2018-12-04 12:09:25
 * @Description: file content
 * @Email: a1439458305@163.com
 */
#ifndef BAC_DEVICE_H
#define BAC_DEVICE_H

#include "bacdef.h"
#include "config.h"
#include "bactext.h"
#include "bacerror.h"
#include "iam.h"
#include "arf.h"
#include "tsm.h"
#include "address.h"
#include "npdu.h"
#include "apdu.h"
#include "device.h"
#include "net.h"
#include "datalink.h"
#include "whois.h"
#include "version.h"
/* some demo stuff needed */
#include "filename.h"
#include "handlers.h"
#include "client.h"
#include "txbuf.h"
#include "dlenv.h"

#include "std_common.h"
#include "bac_error.h"

typedef enum bac_error_type_t{
	NO_ERROR = 0,
	BAC_LOSE,
	BAC_SEND,
    BAC_TIME_OUT,
    BAC_ERROR,
    BAC_ABORT,
    BAC_REGECT,
    BAC_DECODE,
    BAC_ACK_TYPE,
	BAC_ANALYZE,
}bac_error_type_t;

struct bac_device_t;
typedef struct bac_object_t{
	BACNET_OBJECT_ID type_id;
	struct bac_device_t *device;
	union {
		uint32_t uint_value;
		int32_t int_value;
		float float_value;
		bool bool_value;
	}present_value;
	struct bac_object_t *next;
}bac_object_t;


typedef struct bac_device_t{
	uint16_t type;
    int error_type;
 	int error_class;
	int error_code;
	int reason;  
	uint32_t device_id;
	unsigned max_apdu;
	BACNET_ADDRESS address;
	uint8_t invoke_id;
	uint32_t object_count;
	bac_object_t *root_object;
	struct bac_device_t *next;
}bac_device_t;


int bac_object_add(bac_device_t *device, int type, uint32_t id);
bac_object_t *get_object_by_type_id(bac_device_t *device, uint16_t type, uint32_t id);
int object_traverse(bac_device_t *device, int (cb(bac_object_t *)));

bac_device_t *get_device_by_id(int device_id);
bac_device_t *get_device_by_invoke_id(int invoke_id);
bac_device_t *get_device_by_address(BACNET_ADDRESS * src);
bac_device_t *get_device_by_id_src(int invoke_id, BACNET_ADDRESS * src);
int check_device_address(bac_device_t *device);
bac_device_t *assert_device_id(uint32_t device_id);
int bac_device_add(uint32_t device_id);

void bac_device_init();
void bac_print_error(bac_device_t *device);
void bac_print_device(bac_device_t *device);

#endif