#include "timer.h"

/* Функции инициализации режимов работы */
static void TimerInitMode_Counter(stm32f103_timer_t *_this);
static void TimerInitMode_InputCapture(stm32f103_timer_t *_this);
static void TimerInitMode_OutputCompare(stm32f103_timer_t *_this);
static void TimerInitMode_PwmGenerator(stm32f103_timer_t *_this);
static void TimerInitMode_Encoder(stm32f103_timer_t *_this);

/* Функции реализации основного интерфейса */
static int TimerStm32f103_Init(void *obj, unsigned mode);
static int TimerStm32f103_AttachObserverOverflow(void *obj, unsigned channel);

static void (*TimerInitMode[])(stm32f103_timer_t *_this) = {
    TimerInitMode_Counter,
    TimerInitMode_InputCapture,
    TimerInitMode_OutputCompare,
    TimerInitMode_PwmGenerator,
    TimerInitMode_Encoder
};

/* Таблица фиртуальных функций */
static vTable_iTimer iTimer_iFace = {
    TimerStm32f103_Init,
    TimerStm32f103_AttachObserverOverflow
};


void TimerStm32f103_Construct(void *obj, TIM_TypeDef *timerPeriph) {
    stm32f103_timer_t *_this = obj;
    _this->iFace.vTable = &iTimer_iFace;
    _this->timer = timerPeriph;
}


static int TimerStm32f103_Init(void *obj, unsigned mode) {
    stm32f103_timer_t *_this = obj;

    TimerInitMode[mode](_this);
    return (0);
}


static int TimerStm32f103_AttachObserverOverflow(void *obj, unsigned channel) {
    return (0);
}


static void TimerInitMode_Counter(stm32f103_timer_t *_this)       {}
static void TimerInitMode_InputCapture(stm32f103_timer_t *_this)  {}
static void TimerInitMode_OutputCompare(stm32f103_timer_t *_this) {}
static void TimerInitMode_PwmGenerator(stm32f103_timer_t *_this)  {}

static void TimerInitMode_Encoder(stm32f103_timer_t *_this) {
    _this->timer->CR1   = 0x00;
    _this->timer->PSC   = 0;
    _this->timer->ARR   = 3;
    _this->timer->CCER  = 0x00;
    _this->timer->CCMR1 = TIM_CCMR1_CC2S_0 | TIM_CCMR1_CC1S_0;  // (1 << 8) | (1 << 0);
    _this->timer->SMCR  = TIM_SMCR_SMS_1   | TIM_SMCR_SMS_0;    //(0x03 << 0);
    _this->timer->DIER  = TIM_DIER_UIE;
    _this->timer->CR1   = TIM_CR1_CEN;
}