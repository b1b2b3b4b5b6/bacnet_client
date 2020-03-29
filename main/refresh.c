#include "refresh.h"

#define STD_LOCAL_LOG_LEVEL STD_LOG_DEBUG

#define POLLING_LEN 2000
#define COV_LEN 2000
#define LIFE_TIME 30
#define COV_SPAN LIFE_TIME/2
#define POLLING_SPAN 4
#define COV_CONFIRME false

bac_object_t *g_polling_list[POLLING_LEN] = {0};
bac_object_t *g_cov_list[COV_LEN] = {0};

void polling_list_init()
{
    for(int n = 0; n < POLLING_LEN; n++)
        g_polling_list[n] = NULL;
}

void cov_list_init()
{
    for(int n = 0; n < COV_LEN;n++)
        g_cov_list[n] = NULL;
}

int add_polling(bac_object_t *object)
{
    for(int n = 0; n < POLLING_LEN; n++)
    {
        if(g_polling_list[n] == object)
            return 0;

        if(g_polling_list[n] == NULL)
        {
            g_polling_list[n] = object;
            return 0;
        }
    }
    return -1;
}

int add_cov(bac_object_t *object)
{
    assert(object);
    for(int n = 0; n < COV_LEN; n++)
    {
        if(g_cov_list[n] == object)
            return 0;
        if(g_cov_list[n] == NULL)
        {
            g_cov_list[n] = object;
            return 0;
        }
    }
    return -1;
}

int subcribe_object(bac_object_t *object, uint32_t precess)
{
    bac_device_t *device;
    bac_msg_t *bac_msg = NULL;
    static BACNET_SUBSCRIBE_COV_DATA cov_data;
    memset(&cov_data, 0, sizeof(BACNET_SUBSCRIBE_COV_DATA));
    cov_data.monitoredObjectIdentifier.type = object->type_id.type;
    cov_data.monitoredObjectIdentifier.instance = object->type_id.instance;
    cov_data.subscriberProcessIdentifier = precess;
    cov_data.issueConfirmedNotifications = COV_CONFIRME;
    cov_data.lifetime = LIFE_TIME;
    device = object->device;
    STD_ERROR_GOTO_FAIL(check_device_address(device));
    device->invoke_id = Send_COV_Subscribe_Address(&device->address, device->max_apdu, &cov_data);
    bac_msg = wait_device_ack(device, SUBCRIBE_ACK);
    if(bac_msg)
    {
        free_bac_msg(bac_msg);
        return 0;
    }
        
FAIL:
    free_bac_msg(bac_msg);
	return -1;
}

static int get_cov_free()
{
    for(int n = 0; n < COV_LEN; n++)
    {
        if(g_cov_list[n] == NULL)
            return n;
    }
    STD_LOGE("cov list has full");
    return -1;
}

static int get_polling_free()
{
    for(int n = 0; n < POLLING_LEN; n++)
    {
        if(g_polling_list[n] == NULL)
            return n;
    }
    STD_LOGE("polling list has full");
    return -1;
}

int add_monitor_object(bac_object_t *object)
{
    int index;
    index = get_cov_free();
    if(index == -1)
        goto FAIL;
    if(subcribe_object(object, index) == 0)
    {
        g_cov_list[index] = object;
        STD_LOGI("obejct has been add cov, type: %s, id: %d",
            bactext_object_type_name(object->type_id.type),
            object->type_id.instance);
        return 0;
    }

    STD_LOGI("cov fail, try to polling");

    index = get_polling_free();
    if(index == -1)
        goto FAIL;   
    g_polling_list[index] = object;
    STD_LOGI("obejct has been add polling, type: %s, id: %d",
        bactext_object_type_name(object->type_id.type),
        object->type_id.instance);   
    return 0; 
FAIL:
    return -1;
}

void polling_loop(void)
{
    for(;;)
    {
        sleep(POLLING_SPAN);
        STD_LOGD("polling loop awake");
        for(int n = 0; n < POLLING_LEN; n++)
        {
            if(g_polling_list[n] != NULL)
            {
                traversal_present_cb(g_cov_list[n]);
            }
        }
    }
}

void cov_loop(void)
{
    for(;;)
    {
        sleep(COV_SPAN);
        STD_LOGD("cov loop awake");
        for(int n = 0; n < COV_LEN; n++)
        {
            if(g_cov_list[n] != NULL)
            {
                subcribe_object(g_cov_list[n], n);
            }
        }
    }
}