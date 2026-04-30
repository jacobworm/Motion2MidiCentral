#include "BLE_central.h"

//Det som bliver advertised af enheden, sætter den i genrel mode
// og giver den service UUID værdien, så enheder an se hvilken service den har
struct bt_data ad[] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL)),
        BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_SERVICE_VAL),
};


//Handler til write
//https://docs.zephyrproject.org/apidoc/latest/group__bt__gatt.html#ga3fd8527a0f3e8f3699dc0d3b0339eba1
//taget direkte her fra
//Tror kun vi skal bruge buf og len, da det er dte som holder data
ssize_t on_write(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len,
uint16_t offset, uint8_t flags)
{
        char msg[MAX_MSG_LEN + 1] = {0};        //gør plads til 0 terminering

        //hvis beskeden er for lang bliver den kortet af til MAX_MSG_LEN
        //
        if (len > MAX_MSG_LEN) {
                len = MAX_MSG_LEN;
        }

        memcpy(msg, buf, len);                  //Kopir buf over i char arrayet
        msg[len] = '\0';                        //0 terminering så vi ved at beskeden er slut

        printk("Besked modtaget: %s\n", msg);   //skriver char arrayet ud

        return len;                             //return len, noget zephyr har bestemt, noget error handeling
        }

// GATT service definition
BT_GATT_SERVICE_DEFINE(custom_svc,
        BT_GATT_PRIMARY_SERVICE(BT_UUID_SERVICE),
        BT_GATT_CHARACTERISTIC(
                        BT_UUID_CHAR,
                        BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                        BT_GATT_PERM_WRITE,
                        NULL, on_write, NULL),
);

//connection funktions strukturen kommer her fra
//https://docs.zephyrproject.org/apidoc/latest/structbt__conn__cb.html#ab3618150bfeea9492095ba27ce978c69
//søg: connected og disconnected
void connected(struct bt_conn *conn, uint8_t err)
{
        if (err) {
                printk("Forbindelse fejlede (err %d)\n", err);
        } else {
                printk("Enhed forbundet!\n");
        }
}


void disconnected(struct bt_conn *conn, uint8_t reason)
{
        printk("Enhed frakoblet (reason %d)\n", reason);
}

//opsætnign af de her callbacks kommer også fra den her github samt overstående link til dokumentation
//https://github.com/zephyrproject-rtos/zephyr/blob/main/samples/bluetooth/peripheral/src/main.c
BT_CONN_CB_DEFINE(conn_callbacks) = {
        .connected    = connected,
        .disconnected = disconnected,
};



uint8_t BLE_init()
{
        int err;

        err = bt_enable(NULL);
        if (err) {
                printk("bt enable fejlede: %d\n", err);
                return err;
        }
        printk("Bluetooth aktiveret!\n");
        /*
        https://docs.zephyrproject.org/apidoc/latest/group__bt__gap.html#gaa700527b1caf3bef27d96a3f91a29f69
        søg - BT_LE_ADV_CONN_FAST_1
        eksemplet fra git brugte BT_LE_ADV_CONN_FAST_1 som er en færdig macro
        den er advertiser dog ret hurtigt og derfor bruger nok en del strøm.
        derfor prøver vi lige at lave en langsom en for at spare ressourcer
        man kan nemlig definere sin egen med BT_LE_ADV_PARAM

        BT_GAP_ADV_SLOW_INT_MIN = 1 s
        BT_GAP_ADV_SLOW_INT_MAX = 1.2 s
        til forskel for
        BT_LE_ADV_CONN_FAST_1 som er mellem 30 og 60 ms
        men tror egenligt kun det gælder når den advertiser og ikke når en enhed er forbundet til den..
        */
        err = bt_le_adv_start(BT_LE_ADV_PARAM(BT_LE_ADV_OPT_CONN,
                      BT_GAP_ADV_SLOW_INT_MIN,
                      BT_GAP_ADV_SLOW_INT_MAX,
                      NULL), ad, ARRAY_SIZE(ad), NULL, 0);

        if (err) {
                printk("Advertising fejlede: %d\n", err);
                return err;
        }
        printk("Advertiser startet — venter på forbindelse...\n");

        return 0;
}