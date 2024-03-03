//
// Engine auto start-stop system eliminator firmware for SUBARU Levorg VN5
//

#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"

#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "can.h"
#include "led.h"
#include "system.h"
#include "error.h"
#include "printf.h"
#include "subaru_levorg_vnx.h"

#ifdef DEBUG_MODE
    enum debug_mode DebugMode = DEBUG;
#else
    enum debug_mode DebugMode = NORMAL;
#endif


void print_rx_frame(CAN_RxHeaderTypeDef* rx_msg_header, uint8_t* rx_msg_data){
    uint32_t CurrentTime;

    CurrentTime = HAL_GetTick();

    // Output all received message(s) to CDC port as candump -L
    if(rx_msg_header->RTR == CAN_RTR_DATA){ // Data Frame
        printf_("(%d.%03d000) can0 %03X#", CurrentTime / 1000,
                                           CurrentTime % 1000,
                                           rx_msg_header->StdId);
        for (uint8_t i=0; i < rx_msg_header->DLC; i++) {
            printf_("%02X", rx_msg_data[i]);
        }
        printf_("\n");
    } else { // Remote Frame
        printf_("(%d.%03d000) can0 %03X#R%d\n", CurrentTime / 1000,
                                                CurrentTime % 1000,
                                                rx_msg_header->StdId,
                                                rx_msg_header->DLC);
    }
}


void print_tx_frame(CAN_TxHeaderTypeDef* tx_msg_header, uint8_t* tx_msg_data){
    uint32_t CurrentTime;

    CurrentTime = HAL_GetTick();

    // Output all received message(s) to CDC port as candump -L
    printf_("(%d.%03d000) can0 %03X#%02X%02X%02X%02X%02X%02X%02X%02X\n",
                                CurrentTime / 1000,
                                CurrentTime % 1000,
                                tx_msg_header->StdId,
                                tx_msg_data[0],
                                tx_msg_data[1],
                                tx_msg_data[2],
                                tx_msg_data[3],
                                tx_msg_data[4],
                                tx_msg_data[5],
                                tx_msg_data[6],
                                tx_msg_data[7]);
}


}void send_cancel_frame(uint8_t* rx_msg_data){
    // Storage for transmit message buffer
    CAN_TxHeaderTypeDef tx_msg_header;
    tx_msg_header.IDE = CAN_ID_STD;
    tx_msg_header.StdId = CAN_ID_CCU;
    tx_msg_header.ExtId = 0;
    tx_msg_header.RTR = CAN_RTR_DATA;
    tx_msg_header.DLC = 8;
    uint8_t tx_msg_data[8] = {0};

    if ((rx_msg_data[1] & 0x0f) == 0x0f) {
        tx_msg_data[1] = rx_msg_data[1] & 0xf0;
    } else {
        tx_msg_data[1] = rx_msg_data[1] + 0x01;
    }
    tx_msg_data[2] = rx_msg_data[2];
    tx_msg_data[3] = rx_msg_data[3];
    tx_msg_data[4] = rx_msg_data[4];
    tx_msg_data[5] = rx_msg_data[5];
    tx_msg_data[6] = rx_msg_data[6] | 0x40; // Eliminate engine auto stop bit on
    tx_msg_data[7] = rx_msg_data[7];
    // Calculate checksum
    tx_msg_data[0] = (tx_msg_data[1] +
                      tx_msg_data[2] +
                      tx_msg_data[3] +
                      tx_msg_data[4] +
                      tx_msg_data[5] +
                      tx_msg_data[6] +
                      tx_msg_data[7]) % SUM_CHECK_DIVIDER;
    can_tx(&tx_msg_header, tx_msg_data); // Queueing message
    can_process(); // Transmit message
    if(DebugMode == DEBUG){
        printf_("# ");
        print_tx_frame(&tx_msg_header, tx_msg_data);
    }
}


void led_blink(enum status Status){
    if(Status & 1){
        led_orange_on();
    } else {
        led_orange_off();
    }
    if(Status & 2){
        led_green_on();
    } else {
        led_green_off();
    }
}


