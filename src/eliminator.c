//
// eliminator: Parse an incoming command from the USB CDC port and change function
//

#include "stm32f0xx_hal.h"
#include <string.h>
#include "can.h"
#include "error.h"
#include "printf.h"
#include "usbd_cdc_if.h"
#include "subaru_levorg_vnx.h"

// Parse an incoming command from the USB CDC port
int8_t eliminator_parse_str(uint8_t *buf, uint8_t len)
{

    // Convert from ASCII (2nd character to end)
    for (uint8_t i = 1; i < len; i++)
    {
        // Lowercase letters
        if(buf[i] >= 'a')
            buf[i] = buf[i] - 'a' + 10;
        // Uppercase letters
        else if(buf[i] >= 'A')
            buf[i] = buf[i] - 'A' + 10;
        // Numbers
        else
            buf[i] = buf[i] - '0';
    }

    // Process command
    switch(buf[0])
    {
		case 'm':
		case 'M':
			// Set mode command
			switch(buf[1])
			{
				case 0:
					// Mode 0: eliminator silent mode(default)
					if(DebugMode == CANDUMP)
					{
						can_filter_enable();
					}
					DebugMode = NORMAL;
					CDC_Transmit_FS("NORMAL MODE\n", 12);
					break;
				
				case 1:
					// Mode 1: eliminator debug mode
					if(DebugMode == CANDUMP)
					{
						can_filter_enable();
					}
					DebugMode = DEBUG;
					CDC_Transmit_FS("DEBUG MODE\n", 11);
					break;
				
				case 2:
					// Mode 2: candump mode
					if(DebugMode != CANDUMP)
					{
						can_filter_disable();
					}
					DebugMode = CANDUMP;
					CDC_Transmit_FS("CANDUMP MODE\n", 13);
					break;
			}
			
			break;

		case 'v':
		case 'V':
		{
			// Report firmware version and remote
			char* fw_id = GIT_VERSION " " GIT_REMOTE "\n";
			CDC_Transmit_FS((uint8_t*)fw_id, strlen(fw_id));
			break;
		}

    		default:
    		// Error, unknown command
    		return -1;
    }

    return 0;
}

