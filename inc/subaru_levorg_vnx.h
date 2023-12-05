#ifndef __SUBARU_LEVORG_VNX_H__
#define __SUBARU_LEVORG_VNX_H__

// Receive Only Two CAN Ids
#define CAN_ID_CCU 0x390
#define CAN_ID_TCU 0x174

// CCU and TCU STATUS
enum cu_status {
    ENGINE_STOP,
    NOT_READY,
    READY,
    IDLING_STOP_ON,
    IDLING_STOP_OFF,
    CAN_FRAME_SENDED
};

// MODE
enum debug_mode {
    NORMAL,
    DEBUG,
    CANDUMP
};

extern enum debug_mode DebugMode;

// for Calculate Check Sum
#define SUM_CHECK_DIVIDER 365

#endif /* __SUBARU_LEVORG_VNX_H_ */
