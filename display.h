#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <Arduino.h>
#include <LiquidCrystal.h>

class DSPL : public LiquidCrystal {
	public:
		DSPL(uint8_t RS, uint8_t E, uint8_t DB4, uint8_t DB5, uint8_t DB6, uint8_t DB7, uint8_t BL);
		void 			init(void);
		void			power(bool on);
		uint8_t			getBackLight(void)		    { return brightness; }
		void			setBackLight(uint8_t bl);
		void			playerName(const char *player_name, uint8_t sym_num, bool showCursor = true);
        void            player(const char *Name, int32_t Money, int32_t AddMoney);
        void            transfer(const char *sName, const char *dName, uint32_t Money);
        void            check(const char *Name1, int32_t Money1, const char *Name2 = 0, int32_t Money2 = 0);
	private:
		uint8_t bl_pin, brightness;
};

#endif
