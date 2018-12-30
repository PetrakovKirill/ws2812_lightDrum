#ifndef _TIMER_STM32F103_
#define _TIMER_STM32F103_

#include <stdint.h>
#include "stm32f10x.h"
#include "iTimer.h"

#define TIMER_MODE_COUNTER          (0)
#define TIMER_MODE_INP_CAPTURE      (1)
#define TIMER_MODE_OUT_COMPARE      (2)
#define TIMER_MODE_PWM_GEN          (3)
#define TIMER_MODE_ENCODER          (4)

typedef struct stm32f103_timer_t {
    iTimer iFace;

    TIM_TypeDef *timer;
} stm32f103_timer_t;


/* typedef struct outCompareMode_ {
    uint8_t ccs  : 2;
    uint8_t ocfe : 1;
    uint8_t ocpe : 1;
    uint8_t ocm  : 3;
    uint8_t occe : 1;
} outCompareMode_t; */

void TimerStm32f103_Construct(void *obj, TIM_TypeDef *timerPeriph);



#endif /* _TIMER_STM32F103_ */