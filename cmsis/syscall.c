#include <sys/types.h>

#define Get_Stack(__gs)   __asm__( "mov %0, sp" : "=r"(__gs) : )

extern unsigned long _start_user_heap;
static unsigned int  _heapUsedCount = 0;

caddr_t _sbrk(int incr) {
    /* Текущий указатель кучи */
	static char *heapEnd = 0;
	char *prevHeapEnd;

    if (heapEnd == 0) {
        heapEnd = (char *)&_start_user_heap;
    }

    /* Вычисляем необходимый остаток для выравнивания */
    int align = (incr % 4) ? (4 - (incr % 4)) : (0);
    unsigned currentStack;

	prevHeapEnd = heapEnd;
    heapEnd += incr + align;
    _heapUsedCount += incr + align;

    Get_Stack(currentStack);

    /* Проверка коллизии кучи со стеком */
	if (heapEnd > (char *)currentStack) {
	    heapEnd = prevHeapEnd;
		return (caddr_t) NULL; 
	}
	return (caddr_t) prevHeapEnd;
}


unsigned int _HeapUsedCount(void) {
    return (_heapUsedCount);
}