#include "lcd_controller.h"

LCDController::LCDController()
{

}
//

LCDController::~LCDController()
{

}
//

void LCDController::Init(I2CController *i2c, uint8_t addr)
{
	this->i2c = i2c;
	this->addr = addr;
	
	// 4-bit mode, 2 lines, 5x7 format
    SendCommand(0x30); //0b00110000
	Home();
    // display on, right shift, underline off, blink off
    SendCommand(0xC); //0b00001100
	Clear();
}
//

void LCDController::SendCommand(uint8_t data)
{
	SendInternal(data, 0);
}
//

void LCDController::SendData(uint8_t data)
{
	SendInternal(data,PIN_RS);
}
//

void LCDController::SendInternal(uint8_t data, uint8_t flags)
{
	uint8_t up = data & 0xF0;
    uint8_t lo = (data << 4) & 0xF0;

    uint8_t data_arr[4];
    data_arr[0] = up|flags|BACKLIGHT|PIN_EN;
    data_arr[1] = up|flags|BACKLIGHT;
    data_arr[2] = lo|flags|BACKLIGHT|PIN_EN;
    data_arr[3] = lo|flags|BACKLIGHT;
	i2c->WriteBytes(addr,data_arr,4);
	i2c->Await(5);

}
//

void LCDController::SendString(const char *str)
{
	while(*str) {
        SendData((uint8_t)(*str));
        str++;
    }
}
//

void LCDController::Clear()
{
	// clear display (optional here)
    SendCommand(0x01); //0b00000001
}
//

void LCDController::Home()
{
	// display & cursor home (keep this!)
    SendCommand(0x02); //0b00000010
}
//

void LCDController::SetCursor(uint8_t row, uint8_t col)
{
	SendCommand(0x80 | (row*0x40 + col));
}
//
