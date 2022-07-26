#ifndef HEATER_CONTROLLER_H
#define HEATER_CONTROLLER_H

#include "defines.h"

struct Heater_InitTypeDef
{
	GPIO_TypeDef*	heat_gpio;
	uint32_t		heat_pin;
	uint32_t		heat_af;
	TIM_TypeDef*	heat_tim;
	uint32_t		heat_tim_ch;
	bool			soft_pwm;
};
//

class HeaterController
{
    public:
        HeaterController();
        ~HeaterController();
        void Init(Heater_InitTypeDef);
		void SetHeaterPercent(uint8_t);

	private:
		void InitGPIO();
		void InitTIM();
		Heater_InitTypeDef htr;
};


#endif
