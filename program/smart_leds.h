#ifndef _SMART_LEDS_
#define _SMART_LEDS_


typedef struct vTable_iSmartLeds {
    void (*Show)(void *obj);
    void (*SetColorPixel)(void *obj, unsigned numPix, unsigned colorPix);    
}vTable_iSmartLeds;


typedef struct iSmartLeds {
    vTable_iSmartLeds *vTable;
}iSmartLeds;


#define SmartLeds_Show(_obj)                                ( ((iSmartLeds *)_obj)->vTable->Show(_obj) )
#define SmartLeds_SetColorPixel(_obj, _numPix, _colorPix)   ( ((iSmartLeds *)_obj)->vTable->SetColorPixel(_obj,_numPix, _colorPix) )


#endif/* _SMART_LEDS_ */