#include <stddef.h>
#include <stdlib.h>
#include "observer.h"


void Observer_Init(observer_t * obj) {
    observer_t *_this = obj;
    _this->eventList = NULL;
}


void Observer_Notice(void * obj) {
    observer_t *_this = obj;
    eventList_t *eventList = _this->eventList;
    void * arg;

    /* Выполнение обработчиков, подписанных на событие */
    while (eventList) {
        arg = eventList->event.arg;
        eventList->event.EventHendler(arg);

        eventList = eventList->prev;
    }
}


void Observer_AttacheEvent(void *obj, void (*Heandler)(void *), void *arg) {
    observer_t *_this = obj;

    /* Новый элемент списка */
    eventList_t * newEvent = malloc(sizeof(eventList_t));
    if (newEvent == NULL) {
        return;
    } 

    newEvent->event.EventHendler = Heandler;
    newEvent->event.arg = arg;
    newEvent->prev = _this->eventList;

    _this->eventList = newEvent;
}