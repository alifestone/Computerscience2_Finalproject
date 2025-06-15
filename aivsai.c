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

extern Fable alice_fable, red_hood_fable, mulan_fable, snow_white_fable, kaguya_fable, match_girl_enhanced_fable;

int main() {
    srand(time(NULL));

    Fable *fables[] = {
        &alice_fable, &red_hood_fable, &mulan_fable,
        &snow_white_fable, &kaguya_fable, &match_girl_enhanced_fable
    };

    const char *names[] = {
        "Alice", "Red Hood", "Mulan", "Snow White", "Kaguya", "Match Girl"
    };

    int idx1 = rand() % 6;
    int idx2;
    do {
        idx2 = rand() % 6;
    } while (idx2 == idx1);

    Player p1 = {0}, p2 = {0};
    p1.fable = fables[idx1];
    p1.health = p1.fable->health;
    p1.lane = 0;
    p1.energy = 4;

    p2.fable = fables[idx2];
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

        if (strcmp(p1.fable->name, "Alice") == 0) ai_alice_turn(&p1, &p2, &shop);
        else if (strcmp(p1.fable->name, "Red Hood") == 0) ai_redhood_turn(&p1, &p2, &shop);
        else if (strcmp(p1.fable->name, "Mulan") == 0) ai_mulan_turn(&p1, &p2, &shop);
        else if (strcmp(p1.fable->name, "Snow White") == 0) ai_snowwhite_turn(&p1, &p2, &shop);
        else if (strcmp(p1.fable->name, "Kaguya") == 0) ai_kaguya_turn(&p1, &p2, &shop);
        else if (strcmp(p1.fable->name, "Match Girl") == 0) ai_match_girl_turn(&p1, &p2, &shop);

        if (p2.health <= 0) break;

        printf("[AI 2] %s's turn (HP: %d, EN: %d)\n", p2.fable->name, p2.health, p2.energy);

        if (strcmp(p2.fable->name, "Alice") == 0) ai_alice_turn(&p2, &p1, &shop);
        else if (strcmp(p2.fable->name, "Red Hood") == 0) ai_redhood_turn(&p2, &p1, &shop);
        else if (strcmp(p2.fable->name, "Mulan") == 0) ai_mulan_turn(&p2, &p1, &shop);
        else if (strcmp(p2.fable->name, "Snow White") == 0) ai_snowwhite_turn(&p2, &p1, &shop);
        else if (strcmp(p2.fable->name, "Kaguya") == 0) ai_kaguya_turn(&p2, &p1, &shop);
        else if (strcmp(p2.fable->name, "Match Girl") == 0) ai_match_girl_turn(&p2, &p1, &shop);

        printf("\nPress Enter to continue to next round...");
        getchar();
    }

    printf("\n🎉 Winner: %s\n", (p1.health > 0) ? p1.fable->name : p2.fable->name);
    return 0;
}
