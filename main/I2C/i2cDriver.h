#ifndef __I2CDRIVER__
#define __I2CDRIVER__

#include "esp_err.h"

esp_err_t i2c_master_init(void);
void i2c_detect(void);

#endif