#ifndef _ADC_PERIPH_
#define _ADC_PERIPH_

#include <stdint.h>
#include "iAdc.h"

#define ADC_CHANNELS_MAX    (16)

#define ADC1_CH1            (0x01)
#define ADC1_CH2            (0x02)
#define ADC1_CH3            (0x03)
#define ADC1_CH4            (0x04)
#define ADC1_CH5            (0x05)
#define ADC1_CH6            (0x06)
#define ADC1_CH7            (0x07)
#define ADC1_CH8            (0x08)
#define ADC1_CH9            (0x09)
#define ADC1_CH10           (0x0A)
#define ADC1_CH12           (0x0B)
#define ADC1_CH13           (0x0C)
#define ADC1_CH14           (0x0D)
#define ADC1_CH15           (0x0E)
#define ADC1_CH16           (0x0F)

typedef struct stm32f103_adc_t {
    /* Наследование от iAdc - реализация интерфейса */
    iAdc                 iFace;

    ADC_TypeDef         *adc;
    DMA_Channel_TypeDef *dma;

    uint8_t              activeChannels;
    uint8_t              mapValues[ADC_CHANNELS_MAX];
    volatile uint16_t             values[ADC_CHANNELS_MAX];
}stm32f103_adc_t;

void AdcStm32f103_Construct(void *obj, ADC_TypeDef *adcPeriph, DMA_Channel_TypeDef *dmaPeriph);


#endif /* _ADC_PERIPH_ */