#include "display.h"

DSPL::DSPL(uint8_t RS, uint8_t E, uint8_t DB4, uint8_t DB5, uint8_t DB6, uint8_t DB7, uint8_t BL)
            : LiquidCrystal(RS, E, DB4, DB5, DB6, DB7) {
    bl_pin      = BL;
    brightness  = 100;
}

void DSPL::init(void) {
	begin(16, 2);
	clear();
	pinMode(bl_pin, OUTPUT);
	analogWrite(bl_pin, brightness);
}


void DSPL::setBackLight(uint8_t bl) {
	brightness = bl;
	analogWrite(bl_pin, brightness);
}

void DSPL::playerName(const char *player_name, uint8_t sym_num, bool showCursor) {
	if (sym_num >= 8) return;
	setCursor(0, 0);
	print(F("Player name:"));
	setCursor(0, 1);
	print(player_name);
    if (showCursor && sym_num < 16) {
	    cursor();
	    setCursor(sym_num, 1);
    } else {
        noCursor();
    }
}

void DSPL::power(bool on) {
	if (on) {
		display();
        for (uint8_t bl = 0; bl < brightness; ++bl) {
            analogWrite(bl_pin, bl);
            delay(5);
        }
	} else {
		noDisplay();
        for (int16_t bl = brightness; bl > 0; --bl) {
            analogWrite(bl_pin, bl);
            delay(5);
        }
        digitalWrite(bl_pin, LOW);
	}
}

void DSPL::player(const char *Name, int32_t Money, int32_t AddMoney) {
    setCursor(0, 0);
	char buff[18];
	sprintf(buff, "%s,", Name);
    print(buff);
    setCursor(8, 0);
    sprintf(buff, " $%6d", Money);
    print(buff);
    setCursor(0, 1);
    char sym = '+';
    int32_t m = AddMoney;
    if (m < 0) {
        sym = '-';
        m *= -1;
    }
    setCursor(8, 1);
    sprintf(buff, "%c$%6d", sym, m);
    print(buff);
}

void DSPL::transfer(const char *sName, const char *dName, uint32_t Money) {
    clear();
    print(sName);
    setCursor(7, 0);
    print(" -> ");
    print(dName);
    char buff[10];
    sprintf(buff, "%8d", Money);
    setCursor(0, 1);
    print(buff);
}
