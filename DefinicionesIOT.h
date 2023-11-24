/*
INNOVA DOMOTICS
Curso Arduino desde Cero PRIMER MODULO 
Youtube: https://www.youtube.com/playlist?list=PLZHVfZzF2DYJeLXXxz6YtpBj4u7FoGPWN
Plataforma de Curso de Innova Domotics:https://cursos.innovadomotics.com/cursos/arduino-desde-cero-primer-modulo/
Curso Arduino desde Cero SEGUNDO MODULO 
Plataforma de Curso de Innova Domotics:https://cursos.innovadomotics.com/cursos/arduino-desde-cero-segundo-modulo/
*/

#ifndef _DEFINICIONESIOT_h
#define _DEFINICIONESIOT_h

#define CANAL_CYN_00   0
#define CANAL_CYN_01   1
#define CANAL_CYN_02   2
#define CANAL_CYN_03   3
#define CANAL_CYN_04   4
#define CANAL_CYN_05   5
#define CANAL_CYN_06   6
#define CANAL_CYN_07   7
#define CANAL_CYN_08   8
#define CANAL_CYN_09   9
#define CANAL_CYN_10   10
#define CANAL_CYN_11   11
#define CANAL_CYN_12   12
#define CANAL_CYN_13   13
#define CANAL_CYN_14   14
#define CANAL_CYN_15   15
#define CANAL_CYN_16   16
#define CANAL_CYN_17   17
#define CANAL_CYN_18   18
#define CANAL_CYN_19   19
#define CANAL_CYN_20   20
#define CANAL_CYN_21   21
#define CANAL_CYN_22   22
#define CANAL_CYN_23   23
#define CANAL_CYN_24   24
#define CANAL_CYN_25   25
#define CANAL_CYN_26   26
#define CANAL_CYN_27   27
#define CANAL_CYN_28   28
#define CANAL_CYN_29   29
#define CANAL_CYN_30   30
#define CANAL_CYN_31   31

#ifdef IOT_MOD_WIZNET_COMPILE
#include <CayenneMQTTEthernet.h>
#endif

#ifdef IOT_MOD_8266_COMPILE
#include <CayenneMQTTESP8266.h>
#define D0   16 //GPIO16 - WAKE UP
#define D1   5  //GPIO5
#define D2   4  //GPIO4
#define D3   0  //GPIO0
#define D4   2  //GPIO2 - TXD1
#define D5   14 //GPIO14 - HSCLK
#define D6   12 //GPIO12 - HMISO
#define D7   13 //GPIO13 - HMOSI - RXD2
#define D8   15 //GPIO15 - HCS   - TXD2
#endif

#ifdef IOT_MOD_ESP32_COMPILE
#include <CayenneMQTTESP32.h>
#define EspGPIO_02   2 
#define EspGPIO_04   4 
#define EspGPIO_05   5 
#define EspGPIO_13   13 
#define EspGPIO_14   14 
#define EspGPIO_15   15 
#define EspGPIO_16   16 
#define EspGPIO_17   17 
#define EspGPIO_18   18 
#define EspGPIO_19   19 
#define EspGPIO_21   21 
#define EspGPIO_22   22 
#define EspGPIO_23   23 
#define EspGPIO_25   25 
#define EspGPIO_26   26 
#define EspGPIO_27   27
#define EspGPIO_32   32 
#define EspGPIO_33   33
#endif
#endif
