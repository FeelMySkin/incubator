#include "main.h"


/*Initialize basic GPIOs*/
void InitGPIO()
{
	LL_GPIO_InitTypeDef gpio;
	gpio.Mode = LL_GPIO_MODE_OUTPUT;
	gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio.Pin = LL_GPIO_PIN_5;
	gpio.Pull = LL_GPIO_PULL_NO;
	gpio.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	LL_GPIO_Init(GPIOA,&gpio);
	
	LL_GPIO_SetOutputPin(GPIOA,LL_GPIO_PIN_5);
}
//

/*Set RCC*/
void InitRCC()
{
	SystemCoreClockUpdate();
}
//

/*On/Off Peripherals*/
void InitPeriph()
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
}
//

int main()
{
	InitRCC();
	InitGPIO();
	while(1) ;
}
//
