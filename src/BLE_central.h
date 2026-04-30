#ifndef BLE_CENTRAL_H
#define BLE_CENTRAL_H

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
//#include "control_queue.h"

//-----------Claude insert start-----------------------------


// De her er service og characteristic værdier, de står seperat fordi vi måske skal bruge dem som de her værdier inde i .c filen..
// ellers kunne de skrives sammen med de defines under
#define BT_UUID_SERVICE_VAL BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef0)

#define BT_UUID_CHRC_VAL BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef1)


//SERVICE

//Når vi åbner en BT terminal fra telefonen og connecter til enheden, ser vi servicen og alle dens characteristics
//characteristics kan både være reads and writes, vores komemr bare til at være en write, og tror kun vi skal bruge en characteristic..

#define BT_UUID_SERVICE BT_UUID_DECLARE_128(BT_UUID_SERVICE_VAL)

//CHARACTERISTIC - den som vi skriver til inde i servicen.
#define BT_UUID_CHAR BT_UUID_DECLARE_128(BT_UUID_CHRC_VAL)


#define MAX_MSG_LEN 16

//-----------Claude insert end-------------------------------

uint8_t BLE_init();

#endif