#include <stdlib.h>
#include <stdint.h>
#include "stm32f10x.h"
#include "hw_spi.h"
#include "gpio.h"
#include "bool_const.h"
#include "observer.h"

/* SPI 1 */
#define SPI1_NSS     (4) // PA4 
#define SPI1_SCK     (5) // PA5 
#define SPI1_MISO    (6) // PA6 
#define SPI1_MOSI    (7) // PA7 

/* SPI 2 */
#define SPI2_NSS     (12) // PB12 
#define SPI2_SCK     (13) // PB13 
#define SPI2_MISO    (14) // PB14 
#define SPI2_MOSI    (15) // PB15 

/* Структура таблицы аппаратных зависимостей */
typedef struct hardwareDependens_t {
    SPI_TypeDef         *spiBase;
    DMA_Channel_TypeDef *dmaBaseRX, *dmaBaseTX;
    DMA_TypeDef         *dmaBase;
    GPIO_TypeDef        *gpioBase;

    uint32_t  dmaFlags;
    uint8_t   dmaIrqRX, dmaIrqTX, 
              pinNSS, pinSCK, pinMISO, pinMOSI;

    volatile uint32_t *rccIFaceEnable, *rccPinPortEnable;
    uint32_t           bitIFaceEnable,  bitPinPortEnable;
} hardwareDependens_t;

/* Таблица аппаратных зависимостей */
static hardwareDependens_t spi_stm32f103c8t6[] = {
    {       /* SPI1 */
    .spiBase          = SPI1,               .dmaBaseRX        = DMA1_Channel2, 
    .dmaBaseTX        = DMA1_Channel3,      .dmaBase          = DMA1,
    .gpioBase         = GPIOA,              
    .dmaIrqRX         = DMA1_Channel2_IRQn, .dmaIrqTX         = DMA1_Channel3_IRQn,
    .pinNSS           = SPI1_NSS,           .pinSCK           = SPI1_SCK, 
    .pinMISO          = SPI1_MISO,          .pinMOSI          = SPI1_MOSI,
    .rccIFaceEnable   = &RCC->APB2ENR,      .bitIFaceEnable   = RCC_APB2ENR_SPI1EN,
    .rccPinPortEnable = &RCC->APB2ENR,      .bitPinPortEnable = RCC_APB2ENR_IOPAEN,
    .dmaFlags = (DMA_ISR_TCIF3 | DMA_ISR_GIF3 | DMA_ISR_HTIF3 | DMA_ISR_TEIF3)
    }, {    /* SPI2 */
    .spiBase          = SPI2,               .dmaBaseRX        = DMA1_Channel4, 
    .dmaBaseTX        = DMA1_Channel5,      .dmaBase          = DMA1,
    .gpioBase         = GPIOB,
    .dmaIrqRX         = DMA1_Channel4_IRQn, .dmaIrqTX         = DMA1_Channel5_IRQn,
    .pinNSS           = SPI2_NSS,           .pinSCK           = SPI2_SCK, 
    .pinMISO          = SPI2_MISO,          .pinMOSI          = SPI2_MOSI,
    .rccIFaceEnable   = &RCC->APB1ENR,      .bitIFaceEnable   = RCC_APB1ENR_SPI2EN,
    .rccPinPortEnable = &RCC->APB2ENR,      .bitPinPortEnable = RCC_APB2ENR_IOPAEN,
    .dmaFlags = (DMA_ISR_TCIF5 | DMA_ISR_GIF5 | DMA_ISR_HTIF5 | DMA_ISR_TEIF5)
    }
};


typedef struct hardwareSpi {
    hardwareDependens_t *hw;
    uint32_t     config[2]; /* Биты соотвествующие регистру CR1 и CR2 */

    observer_t   eventTransferComplite;
    pin_t        nssPin;
} hardwareSpi_t;

/* 
 * Указатель хранит текущий объект, 
 * нужен в обработчике прерывания ДМА
 */
static hardwareSpi_t *currentObj = NULL;


static void HardwareSpi_TransferCompiteHandler(void *arg) {
    hardwareSpi_t *_this = arg;
    Pin_SetState(&_this->nssPin, PIN_STATE_HIGH);
}


