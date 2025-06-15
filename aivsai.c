#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "base.h"
#include "cards.h"
#include "aifables.h"
#include "alice.h"
#include "red_hood.h"
#include "kaguya.h"
#include "mulan.h"
#include "match-girl.h"
#include "snow_white.h"

#define INIT_DRAW 3
#define DRAW_PER_TURN 1
#define ENERGY_PER_TURN 3
#define MAX_ENERGY 10
#define MAX_HAND 5

extern Fable alice_fable, red_hood_fable, kaguya_fable;
extern Fable match_girl_enhanced_fable, mulan_fable, snow_white_fable;

extern Card *alice_deck[];        extern int alice_deck_size;
extern Card *redhood_deck[];      extern int redhood_deck_size;
extern Card *kaguya_deck[];       extern int kaguya_deck_size;
extern Card *mg_deck[];           extern int mg_deck_size;
extern Card *mulan_deck[];        extern int mulan_deck_size;
extern Card *snow_deck[];         extern int snow_deck_size;

extern Card *draw_deck(Deck *deck);
extern void draw_hand(Player *p, int n);


void add_role_cards(Player *p, Fable *fable) {
    Card **deck = NULL;
    int size = 0;
    if (fable == &alice_fable) { deck = alice_deck; size = alice_deck_size; }
    else if (fable == &red_hood_fable) { deck = redhood_deck; size = redhood_deck_size; }
    else if (fable == &kaguya_fable) { deck = kaguya_deck; size = kaguya_deck_size; }
    else if (fable == &match_girl_enhanced_fable) { deck = mg_deck; size = mg_deck_size; }
    else if (fable == &mulan_fable) { deck = mulan_deck; size = mulan_deck_size; }
    else if (fable == &snow_white_fable) { deck = snow_deck; size = snow_deck_size; }

    for (int i = 0; i < size && i < 6; ++i)
        add_deck(&p->draw, deck[i]);
    shuffle_deck(&p->draw);
    draw_hand(p, INIT_DRAW);
}

void print_hand(Player *p) {
    printf("  [手牌] %s：\n", p->fable->name);
    for (int i = 0; i < p->hand.cnt; ++i) {
        Card *c = p->hand.cards[i];
        if (c)
            printf("    - %s (cost:%d type:%d)\n", c->name, c->cst, c->type);
    }
}

void print_status(Player *p) {
    printf("➡️ [%s] HP: %d | DEF: %d | ENERGY: %d | Lane: %d\n",
           p->fable->name, p->health, p->defense, p->fable->energy, p->lane);
}

void refill_energy(Player *p) {
    p->fable->energy += ENERGY_PER_TURN;
    if (p->fable->energy > MAX_ENERGY)
        p->fable->energy = MAX_ENERGY;
}

void discard_used(Player *p) {
    for (int i = 0; i < p->hand.cnt; ++i) {
        if (p->hand.cards[i] == NULL) {
            for (int j = i + 1; j < p->hand.cnt; ++j)
                p->hand.cards[j - 1] = p->hand.cards[j];
            p->hand.cnt--;
            i--;
        }
    }
}

void ai_turn(Player *self, Player *enemy, FableShop *shop) {
    if (self->fable == &alice_fable) ai_alice_turn(self, enemy, shop);
    else if (self->fable == &red_hood_fable) ai_redhood_turn(self, enemy, shop);
    else if (self->fable == &kaguya_fable) ai_kaguya_turn(self, enemy, shop);
    else if (self->fable == &match_girl_enhanced_fable) ai_match_girl_turn(self, enemy, shop);
    else if (self->fable == &mulan_fable) ai_mulan_turn(self, enemy, shop);
    else if (self->fable == &snow_white_fable) ai_snowwhite_turn(self, enemy, shop);
}

int main() {
    srand(time(NULL));

    Fable *fables[] = {
        &alice_fable, &red_hood_fable,
        &match_girl_enhanced_fable, &mulan_fable, &snow_white_fable
    };

    int idx1 = rand() % 5, idx2;
    do { idx2 = rand() % 5; } while (idx2 == idx1);

    Player p1 = {0}, p2 = {0};
    p1.fable = fables[idx1];
    p2.fable = fables[idx2];
    p1.health = p1.fable->health;
    p2.health = p2.fable->health;
    p1.lane = p2.lane = 0;
    p1.fable->energy = p2.fable->energy = 5;

    add_role_cards(&p1, p1.fable);
    add_role_cards(&p2, p2.fable);

    FableShop shop = {0};

    printf("🤖 AI 1: %s\n", p1.fable->name);
    printf("🤖 AI 2: %s\n", p2.fable->name);

    int round = 1;
    while (p1.health > 0 && p2.health > 0) {
        printf("\n=== 第 %d 回合 ===\n", round++);

        printf("[AI 1] %s 行動\n", p1.fable->name);
        refill_energy(&p1);
        draw_hand(&p1, DRAW_PER_TURN);
        print_hand(&p1);
        ai_turn(&p1, &p2, &shop);
        print_status(&p1);
        print_status(&p2);
        discard_used(&p1);
        if (p2.health <= 0) break;

        printf("[AI 2] %s 行動\n", p2.fable->name);
        refill_energy(&p2);
        draw_hand(&p2, DRAW_PER_TURN);
        print_hand(&p2);
        ai_turn(&p2, &p1, &shop);
        print_status(&p1);
        print_status(&p2);
        discard_used(&p2);

        printf("\n按 Enter 鍵繼續...\n");
        getchar();
    }

    printf("\n🎉 勝者：%s\n", (p1.health > 0) ? p1.fable->name : p2.fable->name);
    return 0;
}
