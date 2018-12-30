#include "msprintf.h"
#include <stdint.h>


void msprintf(char *textBuffer, const char *constText, ...)
{
	// iEndTextBuffer - счетчик конца буфера
	uint32_t iEndTextBuffer = 256;
	// счетчик используемых аргументов
	va_list pArg;
	va_start(pArg, constText);
	while(*constText && (iEndTextBuffer--))
	{
		if(*constText != '%') {*textBuffer++ = *constText++;}
		else
			{
				// Если встречаем символ d
				if(*(constText+1) == 'd')
				{	
					// Создаем для хранения строки полученную из числа
					char numBuffer[12] = {0};
					// создаем указатель на строку с числом очередного аргумента
					char* TextBuff = intToStroka(va_arg(pArg, int32_t), numBuffer, 0);
					// выводим Число преобразованное в строку
					while(*TextBuff && (iEndTextBuffer--))
					{
						*textBuffer++ = *TextBuff++;
					}
					// пропускаем 2 зарезервированных символа
					constText += 2;
				}
				else if (*(constText+1) == 'f')
				{
					int8_t por = 5, otsup = 0;
					if (((uint8_t)(*(constText+2)) >= 0x30) && ((uint8_t)(*(constText+2)) <= (0x30 + 0x9)))
						{
							otsup = 1;
							por = (int8_t)(*(constText+2) - 0x30);
						}

					// Создаем для хранения строки полученную из числа
					char numBuffer[12] = {0};
					double afterPoint = 1.0;
					for(uint8_t i = 0; i < por; ++i) {afterPoint *= 10.0;}
					// создаем указатель на строку с числом очередного аргумента
					afterPoint *= va_arg(pArg, double);
					char* TextBuff = intToStroka(((int32_t)afterPoint), numBuffer, por);
					// выводим Число преобразованное в строку
					while(*TextBuff && (iEndTextBuffer--))
					{
						*textBuffer++ = *TextBuff++;
					}
					// пропускаем 2 зарезервированных символа
					constText += (2 + otsup);
				}
				else
					// если нет спец символа, то пропускаем 1 символ
					{constText++;}
			}
	}
	va_end(pArg);
}


inline divmod10_t divmodu10(uint32_t n)
{
    divmod10_t res;
// умножаем на 0.8
    res.quot = n >> 1;
    res.quot += res.quot >> 1;
    res.quot += res.quot >> 4;
    res.quot += res.quot >> 8;
    res.quot += res.quot >> 16;
    uint32_t qq = res.quot;
// делим на 8
    res.quot >>= 3;
// вычисляем остаток
    res.rem = (uint8_t)(n - ((res.quot << 1) + (qq & ~7ul)));
// корректируем остаток и частное
    if(res.rem > 9)
    {
        res.rem -= 10;
        res.quot++;
    }
    return res;
}


char * intToStroka(int32_t value, char *buffer, int8_t por)
{

	uint8_t flagMinus = 0;
    buffer += 12;
    *--buffer = 0;
    // проверка на отрицательное число
    if(value < 0)
    {
    	flagMinus = 1;
    	value = (-1)*value;
    }
    if(por == 0) {por = -1;}
    do
    {
        divmod10_t res = divmodu10(value);
        if(por-- == 0) {*--buffer = '.';}
        *--buffer = res.rem + '0';
        value = res.quot;
    }
    while (value != 0);
    if(por > 0)
    {
	    do
	    {
	    	if(por-- == 0) {*--buffer = '.';}
	    	*--buffer = '0';
	    }
	    while ((por > 0) && (por != 0));
	}
    if(flagMinus) {*--buffer = '-';}
    return buffer;
}