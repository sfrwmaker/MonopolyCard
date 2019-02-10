#include "mode.h"
#include <MFRC522.h>
#include <avr/sleep.h>
#include <avr/power.h>

NewGame::NewGame(DSPL *Disp, ENCODER *Enc, Players *Play, MFRC522 *RF) {
	pD			= Disp;
	pEnc		= Enc;
	p			= Play;
	pRF			= RF;
	mode		= 0;
	pEnc->reset(0, 0, 0, 0, 0, 0, false);
}

void NewGame::init(void) {
	p->init();
	mode 	= 0;                                // 0 - wait for new card, 1 - select letter, 2 - edit letter
	card_id = 0;
    select_timeout  = 0;
	pEnc->reset(pD->getBackLight(), 0, 255, 5, 20, 50, false);    // Prepare to tune LCD backlight brightness
}

MODE* NewGame::show(void) {
	uint32_t now_ms = millis();
    if (mode == 2 && select_timeout && now_ms > select_timeout) {
        pEnc->reset(sym_num, 0, PLAYER_NAME_LENGTH-1, 1, 1, 1, true);
        mode = 1;
        select_timeout = 0;
        pD->clear();
        forceRedraw();
    }
	if (now_ms < update_screen) return this;
	
	update_screen = now_ms + period;
	switch(mode) {
		case 0:									// wait for the new card
			pD->setCursor(0, 0);
            pD->print(F("New game"));
            pD->setCursor(0, 1); 
			pD->print(F("Register a card"));
			if (pRF->PICC_IsNewCardPresent() && pRF->PICC_ReadCardSerial()) {
			    card_id = 0;
                for (uint8_t i = 0; i < 4; ++i) {
                    card_id <<= 8;
                    card_id |= pRF->uid.uidByte[i];
                }
				if (!p->exists(card_id)) {	    // This card ID does not exist in the player list
					playerInit();               // Initialize internal array of player name
                    mode    = 2;                // Switch to letter enter mode
					pEnc->reset(int32_t(symbolToCode(player_name[sym_num])), 0, int32_t('Z'-'A'+1), 1, 3, 10, true);
                    pD->clear();
                    forceRedraw();
				} else {
					pD->setCursor(0, 1);
					pD->print(F("Use another card"));
                    delay(2000);
                    pD->clear();
					card_id = 0;
				}
			}
			break;

		case 1:									// Select symbol in the name to be changed
            pD->setCursor(13, 0);
            pD->print("<->");

		case 2:									// Edit the symbol in the name, do not show cursor
			pD->playerName(player_name, sym_num);
			break;

		default:
			break;
	}
	return this;
}


void NewGame::rotaryValue(int32_t value) {
	switch(mode) {
		case 0:									// Wait for the new card
			pD->setBackLight(uint8_t(value));	// Adjust LCD backlight
			break;

		case 1:									// Select character
			sym_num = value;
			break;

		case 2:									// Edit the character
			player_name[sym_num] = codeToSymbol(value);
            select_timeout = millis() + 5000;   // Keep symbol select
			break;

		default:
			return;
	}
	forceRedraw();
}

MODE* NewGame::press(void) {
    pD->clear();
    pD->noCursor();
    forceRedraw();

	switch(mode) {
		case 1:									// Letter select mode
			break;

		case 2:									// Letter edit mode, choose next letter
            if (++sym_num >= PLAYER_NAME_LENGTH-1)
                sym_num = PLAYER_NAME_LENGTH-1;
			break;

		default:
			return this;
	}
    pEnc->reset(int32_t(symbolToCode(player_name[sym_num])), 0, int32_t('Z'-'A'+1), 1, 3, 10, true);
    mode = 2;
    select_timeout = millis() + 5000;           // Enter next symbol for 5 secs or switch to letter select mode
	return this;
}

MODE* NewGame::longPress(void) {
    pD->clear();
    pD->noCursor();

	if (mode > 0) {                             // Register new player
		if (!p->add(player_name, card_id)) {
            pD->clear();
            pD->print(F("Registration"));
            pD->setCursor(1, 1);
            pD->print(F("failed"));
            delay(2000);
            pD->clear();
		}
		mode    = 0;
        card_id = 0;
		pEnc->reset(pD->getBackLight(), 0, 255, 5, 20, 50, false);    // Prepare to tune LCD backlight brightness
		forceRedraw();
		return this;
	} else {                                    // Finish registering, start the game
        if (p->number() < 2) {
            pD->print(F("Insufficient"));
            pD->setCursor(1, 1);
            pD->print(F("players"));
            delay(3000);
            return this;
        }
		if (this->next != 0) return this->next; else return this;
	}
}

uint8_t	NewGame::symbolToCode(char sym) {
	if (sym == ' ') {
		return 0;
	} else {
		if (sym > 'Z') {
			sym -= 'a';
		} else {
			sym -= 'A';
		}
		++sym;
	}
	return uint8_t(sym);
}

char NewGame::codeToSymbol(uint8_t code) {
	if (code == 0) {							// 0 means space character
		return ' ';
	} else {
		--code;
		if (sym_num > 0) {
			code += 'a';
		} else {
			code += 'A';
		}
	}
	return char(code);
}

void NewGame::playerInit(void) {
	sym_num = 0;
	for (uint8_t i = 1; i < PLAYER_NAME_LENGTH; player_name[i++] = ' ');
    player_name[0] = 'A';
	player_name[PLAYER_NAME_LENGTH] = '\0';
}


Sleep::Sleep(DSPL *Disp, ENCODER *Enc, MFRC522 *RF) {
	pD				= Disp;
	pEnc			= Enc;
    pRF         	= RF;
}

