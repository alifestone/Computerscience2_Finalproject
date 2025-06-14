#include "base.h"
#include "cards.h"
#include <stdio.h>

//=============================================================================
// PLACEHOLDER EFFECT FUNCTIONS
// These provide basic implementations for all the missing effect functions
// referenced in the character card definitions
//=============================================================================

//=============================================================================
// ALICE EFFECT FUNCTIONS
//=============================================================================

void twisting_game_effect(void* self, void* target) {
    Player* alice = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Alice uses Twisting Game\n");
    if (opponent) {
        opponent->health -= 2; // Basic damage
    }
}

void controlling_game_effect(void* self, void* target) {
    Player* alice = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Alice uses Controlling Game\n");
    if (opponent) {
        opponent->health -= 3; // Basic damage
    }
}

void mental_illusion_effect(void* self, void* target) {
    Player* alice = (Player*)self;
    
    printf("Alice uses Mental Illusion\n");
    if (alice) {
        alice->defense = MIN(alice->defense + 2, alice->fable->defense);
    }
}

void hat_trick_effect(void* self, void* target) {
    Player* alice = (Player*)self;
    
    printf("Alice uses Hat Trick\n");
    if (alice) {
        alice->defense = MIN(alice->defense + 3, alice->fable->defense);
    }
}

void strange_stealth_effect(void* self, void* target) {
    Player* alice = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Alice uses Strange Stealth\n");
    if (opponent) {
        opponent->health -= 2;
    }
    if (alice) {
        draw_hand(alice, 2); // Draw cards
    }
}

void strange_weird_effect(void* self, void* target) {
    Player* alice = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Alice uses Strange Weird\n");
    if (opponent) {
        opponent->health -= 3;
    }
    if (alice) {
        draw_hand(alice, 3); // Draw cards
    }
}

//=============================================================================
// KAGUYA EFFECT FUNCTIONS
//=============================================================================

void enlightenment_glory_effect(void* self, void* target) {
    Player* kaguya = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Kaguya uses Enlightenment Glory\n");
    if (opponent) {
        int damage = (kaguya && kaguya->defense >= 3) ? 3 : 2;
        opponent->health -= damage;
    }
}

void enlightenment_avatar_effect(void* self, void* target) {
    Player* kaguya = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Kaguya uses Enlightenment Avatar\n");
    if (opponent) {
        int damage = (kaguya && kaguya->defense >= 3) ? 4 : 3;
        opponent->health -= damage;
    }
}

//=============================================================================
// RED HOOD EFFECT FUNCTIONS
//=============================================================================

void electrified_shield_enhanced_effect(void* self, void* target) {
    Player* red_hood = (Player*)self;
    
    printf("Red Hood uses Electrified Shield\n");
    if (red_hood) {
        red_hood->defense = MIN(red_hood->defense + 4, red_hood->fable->defense);
    }
}

void omega_shield_enhanced_effect(void* self, void* target) {
    Player* red_hood = (Player*)self;
    
    printf("Red Hood uses Omega Shield\n");
    if (red_hood) {
        red_hood->defense = MIN(red_hood->defense + 6, red_hood->fable->defense);
    }
}

void unleashed_firepower_enhanced_effect(void* self, void* target) {
    Player* red_hood = (Player*)self;
    
    printf("Red Hood uses Unleashed Firepower\n");
    // Enhanced movement effect - placeholder
}

void unleashed_fury_enhanced_effect(void* self, void* target) {
    Player* red_hood = (Player*)self;
    
    printf("Red Hood uses Unleashed Fury\n");
    // Enhanced movement effect - placeholder
}

//=============================================================================
// MATCH GIRL EFFECT FUNCTIONS
//=============================================================================

void secret_desire_enhanced_effect(void* self, void* target) {
    Player* match_girl = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Match Girl uses Secret Desire\n");
    if (opponent) {
        opponent->health -= 2;
    }
}

void endless_luxury_enhanced_effect(void* self, void* target) {
    Player* match_girl = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Match Girl uses Endless Luxury\n");
    if (opponent) {
        opponent->health -= 3;
    }
}

