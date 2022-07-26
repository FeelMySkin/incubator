#ifndef HTS221_H
#define HTS221_H

#include "defines.h"
#include "i2c_controller.h"

struct HtsFlags
{
	bool inited:1;
	bool setupped:1;
	bool heating:1;
};

struct HtsVariables
{
	double T0_degC;
	double T1_degC;
	double H0_rh;
	double H1_rh;
	int16_t T0_OUT;
	int16_t T1_OUT;
	int16_t H0_OUT;
	int16_t H1_OUT;
	uint8_t conf,ctrl;
};


class HTS221
{
	public:
		void Init(I2CController*);
		void Process();
	
		double temp,hum;
	
	private:
		void CheckDevice();
		void SetupDevice();
		void CountTemp();
		void CountHum();
		void SetHeater(bool);
	
		HtsFlags flags;
		HtsVariables vars;
		I2CController* i2c;
};

#endif
