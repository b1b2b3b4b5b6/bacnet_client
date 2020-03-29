/*
 * @Author: lumos
 * @Date: 2018-12-03 15:04:30
 * @LastEditors: lumos
 * @LastEditTime: 2018-12-03 18:01:03
 * @Description: file content
 * @Email: a1439458305@163.com
 */
#include "bac_client.h"

#define STD_LOCAL_LOG_LEVEL STD_LOG_DEBUG

/* buffer used for receive */
static uint8_t Rx_Buf[MAX_MPDU] = { 0 };

static void MyErrorHandler(
    BACNET_ADDRESS * src,
    uint8_t invoke_id,
    BACNET_ERROR_CLASS error_class,
    BACNET_ERROR_CODE error_code)
{
    bac_device_t *device = get_device_by_id_src(invoke_id, src);
    if (device != NULL) {
        STD_LOGE("BACnet Error: %s: %s\r\n",
            bactext_error_class_name((int) error_class),
            bactext_error_code_name((int) error_code));
        device->error_type = BAC_ERROR;
        device->error_class = error_class;
        device->error_code = error_code;
        bac_msg_t bac_msg = {0};
        bac_msg.type = ERROR_ACK;
        msg_send(&bac_msg);

    }
	else
		STD_LOGE("receive unknow error ack");

}

static void MyAbortHandler(
    BACNET_ADDRESS * src,
    uint8_t invoke_id,
    uint8_t abort_reason,
    bool server)
{
    bac_device_t *device = get_device_by_id_src(invoke_id, src);
    if (device != NULL) {
        STD_LOGE("BACnet Abort: %s\r\n", bactext_abort_reason_name((int) abort_reason));
        device->error_type = BAC_ABORT;
        device->reason = abort_reason;
        bac_msg_t bac_msg = {0};
        bac_msg.type = ERROR_ACK;
        msg_send(&bac_msg);
    }
	else
		STD_LOGE("receive unknow abort ack");

}

static void MyRejectHandler(
    BACNET_ADDRESS * src,
    uint8_t invoke_id,
    uint8_t reject_reason)
{
    bac_device_t *device = get_device_by_id_src(invoke_id, src);
    if (device != NULL) {
        STD_LOGE("BACnet Reject: %s\r\n", bactext_reject_reason_name((int) reject_reason));
        device->error_type = BAC_REGECT;
        device->reason = reject_reason;
        bac_msg_t bac_msg = {0};
        bac_msg.type = ERROR_ACK;
        msg_send(&bac_msg);
    }
	else
		STD_LOGE("receive unknow reject ack");
}




static void My_Read_Property_Ack_Handler(
    uint8_t * service_request,
    uint16_t service_len,
    BACNET_ADDRESS * src,
    BACNET_CONFIRMED_SERVICE_ACK_DATA * service_data)
{
    int len = 0;
    bac_device_t *device = get_device_by_id_src(service_data->invoke_id, src);
    if (device != NULL) {
		
		BACNET_READ_PROPERTY_DATA *data = calloc(1, sizeof(BACNET_READ_PROPERTY_DATA));

        len = rp_ack_decode_service_request(service_request, service_len, data);
        if (len > 0) {
            bac_msg_t bac_msg = {0};
            bac_msg.type = READ_ACK;
			uint8_t *temp = calloc(data->application_data_len, 1);
			memcpy(temp, data->application_data, data->application_data_len);
			data->application_data = temp;
			bac_msg.ptr = (uint8_t *)data;
			bac_msg.len = sizeof(BACNET_READ_PROPERTY_DATA);
            bac_msg.invoke_id = device->invoke_id;
            msg_send(&bac_msg);
		}
		else
		{
			STD_FREE(data);
			STD_LOGE("decode rp ack fail");
            device->error_type = BAC_DECODE;
		}
	}
	else
		STD_LOGE("receive unknow read ack");
}

