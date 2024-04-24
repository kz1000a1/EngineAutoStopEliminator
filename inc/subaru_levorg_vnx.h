#ifndef __SUBARU_LEVORG_VNX_H__
#define __SUBARU_LEVORG_VNX_H__

/* #define for DEBUG_MODE */
#define no_printf_(fmt, ...)         \
({                                   \
	if (0)                           \
		printf_(fmt, ##__VA_ARGS__); \
	0;                               \
})

#ifdef DEBUG_MODE
#define dprintf_(fmt, ...) \
	printf_(fmt, ##__VA_ARGS__)
#else
#define dprintf_(fmt, ...) \
	no_printf_(fmt, ##__VA_ARGS__)
#endif

// Receive Only Two CAN Ids
#define CAN_ID_CCU 0x390
#define CAN_ID_TCU 0x174

// CCU and TCU STATUS
enum cu_status {
    ENGINE_STOP,
    NOT_READY,
    READY,
    IDLING_STOP_ON,
    IDLING_STOP_OFF
};

// STATUS
enum status {
    PROCESSING,
    CANCELLED,
    FAILED,
    SUCCEEDED
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


#define MAX_RETRY 2

#endif /* __SUBARU_LEVORG_VNX_H_ */
