#pragma once

void functionCalledWhenExceptionIsThrowed(const char* text, unsigned int size);

#include <string.h>

#ifndef THROW_invalid_argument
#define THROW_invalid_argument(X) while(true)functionCalledWhenExceptionIsThrowed(X,strlen(X))
#endif
#ifndef THROW_out_of_range
#define THROW_out_of_range(X) while(true)functionCalledWhenExceptionIsThrowed(X,strlen(X))
#endif

