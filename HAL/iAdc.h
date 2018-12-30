#ifndef _ADC_INTERFACE_
#define _ADC_INTERFACE_



typedef struct vTable_iAdc {
    int (*Init)(void *obj);
    int (*AttachChannel)(void *obj, unsigned channel);
    int (*GetValue)(void *obj, unsigned channel);
}vTable_iAdc;



typedef struct iAdc {
    vTable_iAdc *vTable;
} iAdc;



#define Adc_Init(_obj)                          ( ((iAdc *)_obj)->vTable->Init(_obj) )
#define Adc_AttachChannel(_obj, _channel)       ( ((iAdc *)_obj)->vTable->AttachChannel(_obj, _channel) )
#define Adc_GetValue(_obj, _channel)            ( ((iAdc *)_obj)->vTable->GetValue(_obj, _channel) )


#endif /* _ADC_INTERFACE_ */