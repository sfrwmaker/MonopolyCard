/*
 * -------------------------------------
 *             MFRC522      Arduino       
 *             Reader/PCD   Uno           
 * Signal      Pin          Pin           
 * -------------------------------------
 * RST/Reset   RST          9             
 * Interrupt   IRQ          -              
 * SPI SS      SDA(SS)      10            
 * SPI MOSI    MOSI         11 / ICSP-4   
 * SPI MISO    MISO         12 / ICSP-1   
 * SPI SCK     SCK          13 / ICSP-3   
 */

#include <SPI.h>
#include <MFRC522.h>
#include "display.h"
#include "mode.h"
#include "player.h"

// Rotary Encoder
const uint8_t	ENC_A_PIN		= 2;
const uint8_t	ENC_B_PIN		= 4;
const uint8_t	ENC_BTN_PIN		= 3;
const uint8_t	ENC_SH_PIN		= 5;

// Mifare RFID card reader config
const uint8_t   RST_PIN = 9;        // Configurable, see typical pin layout above
const uint8_t   SS_PIN  = 10;       // Configurable, see typical pin layout above

// LCD 1602 display config
// The LCD 1602 parallel interface
const uint8_t   LCD_RS_PIN      = 7;
const uint8_t   LCD_E_PIN       = 8;
const uint8_t   LCD_DB4_PIN     = A0;
const uint8_t   LCD_DB5_PIN     = A1;
const uint8_t   LCD_DB6_PIN     = A2;
const uint8_t   LCD_DB7_PIN     = A3;
const uint8_t   LCD_BL_PIN      = 6;

MFRC522		mfrc522(SS_PIN, RST_PIN);
DSPL    	lcd(LCD_RS_PIN, LCD_E_PIN, LCD_DB4_PIN, LCD_DB5_PIN, LCD_DB6_PIN, LCD_DB7_PIN, LCD_BL_PIN);
ENCODER		rEnc(ENC_A_PIN, ENC_B_PIN, ENC_BTN_PIN, ENC_SH_PIN);
Players		PL;
NewGame		newGameMode(&lcd, &rEnc, &PL, &mfrc522);
Sleep		sleepMode(&lcd, &rEnc, &mfrc522);
Move		moveMode(&lcd, &rEnc, &PL, &mfrc522);
Confirm     confirmMode(&lcd, &rEnc);

static      MODE*       pCurrMode = &newGameMode;

void setup() {
//    Serial.begin(115200);
	SPI.begin();			        // Init SPI bus
	mfrc522.PCD_Init();		        // Init MFRC522
    lcd.init();
	rEnc.init();
	attachInterrupt(digitalPinToInterrupt(ENC_A_PIN), 	rotEncChange,   CHANGE);
	attachInterrupt(digitalPinToInterrupt(ENC_BTN_PIN), rotPushChange,  CHANGE);
	
	newGameMode.next	= &sleepMode;
	sleepMode.next		= &moveMode;
	moveMode.next		= &sleepMode;
    confirmMode.next    = &newGameMode;
    confirmMode.keep    = &sleepMode;

    pCurrMode->init();
}

void rotEncChange(void) {
	rEnc.rotateCB();
}

void rotPushChange(void) {
	rEnc.pressCB();
}

ISR(WDT_vect) { }                	// Watch Dog Timer interrupt handler


void loop() {
    static int32_t  old_pos = rEnc.read();
  
    MODE* nxt = pCurrMode;
    int32_t pos = rEnc.read();
    if (old_pos != pos) {
        pCurrMode->rotaryValue(pos);
        old_pos = pos;
    }

    uint8_t bStatus = rEnc.btnStatus();
    switch (bStatus) {
        case 2:                     // long press;
            nxt = pCurrMode->longPress();
            break;

        case 1:                     // short press
            nxt = pCurrMode->press();
        break;

        case 0:                     // Not pressed
        default:
            break;
    }

    if (!nxt) nxt = &confirmMode;
    if (nxt != pCurrMode) {
        pCurrMode = nxt;
        pCurrMode->init();
        old_pos = rEnc.read();
        lcd.clear();
        pCurrMode->forceRedraw();
    }

    nxt = pCurrMode->show();
    if (!nxt) nxt = &confirmMode;
    if (nxt != pCurrMode) {
        pCurrMode = nxt;
        pCurrMode->init();
        old_pos = rEnc.read();
        lcd.clear();
        pCurrMode->forceRedraw();
    }
    delay(30);
}
