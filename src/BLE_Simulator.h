#ifndef BLE_SIMULATOR_H
#define BLE_SIMULATOR_H


#include <zephyr/types.h>
#include <stddef.h>
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

void BLE_Simulator_ThreadFunct(void *control_q_ptr, void *p2, void *p3);

#endif