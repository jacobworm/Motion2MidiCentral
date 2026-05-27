#include "BLE_Simulator.h"
#include "gestureEvent.h"

void BLE_Simulator_ThreadFunct(void *control_q_ptr, void *p2, void *p3) {
        (void)p2; 
        (void)p3;
        struct k_msgq *control_q = (struct k_msgq *)control_q_ptr; 
        int counter = 0; // Simuleret tæller for at generere forskellige hændelser
        while (1) {

            gestureEvent event = counter; // Simuleret gesturEvent, du kan ændre dette for at teste forskellige hændelser
            k_msgq_put(control_q, &event, K_NO_WAIT);
            k_sleep(K_SECONDS(1)); // Simulerer en forsinkelse mellem hændelser
            counter = (counter + 1) % 8; // Ruller gennem de forskellige gesturEvent værdier (0-7)
        }
}