#include "match-girl.h"
#include "cards.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

MGState match_girl_state = {0};

// --- EPIC 效果函式宣告 ---
void hell_flame_effect(void *self, void *target);
void doomfall_effect(void *self, void *target);
void curse_of_greed_effect(void *self, void *target);

// ------------------- 攻擊技能 -------------------

Card phantom_wish = {
    .name = "Phantom Wish", .type = SKILL_ATK, .cst = 0, .dmg = 1, .defense = 0,
    .mov = 0, .rng = 1, .link = false, .effect = NULL
};

Card secret_desire = {
    .name = "Secret Desire", .type = SKILL_ATK, .cst = 2, .dmg = 2, .defense = 0,
    .mov = 0, .rng = 1, .link = false, .effect = NULL
};

Card endless_luxury = {
    .name = "Endless Luxury", .type = SKILL_ATK, .cst = 4, .dmg = 3, .defense = 0,
    .mov = 0, .rng = 1, .link = false, .effect = NULL
};

// ------------------- 防禦技能 -------------------

Card devil_sacrifice = {
    .name = "Devil's Sacrifice", .type = SKILL_DEF, .cst = 0, .dmg = 0, .defense = 1,
    .mov = 0, .rng = 0, .link = false, .effect = NULL
};

Card devil_bet = {
    .name = "Devil's Bet", .type = SKILL_DEF, .cst = 2, .dmg = 0, .defense = 1,
    .mov = 0, .rng = 0, .link = false, .effect = NULL
};

Card devil_contract = {
    .name = "Devil's Contract", .type = SKILL_DEF, .cst = 4, .dmg = 0, .defense = 1,
    .mov = 0, .rng = 0, .link = false, .effect = NULL
};

// ------------------- 移動技能 -------------------

Card weightless_soul = {
    .name = "Weightless Soul", .type = SKILL_MOV, .cst = 0, .dmg = 1, .defense = 0,
    .mov = 0, .rng = 1, .link = false, .effect = NULL
};

Card indebted_soul = {
    .name = "Indebted Soul", .type = SKILL_MOV, .cst = 2, .dmg = 2, .defense = 0,
    .mov = 0, .rng = 2, .link = false, .effect = NULL
};

Card broken_soul = {
    .name = "Broken Soul", .type = SKILL_MOV, .cst = 4, .dmg = 3, .defense = 0,
    .mov = 0, .rng = 3, .link = false, .effect = NULL
};

// ------------------- TWIST -------------------

Card ritual_of_pain = { .name = "Ritual of Pain", .type = TWIST, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL };

Card flame_trick = { .name = "Flame's Trick", .type = TWIST, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL };

Card torment_of_fate = { .name = "Torment of Fate", .type = TWIST, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL };

Card indulgent_desire = { .name = "Indulgent Desire", .type = TWIST, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL };

Card devil_gaze = { .name = "Devil's Gaze", .type = TWIST, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL };

Card desire_trick = { .name = "Desire's Trick", .type = TWIST, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL };

// ------------------- EPIC -------------------

Card hell_flame = {
    .name = "Hell Flame", .type = EPIC, .cst = 0, .dmg = 0, .defense = 0,
    .mov = 0, .rng = 1, .link = false, .effect = hell_flame_effect
};

Card doomfall = {
    .name = "Doomfall", .type = EPIC, .cst = 0, .dmg = 0, .defense = 0,
    .mov = 0, .rng = 3, .link = false, .effect = doomfall_effect
};

Card curse_of_greed = {
    .name = "Curse of Greed", .type = EPIC, .cst = 0, .dmg = 0, .defense = 0,
    .mov = 0, .rng = 0, .link = false, .effect = curse_of_greed_effect
};

// ------------------- Fable 初始化 -------------------

Fable match_girl;

