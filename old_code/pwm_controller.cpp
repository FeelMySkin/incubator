#include "pwm_controller.h"

PWMController::PWMController()
{
	pwm_count = 0;
}
//

PWMController::~PWMController()
{

}
//

void PWMController::Init()
{
	if(pwm_count == 0) return;
	InitGPIO();
	InitTIM();
}
//

void PWMController::InitGPIO()
{
	LL_GPIO_InitTypeDef gpio;
	gpio.Mode = LL_GPIO_MODE_ALTERNATE;
	gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio.Pull = LL_GPIO_PULL_NO;
	gpio.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	
	
	PWM_Struct* new_pwm = pwm;
	
	for(int i = 0;i<pwm_count;++i)
	{
		gpio.Alternate = new_pwm->af;
		gpio.Pin = new_pwm->pin;
		LL_GPIO_Init(new_pwm->gpio,&gpio);
		new_pwm = new_pwm->next;
	}
}
//

void PWMController::InitTIM()
{
	LL_TIM_InitTypeDef tim;
	tim.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	tim.CounterMode = LL_TIM_COUNTERMODE_UP;
	tim.Prescaler = SystemCoreClock/1000;
	tim.RepetitionCounter = 0;
	
	LL_TIM_OC_InitTypeDef oc;
	oc.CompareValue = 0;
	oc.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
	oc.OCMode = LL_TIM_OCMODE_PWM1;
	oc.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
	oc.OCState = LL_TIM_OCSTATE_ENABLE;
	
	PWM_Struct* new_pwm = pwm;
	for(int i = 0;i<pwm_count;++i)
	{
		tim.Autoreload = new_pwm->arr;
		LL_TIM_Init(new_pwm->tim,&tim);

		LL_TIM_OC_Init(new_pwm->tim,new_pwm->tim_ch,&oc);
		
		LL_TIM_OC_EnablePreload(new_pwm->tim,new_pwm->tim_ch);
		new_pwm = new_pwm->next;
	}
	new_pwm = pwm;
	for(int i = 0;i<pwm_count;++i)
	{
		LL_TIM_EnableCounter(new_pwm->tim);
		new_pwm = new_pwm->next;
	}
}
//

void PWMController::AddChannel(PWM_Struct* str)
{
	if(pwm_count == 0)
	{		
		pwm = str;
		pwm_count++;
		return;
	}
	
	PWM_Struct* new_pwm = pwm;
	for(int i = 0;i<pwm_count-1;++i)
	{
		new_pwm=new_pwm->next;
	}
	new_pwm->next = str;

	pwm_count++;
}
//

void PWMController::SetPercent(const char* id, uint8_t percent)
{
	
}
//