void devil_bet_enhanced_effect(void* self, void* target) {
    Player* match_girl = (Player*)self;
    
    printf("Match Girl uses Devil's Bet\n");
    if (match_girl) {
        match_girl->defense = MIN(match_girl->defense + 1, match_girl->fable->defense);
    }
}

void devil_contract_enhanced_effect(void* self, void* target) {
    Player* match_girl = (Player*)self;
    
    printf("Match Girl uses Devil's Contract\n");
    if (match_girl) {
        match_girl->defense = MIN(match_girl->defense + 1, match_girl->fable->defense);
    }
}

void indebted_soul_enhanced_effect(void* self, void* target) {
    Player* match_girl = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Match Girl uses Indebted Soul\n");
    if (opponent) {
        opponent->health -= 2;
    }
}

void broken_soul_enhanced_effect(void* self, void* target) {
    Player* match_girl = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Match Girl uses Broken Soul\n");
    if (opponent) {
        opponent->health -= 3;
    }
}

//=============================================================================
// MULAN EFFECT FUNCTIONS
//=============================================================================

void unstoppable_effect(void* self, void* target) {
    Player* mulan = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Mulan uses Unstoppable\n");
    if (opponent) {
        opponent->health -= 2;
    }
}

void indestructible_effect(void* self, void* target) {
    Player* mulan = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Mulan uses Indestructible\n");
    if (opponent) {
        opponent->health -= 3;
    }
}

void soft_overcome_hard_effect(void* self, void* target) {
    Player* mulan = (Player*)self;
    
    printf("Mulan uses Soft Overcome Hard\n");
    if (mulan) {
        mulan->defense = MIN(mulan->defense + 2, mulan->fable->defense);
    }
}

void weak_defeat_strong_effect(void* self, void* target) {
    Player* mulan = (Player*)self;
    
    printf("Mulan uses Weak Defeat Strong\n");
    if (mulan) {
        mulan->defense = MIN(mulan->defense + 3, mulan->fable->defense);
    }
}

void no_mercy_effect(void* self, void* target) {
    Player* mulan = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Mulan uses No Mercy\n");
    if (opponent) {
        opponent->health -= 2;
        // Move opponent
        opponent->pos += (mulan->pos < opponent->pos) ? 1 : -1;
        opponent->pos = MAX(-4, MIN(4, opponent->pos));
    }
}

void never_forgive_effect(void* self, void* target) {
    Player* mulan = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Mulan uses Never Forgive\n");
    if (opponent) {
        opponent->health -= 3;
        // Move opponent
        opponent->pos += (mulan->pos < opponent->pos) ? 1 : -1;
        opponent->pos = MAX(-4, MIN(4, opponent->pos));
    }
}

//=============================================================================
// SNOW WHITE EFFECT FUNCTIONS
//=============================================================================

void tainted_feast_effect(void* self, void* target) {
    Player* snow_white = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Snow White uses Tainted Feast\n");
    if (opponent) {
        opponent->health -= 2;
    }
}

void tainted_revelry_effect(void* self, void* target) {
    Player* snow_white = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Snow White uses Tainted Revelry\n");
    if (opponent) {
        opponent->health -= 3;
    }
}

void broken_reality_effect(void* self, void* target) {
    Player* snow_white = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Snow White uses Broken Reality\n");
    if (opponent) {
        opponent->health -= 2;
    }
    // Move to adjacent position
    if (snow_white && opponent) {
        snow_white->pos = opponent->pos + ((snow_white->pos < opponent->pos) ? -1 : 1);
        snow_white->pos = MAX(-4, MIN(4, snow_white->pos));
    }
}

void broken_destiny_effect(void* self, void* target) {
    Player* snow_white = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Snow White uses Broken Destiny\n");
    if (opponent) {
        opponent->health -= 3;
    }
    // Move to adjacent position
    if (snow_white && opponent) {
        snow_white->pos = opponent->pos + ((snow_white->pos < opponent->pos) ? -1 : 1);
        snow_white->pos = MAX(-4, MIN(4, snow_white->pos));
    }
}
