#ifndef PERIPH_USE_H
#define PERIPH_USE_H


#include "stdint.h"
#include "stdlib.h"
#include "stm32f0xx.h"
//#include "stm32f030x6.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_rcc.h"
//#include "stm32f0xx_ll_crs.h"
#include "stm32f0xx_ll_cortex.h"
#include "stm32f0xx_ll_pwr.h"
#include "stm32f0xx_ll_tim.h"
#include "stm32f0xx_ll_utils.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_i2c.h"
#include "stm32f0xx_ll_adc.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_dma.h"
#include "stm32f0xx_ll_exti.h"

#ifdef __cplusplus
extern "C"
{
#endif
	
	void EnableExtiIRQn(uint32_t exti_line,uint8_t priority);
	void EnableTimIRQn(TIM_TypeDef* tim,uint8_t priority);
	void EnableTimCCIRQn(TIM_TypeDef* tim,uint32_t ch);
	void EnableDmaIRQn(DMA_TypeDef* dma, uint32_t stream,uint8_t priority);
	void DisableDmaIRQn(DMA_TypeDef* dma, uint32_t channel);
	void EnableUsartIrqn(USART_TypeDef* usart,uint8_t priority);
	//void DisableExtiIRQn(uint32_t exti_line);void DisableDmaIRQn(DMA_TypeDef* dma, uint32_t channel)
	
	//void SetExtiSource(GPIO_TypeDef* gpio,uint32_t exti_line);
	uint32_t GetExtiLine(uint32_t pin);
	//void ClearDmaTCFlag(DMA_TypeDef* dma, uint32_t dma_stream);
	void ClearTimCCFlag(TIM_TypeDef* tim,uint32_t ch);
	uint32_t GetTimCC(TIM_TypeDef* tim,uint32_t ch);
	
#ifdef __cplusplus
}
#endif


/*GPIO*/
#define HEATER_GPIO			GPIOF
#define HEATER_PIN			LL_GPIO_PIN_0

#define I2C1_SDA_GPIO		GPIOA
#define I2C1_SCL_GPIO		GPIOA
#define I2C1_SDA_PIN		LL_GPIO_PIN_10
#define I2C1_SCL_PIN		LL_GPIO_PIN_9
#define I2C1_AF				LL_GPIO_AF_4

#define TEMP_RES_GPIO		GPIOA
#define TEMP_RES_PIN		LL_GPIO_PIN_0

#define ENCODER_A_GPIO		GPIOA
#define ENCODER_A_PIN		LL_GPIO_PIN_6
#define ENCODER_B_GPIO		GPIOA
#define ENCODER_B_PIN		LL_GPIO_PIN_7
#define ENCODER_AF			LL_GPIO_AF_1

#define ROTATOR_GPIO		GPIOA
#define ROTATOR_PIN			LL_GPIO_PIN_1
#define ROTATOR_EN_GPIO		GPIOA
#define ROTATOR_EN_PIN		LL_GPIO_PIN_2

#define BUTTON_GPIO			GPIOB
#define BUTTON_PIN			LL_GPIO_PIN_1

/*TIM*/
#define HEATER_TIM			TIM16
#define HEATER_TIM_CH		LL_TIM_CHANNEL_CH1

#define ENCODER_TIM			TIM3
#define I2C1_TIMEOUT_TIM	TIM14
#define UPDATE_TIM			TIM17

/*I2C*/
#define I2C1_TIMING			0x20B //0x00101322//0x2000090E

/*ADC*/
#define TEMP_RES_CH			0

//DMA
#define ADC_DMA				DMA1
#define ADC_DMA_CHANNEL		LL_DMA_CHANNEL_1


#endif
