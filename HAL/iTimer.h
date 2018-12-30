#ifndef _TIMER_INTERFACE_
#define _TIMER_INTERFACE_



typedef struct vTable_iTimer {
    int (*Init)(void *obj, unsigned mode);
    int (*AttachObserverOverflow)(void *obj, unsigned channel);
    // int (*GetValue)(void *obj, unsigned channel);
}vTable_iTimer;



typedef struct iTimer {
    vTable_iTimer *vTable;
} iTimer;



#define Timer_Init(_obj, _mode)                         ( ((iTimer *)_obj)->vTable->Init(_obj, _mode) )
#define Timer_AttachObserverOverflow(_obj, _channel)    ( ((iTimer *)_obj)->vTable->AttachObserverOverflow(_obj, _channel) )
// #define Timer_GetValue(_obj, _channel)            ( ((iTimer *)_obj)->vTable->GetValue(_obj, _channel) )


#endif /* _TIMER_INTERFACE_ */