/** Handler for a ReadPropertyMultiple ACK.
 * @ingroup DSRPM
 * For each read property, print out the ACK'd data,
 * and free the request data items from linked property list.
 *
 * @param service_request [in] The contents of the service request.
 * @param service_len [in] The length of the service_request.
 * @param src [in] BACNET_ADDRESS of the source of the message
 * @param service_data [in] The BACNET_CONFIRMED_SERVICE_DATA information
 *                          decoded from the APDU header of this message.
 */
// void My_Read_Property_Multiple_Ack_Handler(
//     uint8_t * service_request,
//     uint16_t service_len,
//     BACNET_ADDRESS * src,
//     BACNET_CONFIRMED_SERVICE_ACK_DATA * service_data)
// {
//     int len = 0;
//     BACNET_READ_ACCESS_DATA *rpm_data;
//     BACNET_READ_ACCESS_DATA *old_rpm_data;
//     BACNET_PROPERTY_REFERENCE *rpm_property;
//     BACNET_PROPERTY_REFERENCE *old_rpm_property;
//     BACNET_APPLICATION_DATA_VALUE *value;
//     BACNET_APPLICATION_DATA_VALUE *old_value;

//     if (address_match(&Target_Address, src) &&
//         (service_data->invoke_id == Request_Invoke_ID)) {
//         rpm_data = calloc(1, sizeof(BACNET_READ_ACCESS_DATA));
//         if (rpm_data) {
//             len =
//                 rpm_ack_decode_service_request(service_request, service_len,
//                 rpm_data);
//         }
//         if (len > 0) {
//             while (rpm_data) {
//                 rpm_ack_print_data(rpm_data);
//                 rpm_property = rpm_data->listOfProperties;
//                 while (rpm_property) {
//                     value = rpm_property->value;
//                     while (value) {
//                         old_value = value;
//                         value = value->next;
//                         free(old_value);
//                     }
//                     old_rpm_property = rpm_property;
//                     rpm_property = rpm_property->next;
//                     free(old_rpm_property);
//                 }
//                 old_rpm_data = rpm_data;
//                 rpm_data = rpm_data->next;
//                 free(old_rpm_data);
//             }
//         } else {
//             fprintf(stderr, "RPM Ack Malformed! Freeing memory...\n");
//             while (rpm_data) {
//                 rpm_property = rpm_data->listOfProperties;
//                 while (rpm_property) {
//                     value = rpm_property->value;
//                     while (value) {
//                         old_value = value;
//                         value = value->next;
//                         free(old_value);
//                     }
//                     old_rpm_property = rpm_property;
//                     rpm_property = rpm_property->next;
//                     free(old_rpm_property);
//                 }
//                 old_rpm_data = rpm_data;
//                 rpm_data = rpm_data->next;
//                 free(old_rpm_data);
//             }
//         }
//     }
// }

void My_Unconfirmed_COV_Notification_Handler(
    uint8_t * service_request,
    uint16_t service_len,
    BACNET_ADDRESS * src)
{
    bac_device_t *device = get_device_by_address(src);
    if (device != NULL) {
        handler_ucov_notification(service_request, service_len, src);
    }
	else
		STD_LOGE("receive unknow ucov");

    
}

void My_Confirmed_COV_Notification_Handler(
    uint8_t * service_request,
    uint16_t service_len,
    BACNET_ADDRESS * src,
    BACNET_CONFIRMED_SERVICE_DATA * service_data)
{
    bac_device_t *device = get_device_by_address(src);
    if (device != NULL) {
        handler_ccov_notification(service_request, service_len, src, service_data);
    }
	else
		STD_LOGE("receive unknow cov");
}

void My_Confirmed_Simple_Ack_Handler(
    BACNET_ADDRESS * src,
    uint8_t invoke_id)
{
    bac_device_t *device = get_device_by_id_src(invoke_id, src);
    if (device != NULL) {
        bac_msg_t bac_msg = {0};
        bac_msg.type = SUBCRIBE_ACK;
        bac_msg.invoke_id = invoke_id;
        msg_send(&bac_msg);
    }
	else
		STD_LOGE("receive unknow cov ack");
}


