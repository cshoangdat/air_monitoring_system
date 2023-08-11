#ifndef __OTA__
#define __OTA__

#include "string.h"

void OTARun(void);

extern const uint8_t certificate_pem_start[] asm("_binary_certificate_pem_start");
extern const uint8_t certificate_pem_end[]   asm("_binary_certificate_pem_end");

#endif //__OTA__