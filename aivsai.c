#include <stdio.h>
#include <stdlib.h>
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

// 所有角色與對應 AI 函式
extern Fable alice_fable, red_hood_fable, mulan_fable, snow_white_fable, kaguya_fable, match_girl_enhanced_fable;

void ai_alice_turn(Player*, Player*, FableShop*);
void ai_redhood_turn(Player*, Player*, FableShop*);
void ai_mulan_turn(Player*, Player*, FableShop*);
void ai_snowwhite_turn(Player*, Player*, FableShop*);
void ai_kaguya_turn(Player*, Player*, FableShop*);
void ai_match_girl_turn(Player*, Player*, FableShop*);

int main() {
    srand(time(NULL));

    Fable *fables[] = {
        &alice_fable, &red_hood_fable, &mulan_fable,
        &snow_white_fable, &kaguya_fable, &match_girl_enhanced_fable
    };

    void (*ai_funcs[])(Player*, Player*, FableShop*) = {
        ai_alice_turn, ai_redhood_turn, ai_mulan_turn,
        ai_snowwhite_turn, ai_kaguya_turn, ai_match_girl_turn
    };

    // 隨機選角，避免重複
    int idx1 = rand() % 6;
    int idx2;
    do {
        idx2 = rand() % 6;
    } while (idx2 == idx1);

    // 建立玩家
    Player p1 = {0}, p2 = {0};
    p1.fable = fables[idx1];
    p1.ai_func = ai_funcs[idx1];
    p1.health = p1.fable->health;
    p1.lane = 0;
    p1.energy = 4;

    p2.fable = fables[idx2];
    p2.ai_func = ai_funcs[idx2];
    p2.health = p2.fable->health;
    p2.lane = 1;
    p2.energy = 5;

    FableShop shop = {0};

    printf("🤖 AI 1: %s\n", p1.fable->name);
    printf("🤖 AI 2: %s\n", p2.fable->name);

    int round = 1;
    while (p1.health > 0 && p2.health > 0) {
        printf("\n=== Round %d ===\n", round++);
        printf("[AI 1] %s's turn (HP: %d, EN: %d)\n", p1.fable->name, p1.health, p1.energy);
        p1.ai_func(&p1, &p2, &shop);
        if (p2.health <= 0) break;

        printf("[AI 2] %s's turn (HP: %d, EN: %d)\n", p2.fable->name, p2.health, p2.energy);
        p2.ai_func(&p2, &p1, &shop);
    }

    printf("\n🎉 Winner: %s\n", (p1.health > 0) ? p1.fable->name : p2.fable->name);
    return 0;
}
