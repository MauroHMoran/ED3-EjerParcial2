/*Programar el microcontrolador LPC1769 para que mediante su ADC digitalice
dos señales analógicas cuyos anchos de bandas son de 10 Khz cada una. Los canales
utilizados deben ser el 2 y el 4 y los datos deben ser guardados en dos regiones de
memorias distintas que permitan contar con los últimos 20 datos de cada canal. Suponer
una frecuencia de core cclk de 100 Mhz. El código debe estar debidamente comentado.*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#ifdef __USE_MCUEXPRESSO
#include <cr_section_macros.h> /* The cr_section_macros is specific to the MCUXpresso delivered toolchain */
#endif

#include "lpc17xx_adc.h"
#include "lpc17xx_pinsel"