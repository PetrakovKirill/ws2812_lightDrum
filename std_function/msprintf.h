#include <stdint.h>
#include <stdarg.h>

#ifndef MSPRINTF_H
#define MSPRINTF_H

typedef struct divmod10_t
{
    uint32_t quot;
    uint8_t rem;
}divmod10_t;

divmod10_t divmodu10(uint32_t n);
char * intToStroka(int32_t value, char *buffer, int8_t por);


void msprintf(char *textBuffer, const char *constText, ...);

#endif // MSPRINTF_H
