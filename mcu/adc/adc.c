#include "stm32f10x.h"
#include "adc.h"



static int AdcStm32f103_Init(void *obj);
static int AdcStm32f103_AttachChannel(void *obj, unsigned channel);
static int AdcStm32f103_GetValue(void *obj, unsigned channel);



static vTable_iAdc iAdc_iFace = {
    .Init          = AdcStm32f103_Init,
    .AttachChannel = AdcStm32f103_AttachChannel,
    .GetValue      = AdcStm32f103_GetValue
};



void AdcStm32f103_Construct(void *obj, ADC_TypeDef *adcPeriph, DMA_Channel_TypeDef *dmaPeriph) {
    stm32f103_adc_t *_this = obj;
    _this->iFace.vTable = &iAdc_iFace;
    _this->adc = adcPeriph;
    _this->dma = dmaPeriph;
}

static int AdcStm32f103_Init(void *obj) {
    stm32f103_adc_t *_this = obj;
    volatile int i = 10;

    _this->adc->CR1 |= ADC_CR1_SCAN;
    _this->adc->CR2 |= ADC_CR2_DMA | ADC_CR2_CONT |
                       ADC_CR2_EXTSEL | ADC_CR2_EXTTRIG;

    /* Сброс калибровки */
    // _this->adc->CR2 |= ADC_CR2_RSTCAL;
    // while(_this->adc->CR2 & ADC_CR2_RSTCAL);
    _this->adc->CR2 |= ADC_CR2_ADON;
    for (; i--;);
    /* Выполнение калибровки */
    _this->adc->CR2 |= ADC_CR2_CAL;
    while(_this->adc->CR2 & ADC_CR2_CAL);

    /* Конфигурирование DMA */
    _this->dma->CPAR  = (uint32_t)&_this->adc->DR;
    _this->dma->CMAR  = (uint32_t)_this->values;
    _this->dma->CNDTR = _this->activeChannels;
    _this->dma->CCR   = DMA_CCR2_MINC    |
                        DMA_CCR2_PSIZE_0 | 
                        DMA_CCR2_MSIZE_0 |
                        DMA_CCR2_CIRC    | 
                        DMA_CCR2_EN;

    _this->adc->CR2 |= ADC_CR2_SWSTART;
    return (0);
}


static int AdcStm32f103_AttachChannel(void *obj, unsigned channel) {
    stm32f103_adc_t *_this = obj;

    /* Конфигурация списка каналов АЦП */
    _this->mapValues[channel] = _this->activeChannels;
        if (_this->activeChannels < 7) {
            _this->adc->SQR3 |= (channel << (_this->activeChannels * 5));
        } else if (_this->activeChannels < 13) {
            _this->adc->SQR2 |= (channel << (_this->activeChannels * 5));
        } else {
            _this->adc->SQR1 |= (channel << (_this->activeChannels * 5));
        }
    
    /* Кличество преобразований */
    _this->adc->SQR1 &= ~(0x0F << 20);      /* Очистка поля  L */
    _this->adc->SQR1 |= (_this->activeChannels << 20);  

    _this->activeChannels++;
    return (0);
}


static int AdcStm32f103_GetValue(void *obj, unsigned channel) {
    stm32f103_adc_t *_this = obj;

    return (_this->values[_this->mapValues[channel]]);
}