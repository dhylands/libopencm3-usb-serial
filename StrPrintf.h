/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <stdarg.h>

typedef int (*StrXPrintfFunc)(void *outParm, int c);

int
StrPrintf(char* outStr, int maxLen, const char* fmt, ...);

int
StrXPrintf(StrXPrintfFunc outFunc, void* outParm, const char* fmt, ...);

int
vStrPrintf(char* outStr, int maxLen, const char* fmt, va_list args);

int
vStrXPrintf(StrXPrintfFunc outFunc,void* outParm, const char* fmt,
            va_list args);
