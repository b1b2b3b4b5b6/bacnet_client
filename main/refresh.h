#ifndef REFRESH_H
#define REFRESH_H

#include "bac_device.h"
#include "handle_object.h"

void polling_list_init();
void cov_list_init();
int add_monitor_object(bac_object_t *object);

void polling_loop(void);
void cov_loop(void);

#endif