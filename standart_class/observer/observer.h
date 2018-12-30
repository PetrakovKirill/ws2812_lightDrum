#ifndef _OBSERVER_
#define _OBSERVER_


typedef struct event {
    void (*EventHendler)(void *arg);
    void * arg;
} event_t;

typedef struct eventList {
    struct eventList *prev;
    event_t           event;
} eventList_t;


typedef struct observer {
    eventList_t *eventList;
} observer_t;

void Observer_Init(observer_t * obj);
void Observer_Notice(void * obj);
void Observer_AttacheEvent(void *obj, void (*Heandler)(void *), void *arg);

#endif /* _OBSERVER_ */