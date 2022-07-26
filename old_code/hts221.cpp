#include "hts221.h"

/*HTS221 Registers*/
#define HTS_ADDR 0x5F
#define WHO_AM_I 0x0F
#define AV_CONF 0x10
#define HTS_CTRL_REG1 0x20
#define HTS_CTRL_REG2 0x21
#define HTS_CTRL_REG3 0x22
#define STATUS_REG 0x27
#define H_OUT_L 0x28
#define H_OUT_H 0x29
#define TEMP_OUT_L 0x2A
#define TEMP_OUT_H 0x2B
#define T0_DEGC 0x32
#define T1_DEGC 0x33
#define T1_T0_MSB_DEGC 0x35
#define T0_OUT_L 0x3C
#define T0_OUT_H 0x3D
#define T1_OUT_L 0x3E
#define T1_OUT_H 0x3F
#define H0_OUT_L 0x36
#define H0_OUT_H 0x37
#define H1_OUT_L 0x3A
#define H1_OUT_H 0x3B

void HTS221::Init(I2CController* i2c)
{
	this->i2c = i2c;
	CheckDevice();
	SetupDevice();
}
//

void HTS221::CheckDevice()
{
	if(flags.inited == true) return;
	
	/*Else read WHOAMI register*/
	if(i2c->ReadRegister(HTS_ADDR,WHO_AM_I,1) == I2C_RESULT::I2C_RECV_OK)
	{
		if(i2c->received[0] == 0xBC)
		{
			flags.inited = true;
		}
		else flags.inited = false;
		//i2c_hw.StopLine();
	}
}
//

/*Setup device*/
void HTS221::SetupDevice()
{
	/*if not checcked check*/
	if(!flags.inited)
	{
		CheckDevice();
		return;
	}
	/*if already settuped return*/
	if(flags.setupped) return;
	
	uint8_t send = {0x3F};
	uint8_t send2 = {0x85};
	
	/*Write configs (see datasheet)*/
	if(i2c->WriteRegister(HTS_ADDR,AV_CONF,&send,1) == I2C_RESULT::I2C_WRITE_OK && i2c->WriteRegister(HTS_ADDR,HTS_CTRL_REG1,&send2,1) == I2C_RESULT::I2C_WRITE_OK)
	{
		
		/*Read all tweaking data from HTS221*/
		if(i2c->ReadRegister(HTS_ADDR,AV_CONF,1) == I2C_RESULT::I2C_RECV_OK) vars.conf = i2c->received[0];
		if(i2c->ReadRegister(HTS_ADDR,HTS_CTRL_REG1,1) == I2C_RESULT::I2C_RECV_OK) vars.ctrl = i2c->received[0];
		
		if(i2c->ReadRegister(HTS_ADDR,0x32,1) == I2C_RESULT::I2C_RECV_OK)
		{
			vars.T0_degC = i2c->received[0];
		}
		else return;
		if(i2c->ReadRegister(HTS_ADDR,0x33,1) == I2C_RESULT::I2C_RECV_OK)
		{
			vars.T1_degC = i2c->received[0];
		}
		else return;
		if(i2c->ReadRegister(HTS_ADDR,0x35,1) == I2C_RESULT::I2C_RECV_OK)
		{
			vars.T0_degC = (((uint16_t)i2c->received[0]&0x3)<<8 | static_cast<uint16_t>(vars.T0_degC));
			vars.T1_degC = (((uint16_t)i2c->received[0]&0xC)<<6 | static_cast<uint16_t>(vars.T1_degC));
			vars.T0_degC/=8;
			vars.T1_degC/=8;
		}
		else return;
		if(i2c->ReadRegister(HTS_ADDR,0x30,1) == I2C_RESULT::I2C_RECV_OK)
		{
			vars.H0_rh = i2c->received[0]/2;
		}
		else return;
		if(i2c->ReadRegister(HTS_ADDR,0x31,1) == I2C_RESULT::I2C_RECV_OK)
		{
			vars.H1_rh = i2c->received[0]/2;
		}
		else return;
		vars.T0_OUT = 0;
		vars.T1_OUT = 0;
		for(int i = 0;i<2;++i)
		{
			if(i2c->ReadRegister(HTS_ADDR,T0_OUT_L+i,1) == I2C_RESULT::I2C_RECV_OK) vars.T0_OUT |= i2c->received[0]<<(i*8);
			else return;
		}
		for(int i = 0;i<2;++i)
		{
			if(i2c->ReadRegister(HTS_ADDR,T1_OUT_L+i,1) == I2C_RESULT::I2C_RECV_OK) vars.T1_OUT |= i2c->received[0]<<(i*8);
			else return;
		}
		vars.H0_OUT = 0;
		vars.H1_OUT = 0;
		for(int i = 0;i<2;++i)
		{
			if(i2c->ReadRegister(HTS_ADDR,H0_OUT_L+i,1) == I2C_RESULT::I2C_RECV_OK) vars.H0_OUT |= i2c->received[0]<<(i*8);
			else return;
		}
		for(int i = 0;i<2;++i)
		{
			if(i2c->ReadRegister(HTS_ADDR,H1_OUT_L+i,1) == I2C_RESULT::I2C_RECV_OK) vars.H1_OUT |= i2c->received[0]<<(i*8);
			else return;
		}
		//i2c_hw.StopLine();
		flags.setupped = true;
	}
}
//

