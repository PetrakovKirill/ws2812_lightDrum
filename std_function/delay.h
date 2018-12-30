#include <stdint.h>
#include "stm32f10x.h"

#ifndef DELAY_H
#define DELAY_H

#ifndef CLK_CPU
#define CLK_CPU 8000000
#endif /* CLK_CPU */

void Delay_ms(uint32_t arg);
void Delay_us(uint32_t arg);

void DelayTick(uint32_t tick);


#endif // DELAY_H
