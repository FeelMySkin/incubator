#include "Source.h"

//#define NANOPRINTF_IMPLEMENTATION
//#include "nanoprintf.h"

#define CLOCK_PRESCALER 50

double temp = 0;
double last_temp = 0;
char string_up[16] = {""};
char string_down[16] = {""};

volatile double need_temp = 37.7;
volatile uint8_t curr_state_clk, last_state_clk;

volatile int32_t click_ctr;
volatile double calced_error;
volatile double mid_temp; 
uint32_t timer = 0;
uint8_t update_pointer = 0;

const uint32_t update_hours[3] = {6,4,0};

#define UPDATE_MSEC		600
#define ROT_SEC			4*60*60
#define PWM_SETTER		95

extern "C"
{
	void TIM14_IRQHandler()
	{
		LL_TIM_ClearFlag_UPDATE(TIM14);
		i2c.TimeoutHandler();
	}
	//
	
	void TIM17_IRQHandler()
	{
		adc.ProcessAll();
		LL_TIM_ClearFlag_UPDATE(TIM17);
		//calced_error = pid.CalculateError(need_temp-hts.temp,UPDATE_MSEC);
	}
	//
	
	void TIM16_IRQHandler()
	{
		if(LL_TIM_IsActiveFlag_UPDATE(HEATER_TIM))
		{
			LL_TIM_ClearFlag_UPDATE(HEATER_TIM);
			if(GetTimCC(HEATER_TIM,HEATER_TIM_CH) != 0) LL_GPIO_SetOutputPin(HEATER_GPIO,HEATER_PIN);
		}
		else
		{
			ClearTimCCFlag(HEATER_TIM,HEATER_TIM_CH);
			LL_GPIO_ResetOutputPin(HEATER_GPIO,HEATER_PIN);
		}
	}
	//
	
	void SysTick_Handler()
	{
		if(!flags.rot)timer++;
		else
		{
			timer--;
			LL_GPIO_TogglePin(ROTATOR_GPIO,ROTATOR_PIN);
			if(timer == 0)
			{
				flags.rot = false;
				//timer = 50;
				LL_GPIO_TogglePin(ROTATOR_EN_GPIO,ROTATOR_EN_PIN);
			}
		}
		
		if(timer >= update_hours[update_pointer]*60*60*CLOCK_PRESCALER && update_hours[update_pointer] != 0)
		{
			flags.rot = true;
			timer = 1000;
			LL_GPIO_TogglePin(ROTATOR_EN_GPIO,ROTATOR_EN_PIN);
		}
	}
	
}
//

void Redraw()
{
	//lcd.Clear();
	lcd.SetCursor(0,0);
	lcd.SendString(string_up);
	lcd.SetCursor(1,0);
	lcd.SendString(string_down);
}
//

void ProcessEncoder()
{
	double l_n = need_temp;
	need_temp+=((int32_t)LL_TIM_GetCounter(ENCODER_TIM) -32000)/10.0;
	LL_TIM_SetCounter(ENCODER_TIM,32000);
	if(l_n != need_temp) flags.need_update = true;
}
//

int main()
{
	
	InitRCC();
	InitPeriph();
	
	flags.rot = false;
	
	InitPID();
	InitADC();
	InitHeater();
	InitI2C();
	InitButton();
	InitUpdater();
	InitEncoder();
	InitRotator();
	InitHTS();
	InitSysTick();
	
	
	lcd.Init(&i2c,0x3F);
	lcd.Clear();
	lcd.SetCursor(0,0);
	SetUpperString();
	SetLowerString();
	Redraw();
	
	
	while(1)
	{
		__WFI();
		
		if(!(LL_GPIO_ReadInputPort(BUTTON_GPIO)&BUTTON_PIN) && !flags.pressed)
		{
			flags.pressed = true;
			flags.need_update = true;
			update_pointer++;
			update_pointer%=3;
		}
		else if(LL_GPIO_ReadInputPort(BUTTON_GPIO)&BUTTON_PIN && flags.pressed)
		{
			flags.pressed = false;
		}
		
		ProcessEncoder();
		temp = 1.0/TEMP_ZERO + (1.0/TERM_BETA)*log(adc.getMeasure(TEMP_RES_CH)/TERM_R_ZERO);
		temp = 1.0/temp - TEMP_ZERO + 25;
		hts.Process();
		mid_temp = (temp + hts.temp)/2;
		
		if(hts.temp<(need_temp) && calced_error != PWM_SETTER) calced_error = PWM_SETTER;
		else if(hts.temp>(need_temp) && calced_error != 0) calced_error = 0;
		
		heater.SetHeaterPercent(calced_error);
		
		if((uint16_t)(last_temp*10) == (uint16_t)(temp*10))
		{
			SetUpperString();
			Redraw();
		}
		if(flags.need_update)
		{
			flags.need_update = false;
			SetLowerString();
			Redraw();
		}
		last_temp = temp;
	}
}
//

