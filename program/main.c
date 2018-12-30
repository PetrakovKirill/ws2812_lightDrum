#include "stm32f10x.h"
#include "SysClock.h"
#include "delay.h"
#include "ws2812.h"
#include "hw_spi.h"
#include "observer.h"
#include "gpio.h"
#include "adc.h"

#define NLED		(30)
#define TRASHOLD	(50)

#define POWER_MAX	(0x505050)

#define ARRAY_ELEM(arr)		( sizeof(arr) / sizeof(arr[0]) ) 



/* Прототипы для функций вычисления логарифма */
float FastLog2( float x );
float FastLog(float x, float a);


/* Таблица градиента */
/* static const unsigned colorTable[NLED] = {
	0xEC1300, 0xD92600, 0xC63900, 0xB34C00, 0xA05F00,
	0x8D7200, 0x7A8500, 0x679800, 0x54AB00, 0x41BE00,
	0x2ED100, 0x1BE400, 0x08f700, 0x00FF00
}; */


static unsigned RgbToGrb(unsigned color) {
	typedef struct rgb_t { uint8_t r, g, b; } rgb_t;
	typedef struct grb_t { uint8_t b, r, g; } grb_t;

	rgb_t *rgb = (rgb_t *)&color;
	grb_t grb;
	grb.g = rgb->g;
	grb.r = rgb->r;
	grb.b = rgb->b;

	return (*(unsigned *)&grb);
}



/* Инициализация Пинов АЦП */
static void AdcPinInit(void) {
    pin_t adc;
    Pin_Init(&adc, GPIOA, 3, GPIO_INP_ANALOG);
	Pin_Init(&adc, GPIOA, 1, GPIO_INP_ANALOG);
}



unsigned WhiteVariableBright(unsigned wVolume, int index) {
	unsigned result;
	result = (0x0A0A0A * wVolume);
	if (result > POWER_MAX) {
		wVolume = POWER_MAX;
	}
	return (result & 0xFFFFFF);
}



unsigned WhiteFillFullPower(unsigned wVolume, int index) {
	unsigned result;
	result = (index < wVolume) ? (POWER_MAX) : (0x00);
	return (result);
}



unsigned WhiteFillLowPower(unsigned wVolume, int index) {
	unsigned result;
	result = (index < wVolume) ? (POWER_MAX) : (0x00);
	// result = (index < wVolume) ? (0x101010) : (0x00);
	return (result);
}

unsigned RainbowFill(unsigned wVolume, int index) {
	unsigned result;
	index  = index * 24 / NLED;
	result = (index < wVolume) ? RgbToGrb(0xFF << (index) ) : (0x00);
	return (result);
}


typedef unsigned (*modeFunc_t)(unsigned, int);


/* Глобальные переменные */
static iSmartLeds *leds;
static pin_t p13, button;
static modeFunc_t Mode[] = {
	WhiteFillLowPower,
	// WhiteFillFullPower,
	WhiteVariableBright,
	RainbowFill
};



int main(void) {
	float log_a = FastLog2(2048.0f);
	int i = 0, wAdcValue = 0, wVolumeLast = 0 , wVolume = 0, count = 70;

	SystemClock_24MHz_HSE();

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPBEN |
					RCC_APB2ENR_ADC1EN | RCC_APB2ENR_AFIOEN;
    RCC->AHBENR  |= RCC_AHBENR_DMA1EN;


	Pin_Init(&p13, GPIOC, 13, GPIO_OUT_PP_50MHz);
	Pin_Init(&button, GPIOA, 2,  GPIO_INP_PUPD);
	Pin_PullUp(&button);

	hardwareSpi_t *spi = HardwareSpi_Constructor(1);
	leds = WS2812_Constructor(NLED, spi);

	AdcPinInit();

	stm32f103_adc_t adc = { 0 };
	AdcStm32f103_Construct(&adc, ADC1, DMA1_Channel1);
	Adc_AttachChannel(&adc,ADC1_CH3);
	Adc_AttachChannel(&adc,ADC1_CH1);
	Adc_Init(&adc);


	/* Очистка состояния ленты */
	for (i = 0;i < NLED; ++i) {
		SmartLeds_SetColorPixel(leds, i, 0x00);
	}
	SmartLeds_Show(leds);

	const unsigned ZERO_POINT = 1;
	const unsigned NLED_DIV   = NLED / ZERO_POINT;

	int j, indexMode = 0, buttonPressFlag = 0;
	unsigned currentColor, pix;

	static unsigned buttonCount = 150;

	while(1) {
		wAdcValue = Adc_GetValue(&adc, ADC1_CH3) - 2048 - TRASHOLD;

		/* Подавление шумов */
		if (wAdcValue < 1) {
			wAdcValue = 1;
		} else {
			wVolume += TRASHOLD;
		}

		wVolume = (unsigned)(FastLog((wAdcValue), log_a) * (float)NLED_DIV); 

		if (wVolume > NLED_DIV) {
			wVolume = NLED_DIV;
		}

		if (wVolume < wVolumeLast) {
			wVolume = wVolumeLast;
		} else {
			wVolumeLast = wVolume;
		}
		
		for(i = 0 ; i < NLED_DIV; ++i) {
			for(j = 0; j < ZERO_POINT; ++j) {
				if (j % ZERO_POINT) {
					pix = ((j+1) * (NLED_DIV - 1)) - i;
				} else {
					pix = i + (j * NLED_DIV);
				}
				// currentColor = (i < wVolume) ? colorTable[i * ZERO_POINT] : (0x00);
				currentColor = Mode[indexMode](wVolume, i);
				SmartLeds_SetColorPixel(leds, pix, currentColor);
			}
		}

		SmartLeds_Show(leds);
		Delay_ms(1);

		if (!(count--)) {
			count =  Adc_GetValue(&adc, ADC1_CH1) >> 5;
			wVolumeLast--;

			GPIOC->ODR ^=(1<<13);
		}
		
		if (!(buttonCount--)) {
			buttonCount = 150;
			if ( (Pin_GetState(&button) == PIN_STATE_LOW) && 
					(buttonPressFlag == 0) ) {
				
				indexMode = (indexMode + 1) % ARRAY_ELEM(Mode);
				buttonPressFlag = 1;
			} else {
				buttonPressFlag = 0;
			}
		}
	}

}