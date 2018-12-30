#ifndef _HARDWARE_SPI_
#define _HARDWARE_SPI_

typedef struct hardwareSpi hardwareSpi_t;

void * HardwareSpi_Constructor(unsigned  spi);
void   HardwareSpi_Transfer(void *obj, void *output, unsigned length);
void * HardwareSpi_GetObserver(void *obj);

#endif /* _HARDWARE_SPI_ */