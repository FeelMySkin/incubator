#ifndef PWM_CONTROLLER_H
#define PWM_CONTROLLER_H

#include "defines.h"

struct PWM_Struct
{
	TIM_TypeDef*	tim;
	GPIO_TypeDef* 	gpio;
	uint32_t		pin;
	const char*		tim_id;
	uint32_t		tim_ch;
	uint32_t 		arr;
	PWM_Struct*		next;
	uint32_t		af;
};

class PWMController
{
    public:
        PWMController();
        ~PWMController();
        void Init();
		void AddChannel(PWM_Struct*);
		void SetPercent(const char* id, uint8_t percent);

    private:
		void InitGPIO();
		void InitTIM();
		PWM_Struct* pwm;
		uint16_t pwm_count;
};


#endif
