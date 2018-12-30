#include "sprintf.h"
#include <stdarg.h>
#include <stdint.h>

typedef unsigned int paddr_t;

#define FORMAT_MARKER   '%'
const static char	c[] = "0123456789abcdef";

int Sprintf(char *outStr, const char *fmtStr, ...) {
    char 			*vs;
	uint64_t		num;
	unsigned		radix;
	int				dig;
	char			buf[64];

    /* курсор */
    int fCur, oCur, mFlag = 0;

    va_list args;
    va_start(args, fmtStr);

    for(fCur = oCur = 0; fmtStr[fCur]; ++fCur, ++oCur) {
		if(fmtStr[fCur] != FORMAT_MARKER) {
			outStr[oCur] = fmtStr[fCur];
			continue;
		}
		radix = dig = 0;
		switch(fmtStr[++fCur]) {
		case 'b':
			num = va_arg(args, unsigned);
			dig = sizeof(uint8_t)*2;
			radix = 2;
			break;
		case 'w':
			num = va_arg(args, unsigned);
			dig = sizeof(uint16_t)*2;
			radix = 16;
			break;
		case 'P':
			num = va_arg(args, paddr_t);
			dig = sizeof(paddr_t)*2;
			radix = 16;
			break;
		case 'x':
		case 'X':
		case 'l':
			num = va_arg(args, unsigned long);
			// dig = sizeof(unsigned long)*2;
			radix = 16;
			break;
		case 'L':
			num = va_arg(args, uint64_t);
            // dig = sizeof(uint64_t)*2;
			radix = 16;
			break;
		case 'd':
        	num = va_arg(args, int);
            if (*(int64_t *)&num < 0) {
                mFlag = 1;
                *(int64_t *)&num *= -1;
            }
			radix = 10;
			break;
		case 'u':
			num = va_arg(args, unsigned);
			radix = 10;
			break;
		case 's':
			vs = va_arg(args, char *);
			while(*vs) {
				outStr[oCur++] = *vs++;
			}
			continue;
		default:
			outStr[oCur] = fmtStr[fCur];
			continue;
		}
		vs = &buf[sizeof(buf) - 1];

		// *--vs = '\0';
		do {
			*--vs = c[num % radix];
			num /= radix;
		} while(num);

        if (mFlag) {
            *--vs = '-';
        }

		for(dig -= &buf[sizeof(buf)-1] - vs; dig > 0; --dig) {
			outStr[oCur++] = '0';
		}

		while(*vs) {
			outStr[oCur++] = *vs++;
		}
	}
    va_end(args);
    return oCur;
}