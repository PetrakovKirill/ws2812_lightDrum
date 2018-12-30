#ifndef _HARDWARE_GPIO_
#define _HARDWARE_GPIO_

#include <stdint.h>
#include "stm32f10x.h"

#define PIN_STATE_HIGH  (1)
#define PIN_STATE_LOW   (0)

typedef enum pin_config {
    GPIO_ALTOUT_PP_50MHz  	= (0x0B),
    GPIO_ALTOUT_PP_10MHz	= (0x09),
    GPIO_ALTOUT_PP_2MHz		= (0x0A),

    GPIO_ALTOUT_OD_50MHz	= (0x0F),
    GPIO_ALTOUT_OD_10MHz	= (0x0D),
    GPIO_ALTOUT_OD_2MHz		= (0x0E),

    GPIO_OUT_PP_50MHz	    = (0x03),
    GPIO_OUT_PP_10MHz	    = (0x01),
    GPIO_OUT_PP_2MHz	    = (0x02),

    GPIO_OUT_OD_50MHz	    = (0x07),
    GPIO_OUT_OD_10MHz	    = (0x05),
    GPIO_OUT_OD_2MHz	    = (0x06),

    GPIO_INP_ANALOG	        = (0x00),
    GPIO_INP_FLOAT	        = (0x04),
    GPIO_INP_PUPD	        = (0x08)
} pin_config_t;


typedef struct pin {
    GPIO_TypeDef *gpio;
    uint32_t      pin;
} pin_t;


void     Pin_Init(void *obj, void *gpioAddr, uint32_t pinNum, pin_config_t pinConfig);
void     Pin_SetState(void *obj, uint32_t state);
uint32_t Pin_GetState(void *obj);
void Pin_PullDown(void *obj);
void Pin_PullUp(void *obj);



typedef struct port {
    uint32_t  pinNums;
    pin_t    *pinMap;
} port_t;


#endif /* _HARDWARE_GPIO_ */