static void Init_Service_Handlers(void)
{
    Device_Init(NULL);
    /* handle i-am to support binding to other devices */
    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_I_AM, handler_i_am_add);
    /* set the handler for all the services we don't implement
       It is required to send the proper reject message... */
    apdu_set_unrecognized_service_handler_handler(handler_unrecognized_service);
    /* we must implement read property - it's required! */
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_READ_PROPERTY,
        handler_read_property);
    /* handle the data coming back from confirmed requests */
    apdu_set_confirmed_ack_handler(SERVICE_CONFIRMED_READ_PROPERTY,
        My_Read_Property_Ack_Handler);
    /* handle any errors coming back */

    apdu_set_confirmed_handler(SERVICE_CONFIRMED_COV_NOTIFICATION,
        My_Confirmed_COV_Notification_Handler);
    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_COV_NOTIFICATION,
        My_Unconfirmed_COV_Notification_Handler);
    /* handle the Simple ack coming back from SubscribeCOV */
    apdu_set_confirmed_simple_ack_handler(SERVICE_CONFIRMED_SUBSCRIBE_COV,
        My_Confirmed_Simple_Ack_Handler);

    apdu_set_error_handler(SERVICE_CONFIRMED_READ_PROPERTY, MyErrorHandler);
    apdu_set_error_handler(SERVICE_CONFIRMED_SUBSCRIBE_COV, MyErrorHandler);
    apdu_set_abort_handler(MyAbortHandler);
    apdu_set_reject_handler(MyRejectHandler);
}


static void *receive_task(void *arg)
{
    BACNET_ADDRESS src = {
        0
    };  /* address where message came from */
    uint16_t pdu_len = 0;
    unsigned timeout = 100;     /* milliseconds */
    time_t elapsed_seconds = 0;
    time_t last_seconds = 0;
    time_t current_seconds = 0;

    /* setup my info */
    /* configure the timeout values */
    last_seconds = time(NULL);

    /* send the request */
    /* loop forever */
    for (;;) {
        /* increment timer - exit if timed out */
        current_seconds = time(NULL);
        /* returns 0 bytes on timeout */
        pdu_len = datalink_receive(&src, &Rx_Buf[0], MAX_MPDU, timeout);

        if (pdu_len) {
            npdu_handler(&src, &Rx_Buf[0], pdu_len);
        }
        /* increment timer - exit if timed out */
        elapsed_seconds = current_seconds - last_seconds;
        if (elapsed_seconds) {
			last_seconds = current_seconds;
			#if defined(BACDL_BIP) && BBMD_ENABLED
				bvlc_maintenance_timer(elapsed_seconds);
			#endif
			tsm_timer_milliseconds(elapsed_seconds*1000);
			
        }
        /* keep track of time for next check */
    }
    return NULL;
}

static void *polling_task(void *arg)
{
    polling_loop();
    exit(-1);
}

static void *subscribe_cov_task(void *arg)
{
    cov_loop();
    exit(-1);
}


void bac_client_init()
{
	bac_device_init();
	Device_Set_Object_Instance_Number(BACNET_MAX_INSTANCE);
    address_init();
    Init_Service_Handlers();
    dlenv_init();
    atexit(datalink_cleanup);
    apdu_timeout_set(APDU_TIMEOUT*1000);
    apdu_retries_set(APDU_RETRY);

    pthread_t thread_receive;
    pthread_create(&thread_receive, NULL, receive_task, NULL);

    pthread_t thread_polling;
    pthread_create(&thread_polling, NULL, polling_task, NULL);
    
    pthread_t thread_subscribe_cov;
    pthread_create(&thread_subscribe_cov, NULL,subscribe_cov_task, NULL);
	msg_init();
}