void InitSysTick()
{
	LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
	LL_SYSTICK_EnableIT();
	SysTick_Config(SystemCoreClock/CLOCK_PRESCALER);
	NVIC_EnableIRQ(SysTick_IRQn);
}
//

void InitPID()
{
	PID_InitTypeDef pids;
	pids.Kp = 25;
	pids.Ki = 1;
	pids.Kd = 8;
	pids.max_val = 85.0;
	pid.Init(pids);
}
//

void InitRCC()
{
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
	while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI) ;
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
	
	LL_RCC_PLL_Disable();
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI_DIV_2,LL_RCC_PLL_MUL_8); //32MHz
	LL_RCC_PLL_Enable();
	
	while(!LL_RCC_PLL_IsReady()) ;
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
	while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) ;
	
	SystemCoreClockUpdate();
	LL_Init1msTick(SystemCoreClock);
	LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
	LL_SetSystemCoreClock(SystemCoreClock);
	LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_HSI);
}
//

void InitHeater()
{
	Heater_InitTypeDef htr;
	htr.heat_gpio = HEATER_GPIO;
	htr.heat_pin = HEATER_PIN;
	htr.heat_tim = HEATER_TIM;
	htr.heat_tim_ch = HEATER_TIM_CH;
	htr.soft_pwm = true;
	heater.Init(htr);
}
//

void InitPeriph()
{
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3 | LL_APB1_GRP1_PERIPH_TIM14 | LL_APB1_GRP1_PERIPH_I2C1);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA | LL_AHB1_GRP1_PERIPH_GPIOB | LL_AHB1_GRP1_PERIPH_GPIOF | LL_AHB1_GRP1_PERIPH_DMA1);
	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_ADC1 | LL_APB1_GRP2_PERIPH_TIM16 | LL_APB1_GRP2_PERIPH_TIM17);
	LL_SYSCFG_SetRemapDMA_ADC(LL_SYSCFG_ADC1_RMP_DMA1_CH1);
	
}
//

void InitI2C()
{
	I2CController_TypeDef i2c_gen;
	
	i2c_gen.i2c = 		I2C1;
	i2c_gen.i2c_af = 	I2C1_AF;
	i2c_gen.scl_gpio =	I2C1_SCL_GPIO;
	i2c_gen.scl_pin =	I2C1_SCL_PIN;
	i2c_gen.sda_gpio =	I2C1_SDA_GPIO;
	i2c_gen.sda_pin =	I2C1_SDA_PIN;
	i2c_gen.timing =	I2C1_TIMING;
	i2c_gen.help_tim =		I2C1_TIMEOUT_TIM;
	i2c.Init(i2c_gen);
	
}
//

void InitADC()
{
	ADC_Struct adc_str;
	adc_str.ch_num = TEMP_RES_CH;
	adc_str.coeff = TERM_R_ZERO;
	adc_str.gpio = TEMP_RES_GPIO;
	adc_str.offset = 0;
	adc_str.pin = TEMP_RES_PIN;
	adc_str.type = ADC_TYPE_NEG_RESISTANCE;
	adc.AddLine(adc_str);
	adc.Init();
}
//


void InitUpdater()
{
	LL_TIM_InitTypeDef tim;
	tim.Autoreload = 600;
	tim.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	tim.CounterMode = LL_TIM_COUNTERMODE_UP;
	tim.Prescaler = SystemCoreClock/1000;
	tim.RepetitionCounter = 0;
	LL_TIM_Init(UPDATE_TIM,&tim);
	
	LL_TIM_EnableIT_UPDATE(UPDATE_TIM);
	LL_TIM_ClearFlag_UPDATE(UPDATE_TIM);
	
	EnableTimIRQn(UPDATE_TIM,2);
	LL_TIM_EnableCounter(UPDATE_TIM);
}
//

