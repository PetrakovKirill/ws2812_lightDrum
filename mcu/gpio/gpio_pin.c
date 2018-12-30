#include "gpio.h"


void Pin_Init(void *obj, void *gpioAddr, uint32_t pinNum, pin_config_t pinConfig) {
    volatile uint32_t *ctrlReg;
    uint32_t           ctrlPin;
    pin_t *_this = obj;

    _this->gpio = gpioAddr;
    _this->pin  = pinNum;

    if (_this->pin < 8) {
        ctrlReg = &_this->gpio->CRL;
        ctrlPin = _this->pin;
    } else {
        ctrlReg = &_this->gpio->CRH;
        ctrlPin = _this->pin - 8;
    }

    ctrlPin *= 4;

    *ctrlReg &= ~(0x0F << ctrlPin);             /* Clear feald   */
    *ctrlReg |=  (pinConfig << ctrlPin);        /* Set configure */
}


void Pin_SetState(void *obj, uint32_t state) {
    pin_t *_this = obj;
    if (state) {
        _this->gpio->BSRR |= (1 << _this->pin);
    } else {
        _this->gpio->BRR  |= (1 << _this->pin);
    }
}


void Pin_PullUp(void *obj) {
    pin_t *_this = obj;
    _this->gpio->ODR |= (1 << _this->pin);
}


void Pin_PullDown(void *obj) {
    pin_t *_this = obj;
    _this->gpio->ODR &= ~(1 << _this->pin);
}


uint32_t Pin_GetState(void *obj) {
    pin_t *_this = obj;
    return ( (_this->gpio->IDR & (1 << _this->pin)) ? (1) : (0) );    
}


