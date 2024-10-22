/*Programar el microcontrolador LPC1769 en un código de lenguaje C para que, utilizando un timer y 
un pin de capture de esta placa sea posible demodular una señal PWM que ingresa por dicho pin 
(calcular el ciclo de trabajo y el periodo) y sacar una tensión continua proporcional al ciclo de trabajo 
a través del DAC de rango dinámico 0-2V con un rate de actualización de 0,5s del promedio de las 
últimos diez valores obtenidos en la captura.*/
/*> <* g h */

#ifdef _USE_CMESIS
#include "LPC17xx.h"
#endif

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_timer.h"

#define CAPTURA ((uint32_t)(1<<4))  //P0.4 para CAP0 del TMR2
//#define DAC_OUTPUT ((uint32_t)(1<<))

#define ENABLE 1
#define DISABLE 0

#define TRUE 1
#define FALSE 0

static uint32_t flanco_subida=0;
static uint32_t flanco_bajada=0;
static uint32_t t_on=0;
static uint32_t t_off=0;
static bool int_flanco=FALSE;
static uint32_t contador=0;
static uint32_t average=0;

void cfg_ports(void)
{
    PINSEL_CFG_Type port_cfg;
    port_cfg.Portnum=PINSEL_PORT_0;
    port_cfg.Pinnum=PINSEL_PIN_4;
    port_cfg.Funcnum=PINSEL_FUNC_3;
    port_cfg.Pinmode=PINSEL_PINMODE_PULLUP;
    port_cfg.OpenDrain=PINSEL_PINMODE_NORMAL;

    PINSEL_ConfigPin(*port_cfg);
}

void cfg_tmr_cap_match(void)
{
    TIM_TIMERCFG_Type tmr_cfg:
    tmr_cfg.PrescaleOption=TIM_PRESCALE_USVAL;
    tmr.cfg.PrescaleValue=1;                    //el contador aumenta cada un useg
    TIM_Init(LPC_TIM2, TIM_TIMER_MODE, *tmr_cfg);

    TIM_CAPTURECFG_Type cap_cfg:
    cap_cfg.CaptureChannel=0;
    cap_cfg.RisingEdge=ENABLE;
    cap_cfg.FallingEdge=ENABLE;
    cap_cfg.IntOnCaption=ENABLE;
    TIM_ConfigCapture(LPC_TIM2, *cap_cfg);

    tmr_cfg.PrecaleValue=1000;      //cada 1ms aumenta el contador;
    TIM_Init(LPC_TIM0, TIM_TIMER_MODE, *tmr_cfg);

    TIM_MATCHCFG_Type match_cfg;
    match_cfg.MatchChannel=0;
    match_cfg.IntOnMatch=ENABLE;
    match_cfg.ResetOnMatch=ENABLE;
    match_cfg.StopOnMatch=DISABLE;
    match_cfg.ExtMatchOutputType=TIM_EXTMATCH_NOTHING;
    match_cfg.MatchValue=5;
    TIM_ConfigMatch(LPC_TIM0, *match_cfg);
}

void TIMER0_IRQHandler(void)
{
    TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
    
    DAC_Init(LPC_DAC);
    DAC_UpdateValue(LPC_DAC, average);
}

void TIMER2_IRQHandler(void)
{
    TIM_ClearIntCapturePending(LPC_TIM2, TIM_CR0_INT);
    /*considero que el pwm empieza en bajo, asi que la primer captura es un alto (comienza el duty cycle)*/
    int_flanco^=1;
    if(int_flanco)
    {
        t_off=TIM_GetCaptureValue(LPC_TIM2, 0);
    }
    else
    {
        t_on=TIM_GetCaptureValue(LPC_TIM2,0)-t_off;
        TIM_ResetCounter(LPC_TIM2);
        avrg();
    }
}

void avrg(void)
{
    if (contador<10)
    {
        contador++;
        average+=t_on;
    }
    {
        else
        {
            average=average/10;
            contador=0;
        }
    }
}
