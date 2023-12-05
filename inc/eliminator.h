#ifndef _ELIMINATOR_H
#define _ELIMINATOR_H

int8_t eliminator_parse_str(uint8_t *buf, uint8_t len);

// maximum rx buffer len: command length from USB CDC port
#define ELIMINATOR_MTU 2 // (sizeof("D0"))

#endif // _ELIMINATOR_H