void * HardwareSpi_Constructor(unsigned spi) {

    if (spi >= ( sizeof(spi_stm32f103c8t6) / sizeof(hardwareDependens_t) )) {
        return NULL;
    }

    hardwareSpi_t *_this = calloc(1, sizeof(hardwareSpi_t));
    pin_t pins;

    if(_this == NULL) {
        return (NULL);
    }

    _this->hw = &spi_stm32f103c8t6[spi];
    *_this->hw->rccIFaceEnable   |= _this->hw->bitIFaceEnable;
    *_this->hw->rccPinPortEnable |= _this->hw->bitPinPortEnable;

    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    // Pin_Init(&miso, GPIOB, SPI2_MISO, GPIO_INP_FLOAT);
    Pin_Init(&pins, _this->hw->gpioBase, _this->hw->pinMOSI, GPIO_ALTOUT_PP_50MHz);
    Pin_Init(&pins, _this->hw->gpioBase, _this->hw->pinSCK , GPIO_ALTOUT_PP_50MHz);
    Pin_Init(&_this->nssPin, _this->hw->gpioBase, _this->hw->pinNSS,  GPIO_OUT_PP_50MHz);

    Pin_SetState(&_this->nssPin, PIN_STATE_HIGH);

    _this->config[0] = SPI_CR1_SSM | SPI_CR1_BR_1 | SPI_CR1_SSI | 
                       SPI_CR1_SPE | SPI_CR1_CPHA | SPI_CR1_MSTR;

    _this->config[1] = SPI_CR2_SSOE | SPI_CR2_TXDMAEN;

    Observer_Init(&_this->eventTransferComplite);   

    /* DMA */
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    /* Set the peripheral register address */
    _this->hw->dmaBaseTX->CPAR = (uint32_t)&_this->hw->spiBase->DR;

    /* DMA1, SPI1_RX = ch2, SPI1_TX = ch3 
     * memory size = 0-bits 
     * Peripheral size = 8-bits
     * Memory increment mode enabled
     * Peripheral increment mode disabled
     * Circular mode disabled
     * Data transfer direction: read from memory
     * Transfer error interrupt enable
     * Half transfer interrupt disable
     * Transfer complete interrupt enabled
     * Chanel enabled */
    _this->hw->dmaBaseTX->CCR = DMA_CCR1_MINC |
                                DMA_CCR1_DIR  |
                                DMA_CCR1_TEIE |
                                DMA_CCR1_TCIE ;
    
    NVIC_EnableIRQ(_this->hw->dmaIrqTX);

    return (_this);
}


void HardwareSpi_Transfer(void *obj, void *output, unsigned length) {
    if ((obj == NULL) || (currentObj)) {
        return;
    }

    currentObj = obj;

    while(currentObj->hw->dmaBaseTX->CCR & DMA_CCR1_EN);

    currentObj->hw->spiBase->CR2 = currentObj->config[1];
    currentObj->hw->spiBase->CR1 = currentObj->config[0];

    Pin_SetState(&currentObj->nssPin, PIN_STATE_LOW);

    currentObj->hw->dmaBaseTX->CNDTR = length;
    currentObj->hw->dmaBaseTX->CMAR  = (uint32_t)output;
    currentObj->hw->dmaBaseTX->CCR  |= DMA_CCR1_EN;
}


void DMA1_Channel5_IRQHandler(void) {
    // dmaStatusReg = DMA1->ISR;
    DMA1->IFCR |= (DMA_ISR_TCIF5 | DMA_ISR_GIF5 | 
                   DMA_ISR_HTIF5 | DMA_ISR_TEIF5); /* Clear flags */
    currentObj->hw->dmaBaseTX->CCR &= ~DMA_CCR1_EN;

    HardwareSpi_TransferCompiteHandler(currentObj);

    /* Выполнение пользовательских обработчиков */
    Observer_Notice(&currentObj->eventTransferComplite);

    currentObj = NULL;
}


void * HardwareSpi_GetObserver(void *obj) {
    hardwareSpi_t *_this = obj;
    return ((void *)&_this->eventTransferComplite);
}