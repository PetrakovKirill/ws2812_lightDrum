#ifndef _INTERFACE_VIRTUAL_PORT_
#define _INTERFACE_VIRTUAL_PORT_

typedef struct vTable_iVrtPort {
    int  (*GetStatePin) (void *obj, unsigned pin);
    int  (*SetStatePin) (void *obj, unsigned pin, unsigned state);
    int  (*GetStatePort)(void *obj);
    int  (*SetStatePort)(void *obj,unsigned value);
}vTable_iVrtPort;


typedef struct iVrtPort {
    vTable_iVrtPort *vTable;
}iVrtPort;


// #define VrtPort_Destructor(_obj)                    ( ((iVrtPort *)_obj)->vTable->Destructor(_obj) )
#define VrtPort_GetStatePin(_obj, _pin)             ( ((iVrtPort *)_obj)->vTable->GetStatePin(_obj, _pin) )
#define VrtPort_SetStatePin(_obj, _pin, _state)     ( ((iVrtPort *)_obj)->vTable->SetStatePin(_obj, _pin, _state) )
#define VrtPort_GetStatePort(_obj)                  ( ((iVrtPort *)_obj)->vTable->GetStatePort(_obj) )
#define VrtPort_SetStatePort(_obj, _value)          ( ((iVrtPort *)_obj)->vTable->SetStatePort(_obj, _value) )
// #define VrtPort_ConfigPin(_obj, _pin, _mode)        ( ((iVrtPort *)_obj)->vTable->ConfigPin(_obj, _pin, _mode) )


#endif /* _INTERFACE_VIRTUAL_PORT_ */