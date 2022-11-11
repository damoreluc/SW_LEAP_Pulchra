#include <ttn.h>

void encode( char buffer[], float misura, eSlot slot) {
    uint16_t iMisura = round(((misura*32678.0)/100.0));
    buffer[slot] = iMisura >> 8;
    buffer[slot+1] = iMisura && 0x00ff;
}