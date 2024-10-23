/*Por un pin del ADC del microcontrolador LPC1769 ingresa una tensión de rango dinámico 0 a 3,3[v]
proveniente de un sensor de temperatura. Debido a la baja tasa de variación de la señal, se pide tomar una
muestra cada 30[s]. Pasados los 2[min] se debe promediar las últimas 4 muestras y en función de este valor,
tomar una decisión sobre una salida digital de la placa:
    ● Si el valor es <1 [V] colocar la salida en 0 (0[V]).
    ● Si el valor es >= 1[V] y <=2[V] modular una señal PWM con un Ciclo de trabajo que va desde el 50%
    hasta el 90% proporcional al valor de tensión, con un periodo de 20[KHz].
    ● Si el valor es > 2[V] colocar la salida en 1 (3,3[V]).*/

/*> <* g h */

#ifdef _USE_CMSIS
#include "LPC17xx.h"
#endif

/*libraries*/
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_systick.h"

/*pines*/
#define ADC_INPUT   ((uint32_t)(1<<2))   //P0.2 ADC input, channel 7
#define SALIDA ((uint32_t)(1<<0))        //P0.0 salida de la señal

#define ENABLE  1
#define DISABLE 0

#define INPUT 0
#define OUTPUT 1

#define RISE 1
#define FALL 0

#define ADC_FREC 100000

static uint32_t contador=0;
static uint32_t average=0;

/*funciones de configuracion*/
void cfg_port(void)
{
    PINSEL_CFG_Type pin;
    pin.Portnum=PINSEL_PORT_0;
    pin.Pinnum=PINSEL_PIN_2;
    pin.Funcnum=PINSEL_FUNC_2;
    pin.Pinmode=PINSEL_PINMODE_PULLDOWN;
    pin.OpenDrain=PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin(&pin);

    pin.Pinnum=PINSEL_PIN_0;
    pin.Funcnum=PINSEL_FUNC_0;
    PINSEL_ConfigPin(&pin);

    GPIO_SetDir(PINSEL_PORT_0, SALIDA, OUTPUT);
}

void cfg_tmr_match(void)
{
    TIM_TIMERCFG_Type tmr;
    tmr.PrescaleOption=TIM_PRESCALE_USVAL;
    tmr.PrescaleValue=1000000;              //el contador aumenta cada 1seg
    TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &tmr);

    TIM_MATCHCFG_Type match;
    match.MatchChannel=0;
    match.IntOnMatch=ENABLE;
    match.ResetOnMatch=ENABLE;
    match.StopOnMAtch=DISABLE;
    match.ExtMatchOutputType=TIM_EXTMATCH_NOTHING;
    match.MatchValue=30;                    //interrumpir en los 30seg para muestrear
}

void cfg_ADC(void)
{
    ADC_Init(LPC_ADC, ADC_FREC);
    ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_7, ENABLE);
    ADC_IntConfig(LPC_ADC,ADC_CHANNEL_7, ENABLE);
}

/*handlers de interrupciones*/
void TIMER0_IRQHandler(void)
{
    TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
    NVIC_EnableIRQ(ADC_IRQn);
    ADC_StartCmd(LPC_ADC, ADC_START_NOW)
    contador++;
}

void ADC_IRQHandler(void)
{
    NVIC_DisableIRQ(ADC_IRQn);
    if(contador<4)
    {
        average=ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_7);
    }
    else if (contador==4)
    {
        average=average/4;
        contador=0;
    }
}

void salida_señal(void)
{
    if(average<1)
    {
        GPIO_ClearValue(PINSEL_PORT_0, SALIDA);
    }
    else if (average>2)
    {
        GPIO_SetValue(PINSEL_PORT_0, SALIDA);
    }
}

/*con el systick armar el generador del pwm*/

