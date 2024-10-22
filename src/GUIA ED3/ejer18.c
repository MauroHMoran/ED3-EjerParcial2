/*18. Utilizando los 4 registros match del Timer 0 y las salidas P0.0, P0.1, P0.2 y P0.3,
realizar un programa en C que permita obtener las formas de ondas adjuntas,
donde los pulsos en alto tienen una duración de 5 mseg. Un pulsador conectado a
la entrada EINT3, permitirá elegir entre las dos secuencias mediante una rutina de
servicio a la interrupción. La prioridad de la interrupción del Timer tiene que ser
mayor que la del pulsador. Estas formas de ondas son muy útiles para controlar un
motor paso a paso.*/

/*> <* g h */

#ifdef _USE_CMSIS
#include "LPC17xx.h"
#endif

/*headers nad libraries*/
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_exti.h"

/*definitions*/
#define SALIDA0 ((uint32_t)(1<<0))
#define SALIDA1 ((uint32_t)(1<<1))
#define SALIDA2 ((uint32_t)(1<<2))
#define SALIDA3 ((uint32_t)(1<<3))
#define ENTRADA ((uint32_t)(1<<13))

#define INPUT 0
#define OUTPUT 1
#define RISE 0
#define FALL 1
#define ENABLE 1
#define DISABLE 0

static bool secuencia=0;
static uint32_t MSEG5=5000;
static uint32_t MSEG10=10000;

/*prototypes function*/
void cfg_ports(void);
void cfg_tmr_match(void);
void cfg_eint3(void);
void turn_off_all_output(void);
void EINT3_IRQHandler(void);
void TIMER0_IRQHandler(void);

/*principal function*/

/*configuration functions*/
void cfg_ports(void)
{
    PINSEL_CFG_Type port_cfg;
    port_cfg.Portnum=PINSEL_PORT_0;
    port_cfg.Pinnum=PINSEL_PIN_0;
    port_cfg.Funcnum=PINSEL_FUNC_0;
    port_cfg.Pinmode=PINSEL_PINMODE_PULLUP;
    port_cfg.OpenDrain=PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin(*port_cfg);

    port_cfg.Pinnum=PINSEL_PIN_1;
    PINSEL_ConfigPin(*port_cfg);

    port.cfg.Pinnum=PINSEL_PIN_2;
    PINSEL_ConfigPin(*port_cfg);

    port.cfg_Pinnum=PINSEL_PIN_3;
    PINSEL_ConfigPin(*port_cfg);

    GPIO_SetDir(PINSEL_PORT_0, SALIDA0|SALIDA1|SALIDA2|SALIDA3, OUTPUT);

    port_cfg.Portnum=PINSEL_PORT_2;
    port_cfg.Pinnum=PINSEL_PIN_13;
    port_cfg.Funcnum=PINSEL_FUNC_1;
    PINSEL_CFG_Type(*port_cfg);
}

void cfg_eint3(void)
{
    EXTI_InitTypeDef eint_cfg;
    eint_cfg.EXTI_Line=EXTI_EINT3;
    eint_cfg.EXTI_Mode=EXTI_MODE_EDGE_SENSITIVE;
    eint_cfg.EXTI_polarity=EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;
    EXTI_Config(*eint_cfg);
    NVIC_SetPriority(EINT3_IRQn, 1);
    NVIC_EnableIRQN(EINT3_IRQn);
}

void cfg_tmr_match(uint32_t match_tim)
{
    TIM_TIMERCFG_Type tmr_cfg;
    tmr_cfg.PrescaleOption=TIM_PRESCALE_USVAL;
    tmr_cfg.PrescaleValue=1;                     //el contador aumenta cada 1us
    TIM_Init(LPC_TIM0, TIM_TIMER_MODE, *tmr_cfg);

    TIM_MATCHCFG_Type match_cfg;
    match_cfg.MatchChannel=0;
    match_cfg.IntOnMatch=ENABLE;
    match_cfg.ResetOnMatch=DISABLE;
    match_cfg.StopOnMatcg=DISABLE;
    match_cfg.ExtMatchOutputType=TIM_EXTMATCH_NOTHING;
    match_cfg.MatchValue=match_tim;
    TIM_ConfigMatch(LPC_TIM0, *match_cfg);

    match_cfg.MatchChannel=1;
    match_cfg.MatchValue=2*match_tim;
    TIM_ConfigMatch(LPC_TIM0, *match_cfg);

    match_cfg.MatchChannel=2;
    match_cfg.MatchValue=3*match_tim;
    TIM_ConfigMatch(LPC_TIM0, *match_cfg);

    match_cfg.MatchChannel=3;
    match_cfg.ResetOnMatch=ENABLE;
    match_cfg.MatchValue=4*match_tim;
    TIM_ConfigMatch(LPC_TIM0, *match_cfg);

    NVIC_SetPriority(TIMER0_IRQn, 0);
}

/*interruption handlers*/
void EINT3_IRQHandler(void)
{
    EXTI_ClearEXTIFlag(EXTI_EINT3);
    turn_off_all_output();
    secuencia^=1;
    if(secuencia)
    {
        cfg_tmr_match(MSEG10);
    }
    else
    {
        cfg_tmr_match(MSEG5);
    }
}

void TIMER0_IRQHandler(void)
{
    turn_off_all_output();
    if(TIM_GetIntStatus(LPC_TIM0,TIM_MR0_INT))
    {
        GPIO_SetValue(PINSEL_PORT_0, SALIDA0);
    }
    else if (TIM_GetIntStatus(LPC_TIM0, TIM_MR1_INT)
    {
        GPIO_SetValue(PINSEL_PORT_0,SALIDA1);
    })
    else if(TIM_GetIntStatus(LPC_TIM0,TIM_MR2_INT))
    {
        GPIO_SetValue(PINSEL_PORT_0,SALIDA2);
    }
    else if (TIM_GetIntStatus(LPC_TIM0, TIM_MR3_INT))
    {
        GPIO_SetValue(PINSEL_PORT_0, SALIDA3);
    }
    TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT|TIM_MR1_INT|TIM_MR2_INT|TIM_MR3_INT);
}

/*additional functions*/
void turn_off_all_output(void)
{
    GPIO_ClearValue(PINSEL_PORT_0, SALIDA0|SALIDA1|SALIDA2|SALIDA3);
}