void init_match_girl_fable(void) {
    match_girl = (Fable){
        .name = "Match Girl",
        .Piece = {255, 127, 36, 255},
        .health = 36,
        .energy = 0,
        .defense = 0,
        .epic_threshold = 18,
        .lane = 1
    };
    match_girl.skill[0] = (Deck){
        .cards = { &phantom_wish, &secret_desire, &secret_desire, &endless_luxury, &endless_luxury },
        .cnt = 5
    };
    match_girl.skill[1] = (Deck){
        .cards = { &devil_sacrifice, &devil_bet, &devil_bet, &devil_contract, &devil_contract },
        .cnt = 5
    };
    match_girl.skill[2] = (Deck){
        .cards = { &weightless_soul, &indebted_soul, &indebted_soul, &broken_soul, &broken_soul },
        .cnt = 5
    };
    match_girl.epic[0] = hell_flame;
    match_girl.epic[1] = doomfall;
    match_girl.epic[2] = curse_of_greed;
}

// ------------------- 火柴功能 -------------------

Card fire_card_template = {
    .name = "Match", .type = UNIVERSAL, .cst = 0,
    .dmg = 0, .defense = 0, .mov = 0, .rng = 0,
    .link = false, .effect = NULL
};

void init_match_pile(void) {
    match_girl_state.match_pile.cnt = 0;
    for (int i = 0; i < 12; i++) {
        Card *c = malloc(sizeof(Card));
        *c = fire_card_template;
        add_deck(&match_girl_state.match_pile, c);
    }
    printf("🧨 Match pile initialized with 12 matches.\n");
}

void reset_match_placement(Player *p) {
    if (p == match_girl_state.player) {
        match_girl_state.match_placed_this_turn = false;
    }
}

bool is_adjacent(Player *p1, Player *p2) {
    return abs(p1->fable->lane - p2->fable->lane) == 1;
}

bool try_place_match(Player *self, Player *enemy) {
    if (match_girl_state.match_placed_this_turn || !is_adjacent(self, enemy) || match_girl_state.match_pile.cnt == 0)
        return false;

    Card *match = draw_deck(&match_girl_state.match_pile);
    if (!match) return false;

    for (int i = enemy->draw.cnt; i > 0; i--)
        enemy->draw.cards[i] = enemy->draw.cards[i - 1];

    enemy->draw.cards[0] = match;
    enemy->draw.cnt++;

    match_girl_state.match_placed_this_turn = true;
    printf("🪄 Match placed on top of enemy deck.\n");
    return true;
}

void on_card_played(Player *who_played, Card *card) {
    if (!card || strcmp(card->name, "Match") != 0) return;
    if (match_girl_state.player && who_played != match_girl_state.player) {
        match_girl_state.player->fable->energy += 1;
        printf("🔥 Match Girl gains +1 energy from Match.\n");
    }
}

// ------------------- EPIC 效果函式 -------------------

void hell_flame_effect(void *self, void *target) {
    Player *p = (Player *)self;
    int x = (p->fable->energy + 1) / 2;
    printf("🔥 Hell Flame deals %d damage (half of energy)\n", x);
}

void doomfall_effect(void *self, void *target) {
    Player *enemy = (Player *)target;
    int count = 0;
    for (int i = 0; i < 6 && enemy->draw.cnt > 0; i++) {
        Card *top = draw_deck(&enemy->draw);
        add_deck(&enemy->disc, top);
        if (top && strcmp(top->name, "Match") == 0) count++;
    }
    printf("☠️ Doomfall causes %d damage from matches\n", count);
}

void curse_of_greed_effect(void *self, void *target) {
    Player *enemy = (Player *)target;
    printf("💀 Curse of Greed places 3 matches on enemy deck top\n");
    for (int i = 0; i < 3; i++) {
        if (match_girl_state.match_pile.cnt == 0) break;
        Card *match = draw_deck(&match_girl_state.match_pile);
        for (int j = enemy->draw.cnt; j > 0; j--) {
            enemy->draw.cards[j] = enemy->draw.cards[j - 1];
        }
        enemy->draw.cards[0] = match;
        enemy->draw.cnt++;
    }
}
