/*Programar el microcontrolador LPC1769 para que mediante su ADC digitalice
dos señales analógicas cuyos anchos de bandas son de 10 Khz cada una. Los canales
utilizados deben ser el 2 y el 4 y los datos deben ser guardados en dos regiones de
memorias distintas que permitan contar con los últimos 20 datos de cada canal. Suponer
una frecuencia de core cclk de 100 Mhz. El código debe estar debidamente comentado.*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include "lpc17xx_adc.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

#define ENABLE 1
#define DISABLE 0

#define DONE 1
#define NO_DONE 0

#define ADC_FREC 40000

static uint8_t auxCanal2=0;
static uint8_t auxCanal4=0; 
static uint32_t canal2[19]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static uint32_t canal4[19]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void cfg_port(void)
{
    PINSEL_CFG_Type port;
    port.Portnum=PINSEL_PORT_0;
    port.Pinnum=PINSEL_PIN_25;                   //ADC channel 2
    port.Funcnum=PINSEL_FUNC_1;
    port.Pinmode=PINSEL_PINMODE_PULLUP;
    port.OpenDrain=PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin(&port)

    port.Portnum=PINSEL_PORT_1;
    port.Pinnum=PINSEL_PIN_30;                   //ADC channel 4
    port.Funcnum=PINSEL_FUNC_2
    PINSEL_ConfigPin(&port);    
}

/*al estar muestreando dos señales de 10k, debemos usar 20k de muestreo para cada señal, por lo que se usa
una frecuencia de muestreo de 40k (20k para cada señal)*/

void cfg_ADC(void)
{
    ADC_Init(LPC_ADC, ADC_FREC);
    ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_2, ENABLE);
    ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_4, ENABLE);
    ADC_IntConfig(LPC_ADC, ADC_CHANNEL_2, ENABLE);
    ADC_IntConfig(LPC_ADC, ADC_CHANNEL_4, ENABLE);
    NVIC_Enable(ADC_IRQn);
    ADC_StartCmd(LPC_ADC, ADC_START_CONTINUOUS);
}

void ADC_IRQHandler(void)
{
    if(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_2, DONE));
    {
        if(auxCanal2<20)
        {
            canal2[auxCanal2]=ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_2);
            auxCanal2++;
        }
        else
        {
            auxCanal2=0;
        }
        
    }
    else if(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_4, DONE))
    {
        if(auxCanal4<20)
        {
            canal4[auxCanal4]=ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_4);
        }
        else
        {
            auxCanal4=0;
        }
    }
}