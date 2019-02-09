# include "player.h"

Player::Player(void) {
	for (uint8_t i = 0; i < PLAYER_NAME_LENGTH; p_name[i++] = ' ');
	p_name[PLAYER_NAME_LENGTH] = '\0';
    next = 0;
}

void Player::init(const char *Name, uint32_t Cid, int32_t Money) {
	for (uint8_t i = 0; i < PLAYER_NAME_LENGTH; ++i) {
		if (Name[i] == ' ' || (Name[i] >= 'A' && Name[i] <= 'Z') || (Name[i] >= 'a' && Name[i] <= 'z')) {
			p_name[i] = Name[i];
		}
		if (Name[i] == 0) break;
	}
    // Remove spaces from the tail of the name
    for (int8_t i = PLAYER_NAME_LENGTH-1; i > 0; --i) {
        if (p_name[i] == ' ') {
            p_name[i] = 0;
        } else {
            break;
        }
    }
	p_card_id = Cid;
	p_money = Money;
}

int32_t Player::add(int32_t value) {
	p_money += value;
	return p_money;
}


void Players::init(void) {
	char p_name[PLAYER_NAME_LENGTH];
	for (uint8_t i = 0; i < PLAYER_NAME_LENGTH; p_name[i++] = ' ');
	p_name[PLAYER_NAME_LENGTH] = '\0';
     
    for (Player *p = top; p; p = p->next) {     // Clear all existing player names
        p->init(p_name, 0, 0);
    }
}

bool Players::exists(uint32_t cid) {
    return (instance(cid) != 0);
}

char* Players::name(uint32_t cid) {
	Player *p = instance(cid);
    if (p) return p->name();
	return 0;
}

int32_t Players::money(uint32_t cid) {
	Player *p = instance(cid);
	if (p) {
		return p->money();
	}
	return 0;
}

int32_t	Players::addMoney(uint32_t cid, int32_t value) {
	Player *p = instance(cid);
	if (p) {
		return p->add(value);
	}
	return 0;
}

bool Players::add(const char *Name, uint32_t Cid, int32_t Money) {
    Player *p = instance(Cid);
    if (p) return false;                        // This card ID is already registered
    p = instance(0);                            // Looking for empty slot (after previous game)
    if (!p) {
        p = (Player *)malloc(sizeof(Player));
        if (p) {
            p->next = top;
            top = p;
        }
    }
    if (p == 0) return false;
    p->init(Name, Cid, Money);
}

uint8_t Players::number(void) {
    uint8_t n = 0;
    for (Player *p = top; p; p = p->next) {
        if (p->cardID() != 0) ++n;
    }
    return n;
}

Player* Players::instance(uint32_t cid) {
    for (Player *p = top; p; p = p->next) {
        if (cid == p->cardID()) return p;
    }
    return 0;
}