void Sleep::init(void) {
    uint8_t bl = pD->getBackLight();
	pEnc->reset(bl, 0, 255, 1, 5, 10, false);	    // Adjust back light of the LCD display
	sleep_ms = millis() + sleep_timeout;
	sleeping = false;
}

MODE* Sleep::show(void) {
	if (sleeping) {
		// Setup the WDT
		MCUSR &= ~(1<<WDRF);                    // Clear the reset flag
		WDTCSR |= (1<<WDCE) | (1<<WDE);
		WDTCSR  = (1<<WDP2)|(1<<WDP1)|(1<<WDP0);// Set new watchdog timeout prescaler value 2.0 second
		WDTCSR |= (1<<WDIE);                    // Enable the WD interrupt (note no reset).
	
		// Entering the sleep mode
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // The lowest power consumption
		sleep_enable();
		sleep_cpu();
  
		// The program will continue from here after the key pressed or WDT interrupt
		sleep_disable();                        // First thing to do is disable sleep.
		power_all_enable();                     // Re-enable the peripherals.
	} else {
		uint32_t now_ms = millis();
		if (now_ms < update_screen) return this;

		if (now_ms > sleep_ms) {				// Switch to sleep mode
			pD->power(false);
			sleeping = true;
		}
		update_screen = now_ms + period;
		pD->setCursor(0, 0);
		pD->print(F("Next player"));
		pD->setCursor(0, 1);
		pD->print(F("Place a card"));
	}
	
	if (next && pRF->PICC_IsNewCardPresent()) {	// If the card present, exit sleep mode
		if (sleeping) {
			WDTCSR &= ~(1<<WDIE);               // Disable the WD interrupt
			pD->power(true);					// Switch on the display
		}
		return next;
	}
	return this;
}

void Sleep::rotaryValue(int32_t value) {
    sleep_ms = millis() + sleep_timeout;
    pD->setBackLight(value);
}

Move::Move(DSPL *Disp, ENCODER *Enc, Players *Play, MFRC522 *RF) {
	pD			= Disp;
	pEnc		= Enc;
	p			= Play;
	pRF			= RF;
}

void Move::init(void) {
	p_card_id = 0;
	add_money = 0;
	if (pRF->PICC_ReadCardSerial()) {
		uint32_t card_id = 0;
        for (uint8_t i = 0; i < 4; ++i) {
            card_id <<= 8;
            card_id |= pRF->uid.uidByte[i];
        }
		if (p->exists(card_id)) {               // If the card is registered, prepare to enter money change
			pEnc->reset(0, -99999, 99999, 1, 5, 50, false);
			p_card_id	= card_id;
			money		= p->money(card_id);
		}
	}
    check_tran  = false;
    return_ms   = millis() + timeout;           // Setup time when to return to 'Sleep' mode
}

MODE* Move::show(void) {
	uint32_t now_ms = millis();
	if (now_ms < update_screen) return this;
    if (return_ms && next && now_ms >= return_ms) return next;

    update_screen = now_ms + period;

    // No encoder activity, if we pay for something, check for another card because we can transfer money
    if (check_tran && pRF->PICC_IsNewCardPresent() && pRF->PICC_ReadCardSerial()) {
        uint32_t dest_card_id = 0;               // Destination card ID
        for (uint8_t i = 0; i < 4; ++i) {
            dest_card_id <<= 8;
            dest_card_id |= pRF->uid.uidByte[i];
        }
        // If the card is belong to another player, transfer money
        if (dest_card_id != p_card_id && p->exists(dest_card_id)) {
            p->addMoney(p_card_id, add_money);
            add_money *= -1;
            p->addMoney(dest_card_id, add_money);
            pD->transfer(p->name(p_card_id), p->name(dest_card_id), uint32_t(add_money));
            delay(2000);                        // Wait to show transfer message
            pD->check(p->name(p_card_id), p->money(p_card_id), p->name(dest_card_id), p->money(dest_card_id));
            delay(3000);
            if (next) return next; else return this;
        }        
    }
    
    check_tran = (add_money < 0);               // If we pay, check for another card next loop

	if (!p->exists(p_card_id)) {
		pD->print(F("Wrong card"));
		delay(2000);
		if (next) return next;
	}
	pD->player(p->name(p_card_id), money, add_money);
	return this;
}

MODE* Move::press(void) {
	p->addMoney(p_card_id, add_money);
    if (add_money != 0) {
        pD->check(p->name(p_card_id), p->money(p_card_id));
        delay(3000);
    }
	if (next) return next; else return this;
}

void Move::rotaryValue(int32_t value) {
	add_money   = value;
    check_tran  = false;
    if (value == 0) {
        return_ms   = millis() + timeout;       // Update return time
    } else {
        return_ms = 0;                          // Do not calcel the operation by timeout
    }
	forceRedraw();
}


Confirm::Confirm(DSPL *Disp, ENCODER *Enc) {
    pD          = Disp;
    pEnc        = Enc;
}

void Confirm::init(void) {
    pEnc->reset(0, 0, 1, 1, 0, 0, true);
}

MODE* Confirm::show(void) {
    uint32_t now_ms = millis();
    if (now_ms < update_screen) return this;

    update_screen = now_ms + period;
    pD->setCursor(0, 0);
    pD->print(F("Restart game?"));
    pD->setCursor(0, 1);
    if (pEnc->read()) {
        pD->print(F("Yes"));
    } else {
        pD->print(F("No "));
    }
    return this;
}

MODE* Confirm::press(void) {
    if (pEnc->read()) {
        if (next) return next;
    } else {
        if (keep) return keep;
    }
    return this;
}
