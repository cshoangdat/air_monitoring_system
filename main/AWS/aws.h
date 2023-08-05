#ifndef __AWS__
#define __AWS__

#include "string.h"

extern const char cert_start[]               asm("_binary_AmazonRootCA1_pem_start");
extern const char cert_end[]                 asm("_binary_AmazonRootCA1_pem_end");
extern const char certificate_start[]        asm("_binary_certificate_pem_crt_start");
extern const char certificate_end[]          asm("_binary_certificate_pem_crt_end");
extern const char private_start[]            asm("_binary_private_pem_key_start");
extern const char private_end[]              asm("_binary_private_pem_key_end");

void AWS_Run(void);
void AWS_Stop(void);

#endif