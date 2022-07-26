#ifndef SOURCE_H
#define SOURCE_H


#include "defines.h"
#include "heater_controller.h"
#include "i2c_controller.h"
#include "lcd_controller.h"
#include "adc_controller.h"
#include "pid_controller.h"
#include "hts221.h"

#define TERM_BETA 	4300
#define TEMP_ZERO	298.15
#define TERM_R_ZERO	10000

HeaterController heater;
I2CController i2c;
LCDController lcd;
ADCController adc;
PIDController pid;
HTS221			hts;

volatile MainFlags flags;

void InitRCC();
void InitHeater();
void InitPeriph();
void InitI2C();
void InitADC();
void InitUpdater();
void InitPID();
void InitEncoder();
void InitHTS();
void SetUpperString();
void SetLowerString();
void InitRotator();
void InitButton();
void InitSysTick();

#endif
