#ifndef __NEXTION__
#define __NEXTION__

#include "stdio.h"
#include "string.h"

void initNextion(void);
int sendData(const char* data);
void NextionRun(void);
void NextionStop(void);

#endif