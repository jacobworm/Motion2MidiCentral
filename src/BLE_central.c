#include "BLE_central.h"
#include "gestureEvent.h"

// ------------------ subscribe struktur START ----------------
// kommer her fra https://github.com/zephyrproject-rtos/zephyr/blob/main/samples/bluetooth/central_hr/src/main.c#L73
static struct bt_gatt_subscribe_params subscribe_params;
static struct k_msgq *BLE_control_q;

static uint8_t notify_func(struct bt_conn *conn, struct bt_gatt_subscribe_params *params, const void *data, uint16_t length)
{
    if (data == NULL) {
        printk("unsubscribed\n");
        return BT_GATT_ITER_STOP;
    }

    uint8_t *buf = (uint8_t *)data;
    gestureEvent event = (gestureEvent)buf[0];
    k_msgq_put(BLE_control_q, &event, K_NO_WAIT);

    return BT_GATT_ITER_CONTINUE;
}

// ------------------ subscribe struktur END ----------------



//---------------- discover struktur START ---------------------
//kommer fra https://github.com/zephyrproject-rtos/zephyr/blob/main/samples/bluetooth/central_hr/src/main.c

static struct bt_gatt_discover_params discover_params;

static uint8_t discover_func(struct bt_conn *conn, const struct bt_gatt_attr *attr, struct bt_gatt_discover_params *params)
{
        if (attr == NULL) {
                subscribe_params.notify = notify_func;
                subscribe_params.value_handle = 52;  // UUID bestemt over på Photon. kommer på handle 52 (fundet i test)
                subscribe_params.ccc_handle = 53;    // CCCD handle
                subscribe_params.value = BT_GATT_CCC_NOTIFY;
                bt_gatt_subscribe(conn, &subscribe_params);
                printk("Discovery færdig - subscribing\n");
                return BT_GATT_ITER_STOP;
        }

        uint8_t uuid_str[BT_UUID_STR_LEN];
        bt_uuid_to_str(attr->uuid, uuid_str, sizeof(uuid_str));
        printk("Handle %d, UUID: %s\n", attr->handle, uuid_str);

        printk("Discovery: handle %d\n", attr->handle);
        return BT_GATT_ITER_CONTINUE;
}

//---------------- discover struktur END ---------------------


//-------- connect/disconnect struktur START----------------
//kommer fra https://github.com/zephyrproject-rtos/zephyr/blob/main/samples/bluetooth/central/src/main.c

static struct bt_conn *thinkerbell_conn;

static void connected(struct bt_conn *conn, uint8_t err)
{
        if (err) {
                printk("Forbindelse fejlede, err: %d\n", err);
                return;
        }
        printk("Forbundet til Thinkerbell!\n"); 

        //discovery parameter sættes her
        discover_params.uuid = NULL;           //default for at finde alt
        discover_params.func = discover_func;
        discover_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;          //alle atributter er en handle søgning, her starter vi bare fra den første af
        discover_params.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;             //og den skal søge op til maks mulige handles, selvom der i virkligheden måske kun er 5
        discover_params.type = BT_GATT_DISCOVER_ATTRIBUTE;                     // Discover Attributes of any type. - zephyr doc.
        bt_gatt_discover(conn, &discover_params);
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
        printk("Forbindelse afbrudt, reason: %d\n", reason);
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};
//-------- connect/disconnect struktur END----------------


//https://docs.zephyrproject.org/latest/doxygen/html/group__bt__gap.html#ga652eef01e5256e0d820cd1f4db877429 søg: bt_data_parse
static bool BLE_check_addr(struct bt_data *data, void *user_data)
{
        const bt_addr_le_t *addr = (const bt_addr_le_t *)user_data;     //skal caste userdata til bt_addr_le_t

        if (data->type == BT_DATA_UUID128_ALL) {                //TYPE: fandt ud af dte var nemmer at matche UUID end navn Thinkerbell

        const uint8_t uuid[] = {                                //Det UUID som er sat på Photon
                0xab, 0x90, 0x78, 0x56, 0x34, 0x12, 0x34, 0x12,
                0x34, 0x12, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12
        };

        if (memcmp(data->data, uuid, 16) == 0) {
                printk("Thinkerbell fundet!\n");
                //connect
                bt_le_scan_stop();              //stopper sacn
                //https://docs.zephyrproject.org/latest/doxygen/html/group__bt__conn.html#ga8d66f3e0262a51279e9fa8b3139252e6  søg: bt_conn_le_create
                bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN, BT_LE_CONN_PARAM_DEFAULT, &thinkerbell_conn);        //alt er bare default
                return false;
                }
        }


        return true;
}

//callback bt_le_scan_cb_t
//https://docs.zephyrproject.org/latest/doxygen/html/group__bt__gap.html#ga1c53d22b6e2dee38c825c58f3eeee9b4 søg: bt_le_scan_cb_t
static void BLE_find_photon(const bt_addr_le_t *addr, int8_t rssi, uint8_t adv_type, struct net_buf_simple *buf){
        bt_data_parse(buf, BLE_check_addr, (void *)addr);                       //cast til void fordi sådan er parse funktionen defineret
}


uint8_t BLE_scan(){
        uint8_t err;

        err = bt_le_scan_start(BT_LE_SCAN_ACTIVE, BLE_find_photon);
        if(err){
                printk("Scanning failed err -  %d\n", err);
                return err;
        }
	printk("Scan function started succesfully...\n");
        return 0;
}

uint8_t BLE_init(){

        uint8_t err;

	err = bt_enable(NULL);
	if (err) {
		printk("BLE_INIT failed err - %d\n", err);
		return 0;
	}

	printk("BLE_INIT succes\n");

	BLE_scan();
        return 0;
}


void BLE_ThreadFunct(void *control_q_ptr, void *p2, void *p3) {
        (void)p2; 
        (void)p3;
        BLE_control_q = (struct k_msgq *)control_q_ptr;

        BLE_init(); 

        while (1) {
                k_sleep(K_SECONDS(1));
        }
}