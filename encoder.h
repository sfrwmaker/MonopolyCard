#ifndef _ENCODER_H
#define _ENCODER_H
#include <Arduino.h>

class ENCODER {
	public:
		ENCODER(uint8_t aPIN, uint8_t bPIN, uint8_t btnPIN, uint8_t shiftPIN, int32_t initPos = 0);
		void		init(void);
		int32_t		read(void)						{ return pos; }
		void		reset(int32_t initPos, int32_t low, int32_t upp, uint8_t inc = 1, uint8_t fast_inc = 0, uint8_t shift_inc = 0, bool looped = false);
		bool		write(int32_t initPos);
		uint8_t		btnStatus(void)                  { uint8_t m = mode; mode = 0; return m; }
		void    	rotateCB(void);
		void		pressCB(void);
	private:
		int32_t           	min_pos		= -2147483647;
		int32_t				max_pos		=  2147483647;
		volatile uint32_t	rt			= 0;		// Time in ms when the encoder was rotated
		volatile uint32_t 	pt			= 0;        // Time in ms when the encoder button was pressed
		volatile uint32_t 	changed		= 0;        // Time in ms when the value was changed
		volatile bool     	channelB	= false;
		volatile int32_t  	pos;                    // Encoder current position
		volatile uint8_t	mode		= 0;		// The button mode: 0 - not pressed, 1 - short pressed, 2 - long pressed
		uint8_t             m_pin, s_pin, b_pin, sh_pin; // The pin numbers connected to the main channel, secondary channel, button and shift
		bool              	is_looped	= false;     // Whether the encoder is looped
		uint8_t             increment	= 1;         // The value to add or substract for each encoder tick
		uint8_t             fast_increment  = 0;     // The value to change encoder when in runs quickly
        uint8_t             shift_increment = 0;     // The value to change encoder when shift pressed
   		const uint16_t    	fast_timeout    = 300;   // Time in ms to change encoder quickly
		const uint16_t    	over_press       = 1500;
        const uint16_t      short_press      = 1000;
};

#endif
