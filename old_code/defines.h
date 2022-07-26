#ifndef DEFINES_H
#define DEFINES_H

//#include "string.h"
#include "stdint.h"
//#include "stdio.h"
//#include "stdlib.h"
#include "math.h"
#include "periph_use.h"

//#define REF_VOLTAGE 3.3

struct MainFlags
{
	bool need_update:1;
	bool rot:1;
	bool pressed:1;
};


#endif
