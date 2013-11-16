/*
 * platform.c
 */

#ifndef PLATFORM_C_
#define PLATFORM_C_

#if defined(PLATFORM_PIC)

// PIC
#define __18F97J60
#include "Include/HardwareProfile.h"
#include "Include/LCDBlocking.h"
#include "Include/TCPIP_Stack/Delay.h"

#include "pic16/button.c"
#include "pic16/display.c"
#include "pic16/led.c"

#elif defined(PLATFORM_WIN32)

// Windows
#include <string.h>
#include <stdlib.h>

#else
#error Unknown platform.
#endif

#endif /* PLATFORM_C_ */
