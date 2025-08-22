#ifndef __VOICE_H__
#define __VOICE_H__

#include "stdint.h"

#define VOICE_SEG_MAX		19
#define DATA_LEN			6

void play_temper(float temper);
void play_distance(uint32_t distance);

//void play_num(_roadblock *roadblock);
//void play_alarm(void);

#endif
