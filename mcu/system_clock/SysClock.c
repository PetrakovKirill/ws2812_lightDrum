#include "stm32f10x.h"
#include "SysClock.h"



void SystemClock_24MHz_HSE(void) {
	RCC->CR |= RCC_CR_HSEON;
	while( !(RCC->CR & (RCC_CR_HSERDY)) );

	RCC->CFGR &= ~(0x0F << 18);
	RCC->CFGR |= (0x01 << 18) | 
				 RCC_CFGR_PLLSRC;

	// RCC->CFGR |=  (0x07 << 18) | 	/* HSE x9 = 72 						*/
	// 			  (0x01 << 22) | 	/* USB must be 48MHz, dev = 1.5 	*/
	// 			  (0x02 << 14) | 	/* ADC must be 14 MHz Max, div = 6 	*/
	// 			  (0x04 <<  8) |	/* APB1 must be 36 MHz max, div = 2 */
	// 			 RCC_CFGR_PLLSRC;


	RCC->CR |= RCC_CR_PLLON;
	while( !(RCC->CR & (RCC_CR_PLLRDY)) );

	RCC->CFGR &= ~(0x03);
	RCC->CFGR |= RCC_CFGR_SW_PLL;
}