void HTS221::Process()
{
	//temp = 25.0;
	//hum = 20.0;
	//return;
	
	/*If not checked and not setupped - check and setup*/
	if(!flags.inited)
	{
		CheckDevice();
		return;
	}
	if(!flags.setupped)
	{		
		SetupDevice();
		return;
	}
	

	
	/*read status registers*/
	volatile uint8_t status;
	volatile uint8_t h;
	if(i2c->ReadRegister(HTS_ADDR,STATUS_REG,1) == I2C_RESULT::I2C_RECV_OK) status = i2c->received[0];
	else return;
	if(i2c->ReadRegister(HTS_ADDR,HTS_CTRL_REG1,1) == I2C_RESULT::I2C_RECV_OK) h = i2c->received[0];
	else return;
	if(h & 2) flags.heating = true;
	
	if(flags.heating) SetHeater(false);
	
	/*Count temp and humidity*/
	if((status &1)) CountTemp();
	if(status &2) CountHum();
	//i2c_hw.StopLine();
}
//

/*Count Temperature*/
void HTS221::CountTemp()
{
	int16_t T_OUT = 0;;
	for(int i = 0;i<2;++i)
	{
		if(i2c->ReadRegister(HTS_ADDR,TEMP_OUT_L+i,1) == I2C_RESULT::I2C_RECV_OK) T_OUT |= i2c->received[0]<<(i*8);
		else return;
	}
	
	/*Formula in datasheet. Used tweak values*/
	temp = 0;
	temp = (vars.T1_degC - vars.T0_degC)*(T_OUT-vars.T0_OUT)/(vars.T1_OUT-vars.T0_OUT);
	temp+= vars.T0_degC;
}
//

/*Count humidity*/
void HTS221::CountHum()
{
	int16_t H_OUT = 0;
	for(int i = 0;i<2;++i)
	{
		if(i2c->ReadRegister(HTS_ADDR,H_OUT_L+i,1) == I2C_RESULT::I2C_RECV_OK) H_OUT |= i2c->received[0]<<(i*8);
		else return;
	}
	
	/*Formula in datasheet. Used tweak values*/
	hum = 0;
	hum = (double)((vars.H1_rh - vars.H0_rh)*(H_OUT - vars.H0_OUT))/(double)(vars.H1_OUT - vars.H0_OUT);
	hum+= vars.H0_rh;
	
	if((hum >=100 || hum<=0) && !flags.heating) SetHeater(true);
	hum = hum>100.0?100:(hum<0?0:hum);
	
	/*Enable  for activating heater when humidity is over 70%*/
}
//

/*Activate heater (for drying humidity sensor)*/
void HTS221::SetHeater(bool stat)
{
	uint8_t stt = stat?1:0;
	uint8_t send[1] = {stt};
	if(stat)
	{		
		if(i2c->WriteRegister(HTS_ADDR,HTS_CTRL_REG2,send,1) == I2C_WRITE_OK) flags.heating = true;
	}
	else if(i2c->WriteRegister(HTS_ADDR,HTS_CTRL_REG2,send,1) == I2C_WRITE_OK) flags.heating = false;
}
//
