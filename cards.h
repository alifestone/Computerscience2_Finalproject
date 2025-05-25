#ifndef CARDS_H_INCLUDED
#define CARDS_H_INCLUDED
#include "base.h"
#include "fables.h"
//-------------------- 機制 --------------------
void add_deck(Deck *deck, Card *card) {
    if(deck->cnt < MAX_CARD) deck->cards[deck->cnt++] = card;
}

Card *draw_deck(Deck *deck) {
    if(deck->cnt == 0) return NULL;
    return deck->cards[--deck->cnt];
}

void shuffle_deck(Deck *deck) {
    for(int i = deck->cnt-1; i > 0; i--) {
        int32_t j = rand() % (i+1);
        Card *tmp = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = tmp;
    }
}

void draw_hand(Player *player,int size) {
    for(int i = 0; i < size; i++) {
        Card *card = draw_deck(&player->draw);
        if(card) add_deck(&player->hand, card);
    }
}

Card Attack1={
    .name="Attack",
    .type=BASIC_ATK,
    .cst=1,
    .dmg=1,
    .defense=0,
    .mov=0,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card Defense1={
    .name="Defense",
    .type=BASIC_DEF,
    .cst=1,
    .dmg=0,
    .defense=1,
    .mov=0,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card Move1={
    .name="Move",
    .type=BASIC_MOV,
    .cst=1,
    .dmg=0,
    .defense=0,
    .mov=1,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card Wild1={
    .name="Wild",
    .type=UNIVERSAL,
    .cst=1,
    .dmg=1,
    .defense=1,
    .mov=1,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card Attack2={
    .name="Attack",
    .type=BASIC_ATK,
    .cst=2,
    .dmg=2,
    .defense=0,
    .mov=0,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card Defense2={
    .name="Defense",
    .type=BASIC_DEF,
    .cst=2,
    .dmg=0,
    .defense=2,
    .mov=0,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card Move2={
    .name="Move",
    .type=BASIC_MOV,
    .cst=2,
    .dmg=0,
    .defense=0,
    .mov=2,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card Attack3={
    .name="Attack",
    .type=BASIC_ATK,
    .cst=3,
    .dmg=3,
    .defense=0,
    .mov=0,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card Defense3={
    .name="Defense",
    .type=BASIC_DEF,
    .cst=3,
    .dmg=0,
    .defense=3,
    .mov=0,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card Move3={
    .name="Move",
    .type=BASIC_MOV,
    .cst=3,
    .dmg=0,
    .defense=0,
    .mov=3,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
#endif
