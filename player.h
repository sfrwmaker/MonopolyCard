#ifndef _PLAYER_H
#define _PLAYER_H
#include <Arduino.h>

#define PLAYER_NAME_LENGTH	8

class Player {
	public:
		Player(void);
		void        init(const char *Name, uint32_t Cid, int32_t Money = 1500);
		char*		name(void)					{ return p_name; }
		int32_t		money(void)					{ return p_money; }
		uint32_t	cardID(void)				{ return p_card_id; }
		int32_t		add(int32_t value);
        Player      *next       = 0;            // Pointer to the next player in the list
	private:
		char		p_name[PLAYER_NAME_LENGTH+1];
		uint32_t	p_card_id   = 0;
		int32_t		p_money     = 0;
};

class Players {
	public:
		Players(void)                           { }
		void		init(void);
        bool        exists(uint32_t cid);
		char* 		name(uint32_t cid);
		int32_t		money(uint32_t cid);
		int32_t		addMoney(uint32_t cid, int32_t value);
		bool		add(const char *Name, uint32_t Cid, int32_t Money = 1500);
        uint8_t     number(void);               // Number of players in the game
	private:
        Player*     instance(uint32_t cid);     // The pointer to the player with specified card ID or 0 if not found
		Player* 	top = 0;                    // Pointer to the first player instance in the list
};

#endif
