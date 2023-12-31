//
// Engine auto start-stop system eliminator firmware for SUBARU Levorg VN5
//

#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"

#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "can.h"
#include "system.h"
#include "led.h"
#include "error.h"
#include "printf.h"
#include "subaru_levorg_vnx.h"

enum debug_mode DebugMode = NORMAL;

int main(void)
{
    // Storage for status and received message buffer
    CAN_RxHeaderTypeDef rx_msg_header;
    uint8_t rx_msg_data[8] = {0};
    uint8_t msg_buf[TX_BUF_SIZE];

    // Storage for transmit message buffer
    CAN_TxHeaderTypeDef tx_msg_header;
    tx_msg_header.IDE = CAN_ID_STD;
    tx_msg_header.StdId = CAN_ID_CCU;
    tx_msg_header.ExtId = 0;
    tx_msg_header.RTR = CAN_RTR_DATA;
    tx_msg_header.DLC = 8;
    uint8_t tx_msg_data[8] = {0};

    uint32_t CurrentTime;

    static enum cu_status TcuStatus = ENGINE_STOP;
    static enum cu_status CcuStatus = ENGINE_STOP;
    static uint16_t PreviousCanId = CAN_ID_CCU;

    // Initialize peripherals
    system_init();
    can_init();
    led_init();
    usb_init();

    led_blue_blink(2);

    can_enable();
            
    while(1)
    {
#ifdef DEBUG_MODE
	cdc_process();
#endif
        led_process();
        can_process();

        // If CAN message receive is pending, process the message
        if(is_can_msg_pending(CAN_RX_FIFO0))
        {
            can_rx(&rx_msg_header, rx_msg_data);

            if(DebugMode != NORMAL)
            {
                for (uint8_t i=0; i < TX_BUF_SIZE; i++) {
                    msg_buf[i] = '\0';
                }

                CurrentTime = HAL_GetTick();

                // Output all received message(s) to CDC port as candump -L
                sprintf_(msg_buf, "(%d.%03d000) can0 %03X#",
                         CurrentTime / 1000,
                         CurrentTime % 1000,
                         rx_msg_header.StdId);
                CDC_Transmit_FS(msg_buf, strlen(msg_buf));

                for (uint8_t i=0; i < TX_BUF_SIZE; i++) {
                    msg_buf[i] = '\0';
                }

                for (uint8_t i=0; i < rx_msg_header.DLC; i++) {
                    sprintf_(msg_buf,"%02X", rx_msg_data[i]);
                    CDC_Transmit_FS(msg_buf, 2);
                }

                CDC_Transmit_FS("\n", 1);
            }

            if(DebugMode != CANDUMP)
            {
                switch (rx_msg_header.StdId) {
                    case CAN_ID_TCU:
                        if ((rx_msg_data[2] & 0x08) != 0x08) {
                            TcuStatus = NOT_READY;
                        } else if (rx_msg_data[4] == 0xc0) {
                            TcuStatus = IDLING_STOP_OFF;
                        } else {
                            TcuStatus = IDLING_STOP_ON;
                        }
                        PreviousCanId = rx_msg_header.StdId;
                        break;

                    case CAN_ID_CCU:
                        if (PreviousCanId == CAN_ID_CCU) { // TCU don't transmit message
                            CcuStatus = ENGINE_STOP;
                            TcuStatus = ENGINE_STOP;
                        } else if (rx_msg_data[6] & 0x42) {
                            CcuStatus = NOT_READY;
                        } else if (CcuStatus == NOT_READY || TcuStatus == IDLING_STOP_OFF) {
                            CcuStatus = READY;
                        } else if (TcuStatus == IDLING_STOP_ON) { // Transmit message for eliminate engine auto stop
                            if (CcuStatus == CAN_FRAME_SENDED) { // Previous eliminate engine auto stop message failed
	                        if(DebugMode == DEBUG)
                                {
                                    for (uint8_t i=0; i < TX_BUF_SIZE; i++) {
                                        msg_buf[i] = '\0';
                                    }

                                    // Output Warning message
                                    sprintf_(msg_buf, "# Warning: Eliminate engine auto stop failure.\n");
                                    CDC_Transmit_FS(msg_buf, strlen(msg_buf));
                                }

                                CcuStatus = READY;

                            } else {

                                // Increment counter
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
                                tx_msg_data[0] = (tx_msg_data[1] + tx_msg_data[2] + tx_msg_data[3] + tx_msg_data[4] + tx_msg_data[5] + tx_msg_data[6] + tx_msg_data[7]) % SUM_CHECK_DIVIDER;

                                HAL_Delay(50); // 50ms delay like real CCU
                                can_tx(&tx_msg_header, tx_msg_data); // Transmit message

                                if(DebugMode == DEBUG)
                                {
                                    for (uint8_t i=0; i < TX_BUF_SIZE; i++) {
                                        msg_buf[i] = '\0';
                                    }

                                    CurrentTime = HAL_GetTick();

                                    // Output all transmitted message(s) to CDC port as candump -L
                                    sprintf_(msg_buf, "# (%d.%03d000) can0 %03X#%02X%02X%02X%02X%02X%02X%02X%02X\n",
                                             CurrentTime / 1000,
                                             CurrentTime % 1000,                        
                                             tx_msg_header.StdId,
                                             tx_msg_data[0],
                                             tx_msg_data[1],
                                             tx_msg_data[2],
                                             tx_msg_data[3],
                                             tx_msg_data[4],
                                             tx_msg_data[5],
                                             tx_msg_data[6],
                                             tx_msg_data[7]);
                                    CDC_Transmit_FS(msg_buf, strlen(msg_buf));
                                }

                                // Discard message(s) that received during HAL_delay()
                                do {
                                    can_rx(&rx_msg_header, rx_msg_data);
                                } while(is_can_msg_pending(CAN_RX_FIFO0));

                                CcuStatus = CAN_FRAME_SENDED;
                            }
                        } else { // Unexpected case
                            if(DebugMode == DEBUG)
                            {
                                for (uint8_t i=0; i < TX_BUF_SIZE; i++) {
                                    msg_buf[i] = '\0';
                                }

                                // Output Warning message
                                sprintf_(msg_buf, "# Warning: Unexpected case (CCU=%d TCU=%d).\n", CcuStatus, TcuStatus);
                                CDC_Transmit_FS(msg_buf, strlen(msg_buf));
                            }

                        }

                        PreviousCanId = rx_msg_header.StdId;
                        break;

                    default: // Unexpected can id
                        if(DebugMode == DEBUG)
                        {
                            for (uint8_t i=0; i < TX_BUF_SIZE; i++) {
                                msg_buf[i] = '\0';
                            }

                            // Output Warning message
                            sprintf_(msg_buf, "# Warning: Unexpected can id (0x%03x).\n", rx_msg_header.StdId);
                            CDC_Transmit_FS(msg_buf, strlen(msg_buf));
                        }

                        break;

                }
            }
        }
    }
}

