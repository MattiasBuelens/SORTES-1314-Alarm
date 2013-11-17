# SDCC paths
# A) Linux
# LIB = /usr/local/lib
# INCLUDE = /usr/local/share/sdcc/include
# B) Windows
#    Do not use spaces in these paths! (set up symbolic links if needed)
LIB = C:\SDCC\lib
INCLUDE = C:\SDCC\include

AS = gpasm
CC = sdcc
CFLAGS= -c -mpic16 -p18f97j60  -o$@ 
LD = sdcc
LDFLAGS= -mpic16 -p18f97j60 -L/pic16 -llibio18f97j60.lib \
         -llibdev18f97j60.lib -llibc18f.lib
AR = ar
RM = rm

OBJECTS= Objects/LCDBlocking.o

SDCC_HEADERS=$(INCLUDE)/string.h \
   $(INCLUDE)/stdlib.h \
   $(INCLUDE)/stdio.h \
   $(INCLUDE)/stddef.h \
   $(INCLUDE)/stdarg.h 

SDCC_PIC16_HEADERS=$(INCLUDE)/pic16/pic18f97j60.h

TCPIP_HEADERS=   Include/TCPIP_Stack/ETH97J60.h \
   Include/TCPIP_Stack/LCDBlocking.h 

APP_HEADERS=Include/GenericTypeDefs.h \
   Include/Compiler.h \
   Include/HardwareProfile.h 

pic16 : $(OBJECTS) Objects/platform_pic16.o Objects/alarm.o
	$(LD) $(LDFLAGS) Objects/alarm.o $(OBJECTS) Objects/platform_pic16.o

win32 : $(OBJECTS) Objects/platform_win32.o Objects/alarm.o
	$(LD) $(LDFLAGS) Objects/alarm.o $(OBJECTS) Objects/platform_win32.o

Objects/alarm.o : alarm.c $(SDCC_HEADERS) $(SDCC_PIC16_HEADERS) \
   $(APP_HEADERS) $(TCPIP_HEADERS)
	$(CC) $(CFLAGS) alarm.c

Objects/platform_pic16.o : platform_pic16.c $(SDCC_HEADERS) $(SDCC_PIC16_HEADERS) \
   $(APP_HEADERS) $(TCPIP_HEADERS)
	$(CC) $(CFLAGS) platform_pic16.c

Objects/platform_win32.o : platform_win32.c $(SDCC_HEADERS) $(SDCC_PIC16_HEADERS) \
   $(APP_HEADERS) $(TCPIP_HEADERS)
	$(CC) $(CFLAGS) platform_win32.c

Objects/LCDBlocking.o : TCPIP_Stack/LCDBlocking.c $(SDCC_HEADERS)  \
   $(SDCC_PIC16_HEADERS) $(APP_HEADERS) $(TCPIP_HEADERS)
	$(CC) -c -mpic16 -p18f97j60  -o"Objects/LCDBlocking.o" \
              -L$(LIB)/pic16  TCPIP_Stack/LCDBlocking.c

Objects/Tick.o : TCPIP_Stack/Tick.c  $(SDCC_HEADERS)  \
   $(SDCC_PIC16_HEADERS) $(APP_HEADERS) $(TCPIP_HEADERS)
	$(CC) -c -mpic16 -p18f97j60  -o"Objects/Tick.o" \
              -L$(LIB)/pic16  TCPIP_Stack/Tick.c

clean : 
	$(RM) -rf *.o *.hex *.cod *.lst