void InitEncoder()
{
	
	LL_GPIO_InitTypeDef gpio;
	gpio.Alternate = ENCODER_AF;
	gpio.Mode = LL_GPIO_MODE_ALTERNATE;
	gpio.Pull = LL_GPIO_PULL_UP;
	gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	
	gpio.Pin = ENCODER_A_PIN;
	LL_GPIO_Init(ENCODER_A_GPIO,&gpio);
	
	gpio.Pin = ENCODER_B_PIN;
	LL_GPIO_Init(ENCODER_B_GPIO,&gpio);

	LL_TIM_ENCODER_InitTypeDef enc;
	LL_TIM_ENCODER_StructInit(&enc);
	LL_TIM_ENCODER_Init(ENCODER_TIM,&enc);
	
	LL_TIM_InitTypeDef tm;
	LL_TIM_StructInit(&tm);
	LL_TIM_Init(ENCODER_TIM,&tm);
	
	LL_TIM_SetCounter(ENCODER_TIM,32000);
	LL_TIM_EnableCounter(ENCODER_TIM);
	
}
//

void InitHTS()
{
	hts.Init(&i2c);
}
//

void InitRotator()
{
	LL_GPIO_InitTypeDef gpio;
	gpio.Mode = LL_GPIO_MODE_OUTPUT;
	gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio.Pin = ROTATOR_PIN;
	gpio.Pull = LL_GPIO_PULL_NO;
	gpio.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	LL_GPIO_Init(ROTATOR_GPIO,&gpio);
	
	gpio.Pin = ROTATOR_EN_PIN;
	LL_GPIO_Init(ROTATOR_EN_GPIO,&gpio);
	LL_GPIO_SetOutputPin(ROTATOR_EN_GPIO,ROTATOR_EN_PIN);
}
//

void InitButton()
{
	LL_GPIO_InitTypeDef gpio;
	gpio.Alternate = LL_GPIO_AF_0;
	gpio.Mode = LL_GPIO_MODE_INPUT;
	gpio.Pull = LL_GPIO_PULL_UP;
	gpio.Pin = BUTTON_PIN;
	gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio.Speed = LL_GPIO_SPEED_LOW;
	LL_GPIO_Init(BUTTON_GPIO,&gpio);
}
//


void printer(char* to, const char* from, uint8_t shift, uint8_t length)
{
	for(uint8_t i = 0;i<length;++i)
	{
		to[shift+i]=from[i];
	}
}
//

void SetUpperString()
{
	uint8_t high_part = 0;
	uint8_t low_part = 0;
	high_part = hts.temp;
	low_part = (hts.temp - high_part)*10;
	char number[4];
	number[0] = '0'+(high_part/10);
	number[1] = '0'+(high_part%10);
	number[2] = '.';
	number[3] = '0'+low_part;
	printer(string_up,"T:",0,2); //2
	printer(string_up,number,2,4); //2+4=6
	printer(string_up,"C H:",6,4); //6+4=10
	number[0] = '0'+(static_cast<uint16_t>(hts.hum)/10);
	number[1] = '0'+(static_cast<uint16_t>(hts.hum)%10);
	printer(string_up,number,10,2); //10+2=12
	printer(string_up,"%%",12,1); //12+1=13
	
	//npf_snprintf(string_up,20,"T:%i.%iC H:%i%%",high_part,low_part,static_cast<uint16_t>(hts.hum));
}
//

void SetLowerString()
{
	uint8_t high_part = 0;
	uint8_t low_part = 0;
	high_part = need_temp;
	low_part = (need_temp - high_part)*10;
	char number[4];
	number[0] = '0'+(high_part/10);
	number[1] = '0'+(high_part%10);
	number[2] = '.';
	number[3] = '0'+low_part;
	printer(string_down,"Need: ",0,6);//0+6=6
	printer(string_down,number,6,4);//6+4=10
	printer(string_down," Rot:",10,5);//10+5=15
	number[0] = '0'+update_hours[update_pointer];
	printer(string_down,number,15,1);
	//npf_snprintf(string_down,20,"Need: %i.%i C",high_part,low_part);
}
//


