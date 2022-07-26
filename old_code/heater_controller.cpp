#include "heater_controller.h"

HeaterController::HeaterController()
{

}
//

HeaterController::~HeaterController()
{

}
//

void HeaterController::Init(Heater_InitTypeDef htr)
{
	this->htr = htr;
	InitGPIO();
	InitTIM();
}
//

void HeaterController::InitGPIO()
{
	LL_GPIO_InitTypeDef gpio;
	gpio.Alternate = htr.heat_af;
	
	if(htr.soft_pwm) gpio.Mode = LL_GPIO_MODE_OUTPUT;
	else gpio.Mode = LL_GPIO_MODE_ALTERNATE;
	gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio.Pin = htr.heat_pin;
	gpio.Pull = LL_GPIO_PULL_NO;
	gpio.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	LL_GPIO_Init(htr.heat_gpio,&gpio);
}
//

void HeaterController::InitTIM()
{
	LL_TIM_InitTypeDef tim;
	tim.Autoreload = 1250;
	tim.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	tim.CounterMode = LL_TIM_COUNTERMODE_UP;
	tim.Prescaler = SystemCoreClock/10000-1;
	tim.RepetitionCounter = 0;
	
	LL_TIM_Init(htr.heat_tim,&tim);
	
	ClearTimCCFlag(htr.heat_tim,htr.heat_tim_ch);
	LL_TIM_ClearFlag_UPDATE(htr.heat_tim);
	
	LL_TIM_OC_InitTypeDef oc;
	oc.CompareValue = 0;
	oc.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
	oc.OCMode = LL_TIM_OCMODE_PWM1;
	oc.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
	oc.OCState = LL_TIM_OCSTATE_ENABLE;
	LL_TIM_OC_Init(htr.heat_tim,htr.heat_tim_ch,&oc);
	
	LL_TIM_OC_EnablePreload(htr.heat_tim,htr.heat_tim_ch);
	LL_TIM_EnableCounter(htr.heat_tim);
	if(htr.soft_pwm)
	{	
		LL_TIM_EnableIT_UPDATE(htr.heat_tim);
		EnableTimCCIRQn(htr.heat_tim,htr.heat_tim_ch);
		LL_TIM_ClearFlag_UPDATE(htr.heat_tim);
		ClearTimCCFlag(htr.heat_tim,htr.heat_tim_ch);
		EnableTimIRQn(htr.heat_tim,0);
	}
}
//

void HeaterController::SetHeaterPercent(uint8_t percent)
{
	uint16_t new_oc = LL_TIM_GetAutoReload(htr.heat_tim)*percent/100.0;
	if(htr.heat_tim_ch == LL_TIM_CHANNEL_CH1) LL_TIM_OC_SetCompareCH1(htr.heat_tim,new_oc);
	if(htr.heat_tim_ch == LL_TIM_CHANNEL_CH2) LL_TIM_OC_SetCompareCH2(htr.heat_tim,new_oc);
	if(htr.heat_tim_ch == LL_TIM_CHANNEL_CH3) LL_TIM_OC_SetCompareCH3(htr.heat_tim,new_oc);
	if(htr.heat_tim_ch == LL_TIM_CHANNEL_CH4) LL_TIM_OC_SetCompareCH4(htr.heat_tim,new_oc);
}
//
