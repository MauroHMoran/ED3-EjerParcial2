/*> <* g h */
/*
Utilizando el Timer 1, escribir un código en C para que por cada presión de un
pulsador, la frecuencia de parpadeo de un led disminuya a la mitad debido a la
modificación del reloj que llega al periférico.El pulsador debe producir una
interrupción por GPIO0 con flanco descendente.
*/

#ifdef _USE_CMSIS
#include "LPC17xx.h"
#endif

#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

/*pin definitions*/
#define PULSADOR ((uint32_t)(1<<0))
#define LED ((uint32_t)(1<<22))

/*definitions*/
#define OUTPUT 1
#define INPUT 0
#define RISE 0
#define FALL 1
#define ENABLE 1
#define DISABLE 0

/*variables globales*/
static uint32_t match_valor;
static bool led=0;

/*functions*/
void configure_ports (void);
void cfg_tmr_match (uint_32t match_value);
void EINT3_IRQHandler(void);
void TIMER1_IRQHandler(void);

/*principal function*/
int main(void)
{
    SystemInit();
    configure_ports();
    cfg_tmr_match(match_valor);
    TIM_Cmd(LPC_TIM1, ENABLE);
    NVIC_EnableIRQ(TIMER1_IRQn);
    
    while (ENABLE)
    {
        asm _nop;
    }
    
}

/*configure functions*/
void configure_ports(void)
{
    PINSEL_CFG_Type cfg_port;
    cfg_port.Portnum=PINSEL_PORT_0;
    cfg_port.Pinnum=PINSEL_PIN_0;
    cfg_port.Funcnum=PINSEL_FUNC_0;
    cfg_port.Pinmode=PINSEL_PINMODE_PULLDOWN;
    cfg_port.OpenDrain=PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin(*cfg_port);
    
    cfg_port.Pinnum=PINSEL_PIN_22;
    cfg_port.Pinmode=PINSEL_PINMODE_PULLUP;
    PINSEL_ConfigPin(*cfg_port);

    GPIO_SetDir(PINSEL_PORT_0, PULSADOR, INPUT);
    GPIO_SetDir(PINSEL_PORT_0, LED, OUTPUT);

    GPIO_IntCmd(PINSEL_PORT_0, PULSADOR, RISE);
}

void cfg_tmr_match(uint32_t match_value)
{
    TIM_TIMERCFG_Type cfg_tmr;
    cfg_tmr.PrescaleOption=TIM_PRESCALE_USVAL;
    cfg_tmr.PrescaleValue=100;      //cada 100us el contador aumenta su valor en 1

    TIM_MATCHCFG_Type match_cfg;
    match_cfg.MatchChannel=0;
    match_cfg.IntOnMatch=ENABLE;
    match_cfg.ResetOnMatch=ENABLE;
    match_cfg.StopOnMatch=DISABLE;
    match_cfg.ExtMatchOutputType=TIM_EXTMATCH_NOTHING;
    match_cfg.MatchValue=match_value;

    TIM_Init(LPC_TIM1, TIM_TIMER_MODE, *cfg_tmr);
    TIM_ConfigMatch(LPC_TIM1, *match_cfg)
}

/*interruption handler functions*/
void EINT3_IRQHandler(void)
{
    GPIO_ClearInt(PINSEL_PORT_0, PULSADOR);
    match_valor/=2;
}

void TIMER1_IRQHandler(void)
{
    TIM_ClearIntPending(LPC_TIM1,TIM_MR0_INT);
    led^=1;
    if(led)
    {
        GPIO_SetValue(PINSEL_PORT_0, LED);     
    }
    else
    {
        GPIO_ClearValue(PINSEL_PORT_0, LED);
    }
}