int main(void)
{
    // Storage for status and received message buffer
    CAN_RxHeaderTypeDef rx_msg_header;
    uint8_t rx_msg_data[8] = {0};

    static enum cu_status TcuStatus = ENGINE_STOP;
    static enum cu_status CcuStatus = ENGINE_STOP;
    static enum status Status = PROCESSING;
    static uint16_t PreviousCanId = CAN_ID_CCU;
    static uint8_t Retry = 0;

    // Initialize peripherals
    system_init();
    can_init();
    led_init();
#ifdef DEBUG_MODE
    usb_init();
#endif

    can_enable();

    while(1){
#ifdef DEBUG_MODE
	cdc_process();
#endif

        // If CAN message receive is pending, process the message
        if(is_can_msg_pending(CAN_RX_FIFO0)){
            can_rx(&rx_msg_header, rx_msg_data);

            if(DebugMode != NORMAL){
                print_rx_frame(&rx_msg_header, rx_msg_data);
            }

            if(rx_msg_header.RTR != CAN_RTR_DATA || rx_msg_header.DLC != 8){
                continue;
            }

            if(DebugMode != CANDUMP){
                switch (rx_msg_header.StdId) {
                    case CAN_ID_TCU:
                        if ((rx_msg_data[2] & 0x08) != 0x08) {
                            TcuStatus = NOT_READY;
                        } else if (rx_msg_data[4] == 0xc0) {
                            TcuStatus = IDLING_STOP_OFF;
                            if (Retry != 0 && Status == PROCESSING) {
	                        if(DebugMode == DEBUG){
                                    // Output Warning message
                                    printf_("# Warning: Eliminate engine auto stop succeeded.\n");
                                }
                                Status = SUCCEEDED;
                                led_blink(Status);
                            }
                        } else {
                            TcuStatus = IDLING_STOP_ON;
                            if (Status == SUCCEEDED) {
	                        if(DebugMode == DEBUG){
                                    // Output Warning message
                                    printf_("# Warning: Eliminate engine auto stop restarted.\n");
                                }
                                Status = PROCESSING;
                                led_blink(Status);
                                CcuStatus = NOT_READY;
                                Retry = 0;
                            }
                        }
                        PreviousCanId = rx_msg_header.StdId;
                        break;

                    case CAN_ID_CCU:
                        if (PreviousCanId == CAN_ID_CCU) { // TCU don't transmit message
                            CcuStatus = ENGINE_STOP;
                            TcuStatus = ENGINE_STOP;
                            Status = PROCESSING;
                            led_blink(Status);
                            Retry = 0;
                        } else if (rx_msg_data[6] & 0x40) {
                            if(DebugMode == DEBUG){
                                // Output Warning message
                                printf_("# Warning: Eliminate engine auto stop cancelled.\n");
                            }
                            Status = CANCELLED;
                            led_blink(Status);
                        } else if (Status == PROCESSING) {
                            if (CcuStatus == NOT_READY || CcuStatus == ENGINE_STOP || TcuStatus == IDLING_STOP_OFF) {
                                CcuStatus = READY;
                            } else if (TcuStatus == IDLING_STOP_ON) { // Transmit message for eliminate engine auto stop
                                if (MAX_RETRY <= Retry) { // Previous eliminate engine auto stop message failed
                                    if(DebugMode == DEBUG){
                                        // Output Warning message
                                        printf_("# Warning: Eliminate engine auto stop failed\n");
                                    }
                                    Status = FAILED;
                                    led_blink(Status);
                                } else {
                                    Retry++;
                                    led_blink(Retry);
                                    // HAL_Delay(50); // 50ms delay like real CCU
                                    HAL_Delay(50 / 2);
                                    send_cancel_frame(rx_msg_data); // Transmit message
                                    // Discard message(s) that received during HAL_delay()
                                    while(is_can_msg_pending(CAN_RX_FIFO0)){
                                        can_rx(&rx_msg_header, rx_msg_data);
                                    }
                                    CcuStatus = NOT_READY;
                                    led_blink(Status);
                                }
                            } else { // Unexpected case
                                if(DebugMode == DEBUG){
                                    // Output Warning message
                                    printf_("# Warning: Unexpected case (CCU=%d TCU=%d).\n", CcuStatus, TcuStatus);
                                }
                            }
                        }
                        PreviousCanId = rx_msg_header.StdId;
                        break;

                    default: // Unexpected can id
                        if(DebugMode == DEBUG){
                            // Output Warning message
                            printf_("# Warning: Unexpected can id (0x%03x).\n", rx_msg_header.StdId);
                        }
                        break;
                }
            }
        }
    }
}

