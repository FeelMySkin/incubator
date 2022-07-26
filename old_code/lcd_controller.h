#ifndef LCD_CONTROLLER_H
#define LCD_CONTROLLER_H

#include "defines.h"
#include "i2c_controller.h"

#define PIN_RS    (1 << 0)
#define PIN_EN    (1 << 2)
#define BACKLIGHT (1 << 3)

class LCDController
{
    public:
        LCDController();
        ~LCDController();
        void Init(I2CController *i2c, uint8_t addr);
		void SendString(const char *str);
		void SetCursor(uint8_t row, uint8_t col);
		void SendCommand(uint8_t data);
		void Clear();
		void Home();	

    private:
		I2CController* i2c;
		uint8_t addr;
		void SendData(uint8_t data);
		void SendInternal(uint8_t data,uint8_t flags);

};


#endif
