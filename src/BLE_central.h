#ifndef BLE_CENTRAL_H
#define BLE_CENTRAL_H


#include <zephyr/types.h>
#include <stddef.h>
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/sys/byteorder.h>

#define DEVICE_NAME "Thinkerbell"


uint8_t BLE_init();

uint8_t BLE_scan();

void BLE_ThreadFunct(void *control_q_ptr, void *p2, void *p3);


#endif