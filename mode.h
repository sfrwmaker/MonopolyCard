#ifndef _MODE_H
#define _MODE_H

#include "player.h"
#include "display.h"
#include "encoder.h"
#include <MFRC522.h>

class MODE {
	public:
		MODE()										{ }
		virtual void	init(void)        			{ }
		virtual MODE*	show(void)					{ }
		virtual MODE*	press(void)					{ return this; }
		virtual MODE*	longPress(void)             { if (this->next != 0) return this->next; else return this; }
		virtual void	rotaryValue(int32_t value)  { }
		void    		forceRedraw(void)           { update_screen = 0; }
		MODE*	next = 0;							// Pointer to the next mode
	protected:
		uint32_t		update_screen;				// Time in ms when the screen should be updated
};

class NewGame: public MODE {
	public:
		NewGame(DSPL *Disp, ENCODER *Enc, Players *Play, MFRC522 *RF);
		virtual void	init(void);
		virtual MODE*	show(void);
        virtual MODE*   press(void);
		virtual MODE*	longPress(void);
		virtual void	rotaryValue(int32_t value);
	private:
		uint8_t			symbolToCode(char sym);
		char			codeToSymbol(uint8_t code);
		void			playerInit(void);
		DSPL			*pD;
		ENCODER			*pEnc;
		Players			*p;
		MFRC522			*pRF;
		uint8_t			player;
		uint8_t			mode;					// 0 - wait for new card, 1 - select letter, 2 - edit letter
		uint32_t		card_id;				// User CARD ID
		uint8_t			sym_num;				// Index of the symbol in user name to edit
        uint32_t        select_timeout;         // Time in ms when to turn automatically from mode 2 to mode 1
		char			player_name[PLAYER_NAME_LENGTH+1];
		const			uint32_t	period = 1000;
};

class Sleep: public MODE {
	public:
		Sleep(DSPL *Disp, ENCODER *Enc, MFRC522 *RF);
		virtual void	init(void);
		virtual MODE*	show(void);
		virtual MODE*	press(void)				{ sleep_ms = millis() + sleep_timeout; return this; }
		virtual MODE*	longPress(void)			{ return 0; }
		virtual void	rotaryValue(int32_t value);
	private:
		DSPL			*pD;
		ENCODER			*pEnc;
        MFRC522         *pRF;
		uint32_t		sleep_ms	= 0;		// The time in ms when go into the sleep mode
		bool			sleeping	= false;	// Sleep mode flag
		const			uint32_t	period 			=  1000;
		const			uint32_t	sleep_timeout	= 10000;
};

class Move: public MODE {
	public:
		Move(DSPL *Disp, ENCODER *Enc, Players *Play, MFRC522 *RF);
		virtual void	init(void);
		virtual MODE*	show(void);
		virtual MODE*	press(void);
		virtual MODE*	longPress(void)			{ }
		virtual void	rotaryValue(int32_t value);
	private:
		DSPL			*pD;
		ENCODER			*pEnc;
		Players			*p;
		MFRC522			*pRF;
		uint32_t		p_card_id	= 0;
		int32_t			money		= 0;
		int32_t		    add_money	= 0;
        uint32_t        return_ms   = 0;        // Time in ms when return to 'Sleep' mode
        bool            check_tran  = false;    // Whether check another card for money transfer
        const           uint32_t    period          = 1000;
        const           uint32_t    timeout         = 10000;
};

class Confirm: public MODE {
    public:
        Confirm(DSPL *Disp, ENCODER *Enc);
        virtual void    init(void);
        virtual MODE*   show(void);
        virtual MODE*   press(void);
        virtual void    rotaryValue(int32_t value)  { forceRedraw(); }
        MODE            *keep;
    private:
        DSPL            *pD;
        ENCODER         *pEnc;
        const           uint32_t    period          = 10000;
};

#endif
