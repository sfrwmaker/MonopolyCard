#include "encoder.h"

ENCODER::ENCODER(uint8_t aPIN, uint8_t bPIN, uint8_t btnPIN, uint8_t shiftPIN, int32_t initPos) {
      m_pin = aPIN; s_pin = bPIN; b_pin = btnPIN; sh_pin = shiftPIN; pos = initPos;
}

void ENCODER::init(void) {
	pinMode(m_pin,  INPUT_PULLUP);
    pinMode(s_pin,  INPUT_PULLUP);
	pinMode(b_pin,  INPUT_PULLUP);
	pinMode(sh_pin, INPUT_PULLUP);
}

void ENCODER::reset(int32_t initPos, int32_t low, int32_t upp, uint8_t inc, uint8_t fast_inc, uint8_t shift_inc, bool looped) {
	min_pos = low; max_pos = upp;
	if (!write(initPos)) initPos = min_pos;
	increment = fast_increment = shift_increment = inc;
	if (fast_inc > increment) fast_increment    = fast_inc;
    if (shift_inc > fast_inc) shift_increment   = shift_inc;
	is_looped = looped;
}

bool ENCODER::write(int32_t initPos) {
	if ((initPos >= min_pos) && (initPos <= max_pos)) {
		pos = initPos;
		return true;
	}
	return false;
}

void ENCODER::rotateCB(void) {							// Interrupt function, called when the channel A of encoder changed
	bool rUp = digitalRead(m_pin);
	unsigned long now_t = millis();
	if (!rUp) {                                   		// The channel A has been "pressed"
		if ((rt == 0) || (now_t - rt > over_press)) {
			rt = now_t;
			channelB = digitalRead(s_pin);
		}
	} else {
		if (rt > 0) {
			uint8_t inc = increment;
			if ((now_t - rt) < over_press) {
				if ((now_t - changed) < fast_timeout) inc = fast_increment;
				changed = now_t;
				if (!digitalRead(sh_pin))               // shift button pressed
                    inc = shift_increment;
				if (channelB) pos -= inc; else pos += inc;
				if (pos > max_pos) { 
					if (is_looped)
						pos = min_pos;
					else 
						pos = max_pos;
				}
				if (pos < min_pos) {
					if (is_looped)
						pos = max_pos;
					else
						pos = min_pos;
				}
			}
			rt = 0; 
		}
	}
}

void ENCODER::pressCB(void) {							// Interrupt function, called when the button status changed
	bool keyUp = digitalRead(b_pin);
	uint32_t now_t = millis();
	if (!keyUp) {										// The button has been pressed
		if ((pt == 0) || (now_t - pt > over_press)) pt = now_t; 
	} else {
		if (pt > 0) {
			if ((now_t - pt) < short_press) mode = 1;	// short press
			else mode = 2;								// long press
			pt = 0;
		}
	}
}
