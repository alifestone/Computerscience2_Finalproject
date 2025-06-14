#ifndef CARDS_H_INCLUDED
#define CARDS_H_INCLUDED
#include "base.h"
#include "red_hood.h"
#include "snow_white.h"
#include "match-girl.h"
#include "alice.h"
#include "mulan.h"
#include "kaguya.h"
//-------------------- 機制 --------------------
void add_deck(Deck *deck, Card *card);

Card *draw_deck(Deck *deck);
void shuffle_deck(Deck *deck);

void draw_hand(Player *player,int size); 

extern Card Attack1;
extern Card Defense1;
extern Card Move1;
extern Card Wild1;
extern Card Attack2;
extern Card Defense2;
extern Card Move2;
extern Card Attack3;
extern Card Defense3;
extern Card Move3;
#endif
