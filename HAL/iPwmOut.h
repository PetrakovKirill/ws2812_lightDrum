#ifndef _PWM_OUTPUT_SIGNAL_INTERFACE_
#define _PWM_OUTPUT_SIGNAL_INTERFACE_

typedef struct vTable_iVrtPort {
    int  (*SetDuty)(void *obj, unsigned duty);
}vTable_iPwmOut;


typedef struct iVrtPort {
    vTable_iPwmOut *vTable;
}iPwmOut;


#define PwmOut_SetDuty(_obj, _duty)     ( ((iPwmOut *)_obj)->vTable->SetDuty(_obj, _duty) )


#endif /* _PWM_OUTPUT_SIGNAL_INTERFACE_ */