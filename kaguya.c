#include "kaguya.h"
#include "cards.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Kaguya's system demonstrates FILTERING ALGORITHMS and CONDITIONAL STATE MANAGEMENT
// This teaches us crucial programming concepts:
// 1. Filtering and searching algorithms - finding specific cards in collections
// 2. Conditional effects - abilities that trigger based on game state
// 3. Resource cap management - dynamic limits that change during play
// 4. Defensive programming - building systems that protect and enhance
// 5. Delayed effects - actions that trigger in future turns

// Kaguya's Defense System - Dynamic defense caps and interactions
typedef struct DefenseSystem {
    int base_defense_cap;          // Starting defense limit
    int current_defense_cap;       // Current maximum defense
    int defense_cap_bonuses;       // Temporary increases to cap
    bool punishment_mode_active;   // When defense > opponent's defense
    int consecutive_high_defense_turns; // Tracks defensive momentum
} DefenseSystem;

// Card Filtering System - Kaguya's signature mechanic
typedef struct FilteringSystem {
    int cards_filtered_this_turn;  // Track filtering usage
    int max_filters_per_turn;      // Balance limitation
    bool divine_call_active;        // Enhanced filtering mode
    Card* filtered_cards_cache[10]; // Cache for UI display
    int cache_count;               // Number of cards in cache
} FilteringSystem;

// Moonlight System - Represents Kaguya's celestial power
typedef struct MoonlightSystem {
    int moonlight_phases;          // Current phase (0-3, like moon phases)
    int defense_per_phase;         // Defense gained each turn end
    bool blood_moon_active;        // Special twist effect
    int lunar_cycle_progress;      // Tracks progression through cycles
} MoonlightSystem;

// Kaguya's complete state - combines all defensive systems
typedef struct KaguyaState {
    DefenseSystem defense;
    FilteringSystem filtering;
    MoonlightSystem moonlight;
    bool contemplation_active;     // From twist cards
    int purification_stacks;       // From movement abilities
    bool invulnerability_active;   // From epic cards
} KaguyaState;

// Forward declarations for Kaguya's card effects
void enlightenment_light_effect(void* self, void* target);
void enlightenment_glory_effect(void* self, void* target);
void enlightenment_avatar_effect(void* self, void* target);
void confusing_echo_effect(void* self, void* target);
void ancient_resonance_effect(void* self, void* target);
void divine_summon_effect(void* self, void* target);
void focused_introspection_effect(void* self, void* target);
void enlightened_resolve_effect(void* self, void* target);
void painful_purification_effect(void* self, void* target);

// Epic effects
void scorching_bamboo_effect(void* self, void* target);
void destined_judgment_effect(void* self, void* target);
void restless_blood_effect(void* self, void* target);

//=============================================================================
// CARD FILTERING ALGORITHMS
// These demonstrate how to implement search and filter operations on card collections
//=============================================================================

typedef enum {
    FILTER_DEFENSE_CARDS,    // Look for defense cards specifically
    FILTER_BY_LEVEL,         // Filter by card level/value
    FILTER_BY_TYPE,          // Filter by card type
    FILTER_BY_COST           // Filter by energy cost
} FilterType;

// Core filtering algorithm - this is a fundamental programming skill
bool card_matches_filter(Card* card, FilterType filter, int parameter) {
    if (!card) return false;
    
    switch (filter) {
        case FILTER_DEFENSE_CARDS:
            return (card->type == BASIC_DEF || card->type == SKILL_DEF);
            
        case FILTER_BY_LEVEL:
            return (card->val == parameter);
            
        case FILTER_BY_TYPE:
            return (card->type == parameter);
            
        case FILTER_BY_COST:
            return (card->cst <= parameter);
            
        default:
            return false;
    }
}

// Advanced filtering with multiple criteria - real-world programming pattern
int filter_deck_advanced(Deck* source_deck, Card* results[], int max_results, 
                        FilterType filter, int parameter, bool remove_from_source) {
    if (!source_deck || !results) return 0;
    
    int found_count = 0;
    
    // Search through deck from top to bottom
    for (int i = 0; i < source_deck->cnt && found_count < max_results; i++) {
        Card* card = source_deck->cards[i];
        
        if (card_matches_filter(card, filter, parameter)) {
            results[found_count] = card;
            found_count++;
            
            if (remove_from_source) {
                // Remove from source deck
                for (int j = i; j < source_deck->cnt - 1; j++) {
                    source_deck->cards[j] = source_deck->cards[j + 1];
                }
                source_deck->cnt--;
                i--; // Adjust index since we removed a card
            }
        }
    }
    
    return found_count;
}

// Kaguya's specific filtering mechanic - show cards and let player choose
void kaguya_filter_and_choose(Player* player, int cards_to_show, FilterType filter) {
    if (!player) return;
    
    KaguyaState* kaguya_state = get_kaguya_state(player);
    if (!kaguya_state) return;
    
    // Check filtering limits
    if (kaguya_state->filtering.cards_filtered_this_turn >= 
        kaguya_state->filtering.max_filters_per_turn) {
        printf("Maximum filtering uses reached this turn\n");
        return;
    }
    
    printf("Kaguya examines the top %d cards of her deck...\n", cards_to_show);
    
    // Look at top cards
    Card* revealed_cards[10];
    int revealed_count = 0;
    
    for (int i = 0; i < cards_to_show && i < player->draw.cnt; i++) {
        revealed_cards[revealed_count] = player->draw.cards[player->draw.cnt - 1 - i];
        revealed_count++;
    }
    
    // Filter for defense cards
    Card* defense_cards[10];
    int defense_count = 0;
    
    for (int i = 0; i < revealed_count; i++) {
        if (card_matches_filter(revealed_cards[i], filter, 0)) {
            defense_cards[defense_count] = revealed_cards[i];
            defense_count++;
        }
    }
    
    // Add filtered cards to hand
    for (int i = 0; i < defense_count; i++) {
        add_deck(&player->hand, defense_cards[i]);
        
        // Remove from deck
        for (int j = 0; j < player->draw.cnt; j++) {
            if (player->draw.cards[j] == defense_cards[i]) {
                for (int k = j; k < player->draw.cnt - 1; k++) {
                    player->draw.cards[k] = player->draw.cards[k + 1];
                }
                player->draw.cnt--;
                break;
            }
        }
    }
    
    // Handle remaining cards (player choice: discard or return to deck)
    // Simplified: discard non-defense cards
    for (int i = 0; i < revealed_count; i++) {
        bool is_defense = false;
        for (int j = 0; j < defense_count; j++) {
            if (revealed_cards[i] == defense_cards[j]) {
                is_defense = true;
                break;
            }
        }
        
        if (!is_defense) {
            add_deck(&player->disc, revealed_cards[i]);
            // Remove from deck
            for (int j = 0; j < player->draw.cnt; j++) {
                if (player->draw.cards[j] == revealed_cards[i]) {
                    for (int k = j; k < player->draw.cnt - 1; k++) {
                        player->draw.cards[k] = player->draw.cards[k + 1];
                    }
                    player->draw.cnt--;
                    break;
                }
            }
        }
    }
    
    kaguya_state->filtering.cards_filtered_this_turn++;
    printf("Kaguya found %d defense cards and added them to hand\n", defense_count);
}

//=============================================================================
// DEFENSE SYSTEM MANAGEMENT
// Shows how to implement dynamic caps and conditional bonuses
//=============================================================================

void init_defense_system(DefenseSystem* defense) {
    if (!defense) return;
    
    defense->base_defense_cap = 6;
    defense->current_defense_cap = 6;
    defense->defense_cap_bonuses = 0;
    defense->punishment_mode_active = false;
    defense->consecutive_high_defense_turns = 0;
}

void increase_defense_cap(KaguyaState* kaguya_state, int amount, const char* source) {
    if (!kaguya_state) return;
    
    kaguya_state->defense.defense_cap_bonuses += amount;
    kaguya_state->defense.current_defense_cap = kaguya_state->defense.base_defense_cap + 
                                               kaguya_state->defense.defense_cap_bonuses;
    
    printf("Kaguya's defense cap increased by %d from %s (now %d)\n", 
           amount, source, kaguya_state->defense.current_defense_cap);
}

bool check_punishment_mode(Player* kaguya_player, Player* opponent) {
    if (!kaguya_player || !opponent) return false;
    
    KaguyaState* kaguya_state = get_kaguya_state(kaguya_player);
    if (!kaguya_state) return false;
    
    bool was_active = kaguya_state->defense.punishment_mode_active;
    kaguya_state->defense.punishment_mode_active = (kaguya_player->defense > opponent->defense);
    
    if (kaguya_state->defense.punishment_mode_active && !was_active) {
        printf("Kaguya enters Punishment Mode - her defense exceeds opponent's!\n");
    } else if (!kaguya_state->defense.punishment_mode_active && was_active) {
        printf("Kaguya's Punishment Mode ends\n");
    }
    
    return kaguya_state->defense.punishment_mode_active;
}

//=============================================================================
// KAGUYA'S CARD DEFINITIONS
// Each card demonstrates different aspects of filtering and defense
//=============================================================================

// Attack cards that scale with defense
Card enlightenment_light = {
    .name = "Enlightenment Light", .type = SKILL_ATK, .val = 1, .cst = 0, .dmg = 1,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = enlightenment_light_effect
};

Card enlightenment_glory = {
    .name = "Enlightenment Glory", .type = SKILL_ATK, .val = 2, .cst = 2, .dmg = 2,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = enlightenment_glory_effect
};

Card enlightenment_avatar = {
    .name = "Enlightenment Avatar", .type = SKILL_ATK, .val = 3, .cst = 4, .dmg = 3,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = enlightenment_avatar_effect
};

// Defense cards with filtering mechanics
Card confusing_echo = {
    .name = "Confusing Echo", .type = SKILL_DEF, .val = 1, .cst = 1, .dmg = 0,
    .defense = 1, .mov = 0, .rng = 0, .link = false, .effect = confusing_echo_effect
};

Card ancient_resonance = {
    .name = "Ancient Resonance", .type = SKILL_DEF, .val = 2, .cst = 2, .dmg = 0,
    .defense = 2, .mov = 0, .rng = 0, .link = false, .effect = ancient_resonance_effect
};

Card divine_summon = {
    .name = "Divine Summon", .type = SKILL_DEF, .val = 3, .cst = 4, .dmg = 0,
    .defense = 3, .mov = 0, .rng = 0, .link = false, .effect = divine_summon_effect
};

// Movement cards with purification mechanics
Card focused_introspection = {
    .name = "Focused Introspection", .type = SKILL_MOV, .val = 1, .cst = 0, .dmg = 1,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = focused_introspection_effect
};

Card enlightened_resolve = {
    .name = "Enlightened Resolve", .type = SKILL_MOV, .val = 2, .cst = 2, .dmg = 2,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = enlightened_resolve_effect
};

Card painful_purification = {
    .name = "Painful Purification", .type = SKILL_MOV, .val = 3, .cst = 4, .dmg = 3,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = painful_purification_effect
};

// Twist cards that modify defense system
Card punishment_moment = {
    .name = "Punishment Moment", .type = TWIST, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL
};

Card moonlight_contemplation = {
    .name = "Moonlight Contemplation", .type = TWIST, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL
};

Card blood_moon = {
    .name = "Blood Moon", .type = TWIST, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL
};

Card master_destiny = {
    .name = "Master Destiny", .type = TWIST, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL
};

// Epic cards with ultimate effects
Card scorching_bamboo = {
    .name = "Scorching Bamboo", .type = EPIC, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = scorching_bamboo_effect
};

Card destined_judgment = {
    .name = "Destined Judgment", .type = EPIC, .val = 0, .cst = 0, .dmg = 0,
    .defense = 6, .mov = 0, .rng = 0, .link = false, .effect = destined_judgment_effect
};

Card restless_blood = {
    .name = "Restless Blood", .type = EPIC, .val = 0, .cst = 0, .dmg = 3,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = restless_blood_effect
};

Fable kaguya_fable;

//=============================================================================
// KAGUYA STATE MANAGEMENT
// Shows how to coordinate defensive and filtering systems
//=============================================================================

KaguyaState* get_kaguya_state(Player* player) {
    return (KaguyaState*)player->fable->character_state;
}

void init_kaguya_state(Player* player) {
    if (!player || !player->fable) return;
    
    KaguyaState* kaguya_state = malloc(sizeof(KaguyaState));
    memset(kaguya_state, 0, sizeof(KaguyaState));
    
    // Initialize defensive systems
    init_defense_system(&kaguya_state->defense);
    
    // Initialize filtering system
    kaguya_state->filtering.max_filters_per_turn = 3; // Balanced limit
    kaguya_state->filtering.cards_filtered_this_turn = 0;
    kaguya_state->filtering.divine_call_active = false;
    kaguya_state->filtering.cache_count = 0;
    
    // Initialize moonlight system
    kaguya_state->moonlight.moonlight_phases = 0;
    kaguya_state->moonlight.defense_per_phase = 2;
    kaguya_state->moonlight.blood_moon_active = false;
    kaguya_state->moonlight.lunar_cycle_progress = 0;
    
    // Initialize other systems
    kaguya_state->contemplation_active = false;
    kaguya_state->purification_stacks = 0;
    kaguya_state->invulnerability_active = false;
    
    player->fable->character_state = kaguya_state;
    
    printf("Kaguya's celestial defense systems initialized\n");
}

//=============================================================================
// TURN MANAGEMENT WITH CONDITIONAL EFFECTS
// Shows how to implement effects that trigger based on game state
//=============================================================================

void kaguya_turn_start(Player* player) {
    KaguyaState* kaguya_state = get_kaguya_state(player);
    if (!kaguya_state) return;
    
    // Reset turn-based counters
    kaguya_state->filtering.cards_filtered_this_turn = 0;
    
    // Check for moonlight contemplation effect (turn end defense)
    if (kaguya_state->contemplation_active) {
        int defense_gain = kaguya_state->moonlight.defense_per_phase;
        player->defense = MIN(player->defense + defense_gain, 
                             kaguya_state->defense.current_defense_cap);
        printf("Moonlight Contemplation: Kaguya gains %d defense\n", defense_gain);
    }
    
    printf("Kaguya begins turn - filtering available, defense: %d/%d\n",
           player->defense, kaguya_state->defense.current_defense_cap);
}

void kaguya_turn_end(Player* player) {
    KaguyaState* kaguya_state = get_kaguya_state(player);
    if (!kaguya_state) return;
    
    // Track consecutive high defense turns
    if (player->defense >= 3) {
        kaguya_state->defense.consecutive_high_defense_turns++;
        printf("Kaguya maintains high defense (streak: %d)\n", 
               kaguya_state->defense.consecutive_high_defense_turns);
    } else {
        kaguya_state->defense.consecutive_high_defense_turns = 0;
    }
    
    // Blood moon effect - reset every 3 defense
    if (kaguya_state->moonlight.blood_moon_active && player->defense >= 3) {
        draw_hand(player, 1);
        printf("Blood Moon: Kaguya draws a card for high defense\n");
    }
    
    // Advance lunar cycle
    kaguya_state->moonlight.lunar_cycle_progress++;
    if (kaguya_state->moonlight.lunar_cycle_progress >= 4) {
        kaguya_state->moonlight.lunar_cycle_progress = 0;
        kaguya_state->moonlight.moonlight_phases = 
            (kaguya_state->moonlight.moonlight_phases + 1) % 4;
        printf("Lunar cycle advances to phase %d\n", kaguya_state->moonlight.moonlight_phases);
    }
}

void kaguya_on_damage_taken(Player* kaguya_player, int damage, Player* source) {
    if (!kaguya_player || damage <= 0) return;
    
    KaguyaState* kaguya_state = get_kaguya_state(kaguya_player);
    if (!kaguya_state) return;
    
    // Reset consecutive defense streak
    kaguya_state->defense.consecutive_high_defense_turns = 0;
    
    printf("Kaguya's defensive focus is disrupted by %d damage\n", damage);
}

//=============================================================================
// KAGUYA'S CARD EFFECT IMPLEMENTATIONS
// These demonstrate conditional effects and filtering mechanics
//=============================================================================

void enlightenment_light_effect(void* self, void* target) {
    Player* kaguya = (Player*)self;
    Player* opponent = (Player*)target;
    KaguyaState* kaguya_state = get_kaguya_state(kaguya);
    
    // "Range 1, Damage 1+O, if you have 3+ defense, damage +1"
    
    int base_damage = 1;
    int bonus_damage = (kaguya->defense >= 3) ? 1 : 0;
    int total_damage = base_damage + bonus_damage;
    
    opponent->health -= total_damage;
    
    if (bonus_damage > 0) {
        printf("Enlightenment Light enhanced by high defense! Total damage: %d\n", total_damage);
    } else {
        printf("Enlightenment Light deals %d damage\n", total_damage);
    }
}

void confusing_echo_effect(void* self, void* target) {
    Player* kaguya = (Player*)self;
    KaguyaState* kaguya_state = get_kaguya_state(kaguya);
    
    // "Defense 1+O, show top 1 card of deck"
    // "If it's a defense card (not universal), add to hand"
    // "Otherwise, discard or put back on top"
    
    int defense_gain = 1;
    kaguya->defense = MIN(kaguya->defense + defense_gain, 
                         kaguya_state->defense.current_defense_cap);
    
    // Filter one card from deck top
    kaguya_filter_and_choose(kaguya, 1, FILTER_DEFENSE_CARDS);
    
    printf("Confusing Echo: +%d defense and deck filtering\n", defense_gain);
}

void ancient_resonance_effect(void* self, void* target) {
    Player* kaguya = (Player*)self;
    KaguyaState* kaguya_state = get_kaguya_state(kaguya);
    
    // "Defense 2+O, show top 2 cards of deck"
    // Enhanced filtering for higher level
    
    int defense_gain = 2;
    kaguya->defense = MIN(kaguya->defense + defense_gain, 
                         kaguya_state->defense.current_defense_cap);
    
    kaguya_filter_and_choose(kaguya, 2, FILTER_DEFENSE_CARDS);
    
    printf("Ancient Resonance: Enhanced filtering and defense\n");
}

void divine_summon_effect(void* self, void* target) {
    Player* kaguya = (Player*)self;
    KaguyaState* kaguya_state = get_kaguya_state(kaguya);
    
    // "Defense 3+O, show top 3 cards of deck"
    // Maximum filtering power
    
    int defense_gain = 3;
    kaguya->defense = MIN(kaguya->defense + defense_gain, 
                         kaguya_state->defense.current_defense_cap);
    
    kaguya_filter_and_choose(kaguya, 3, FILTER_DEFENSE_CARDS);
    
    printf("Divine Summon: Maximum deck filtering power\n");
}

void focused_introspection_effect(void* self, void* target) {
    Player* kaguya = (Player*)self;
    Player* opponent = (Player*)target;
    KaguyaState* kaguya_state = get_kaguya_state(kaguya);
    
    // "Range O, Damage 1, can lose 1 life to remove 1 card from hand/discard"
    
    int damage = 1;
    opponent->health -= damage;
    
    // Option to purify by losing life
    if (kaguya->health > 1) {
        // Simplified: always use purification if available
        kaguya->health -= 1;
        kaguya_state->purification_stacks++;
        
        // Remove a card from hand (simplified choice)
        if (kaguya->hand.cnt > 0) {
            Card* removed = kaguya->hand.cards[kaguya->hand.cnt - 1];
            kaguya->hand.cnt--;
            // Card is completely removed from game
            printf("Focused Introspection: Kaguya purifies by removing %s\n", removed->name);
        }
    }
    
    printf("Focused Introspection: Damage and purification\n");
}

void enlightened_resolve_effect(void* self, void* target) {
    Player* kaguya = (Player*)self;
    Player* opponent = (Player*)target;
    KaguyaState* kaguya_state = get_kaguya_state(kaguya);
    
    // "Range O, Damage 2, can lose 1 life to remove 1 card from hand/discard"
    // "Persistent: Next turn start, if opponent beyond range 4-O, deal 4 damage"
    
    int damage = 2;
    opponent->health -= damage;
    
    // Set up persistent effect for next turn
    // In full implementation, would add to effect system
    printf("Enlightened Resolve: Persistent effect set for next turn\n");
    
    // Purification option
    if (kaguya->health > 1) {
        kaguya->health -= 1;
        kaguya_state->purification_stacks++;
        printf("Enlightened Resolve: Purification through sacrifice\n");
    }
}

void painful_purification_effect(void* self, void* target) {
    Player* kaguya = (Player*)self;
    Player* opponent = (Player*)target;
    KaguyaState* kaguya_state = get_kaguya_state(kaguya);
    
    // "Range O, Damage 3, can lose 1 life to remove 1 card from hand/discard"
    // "Persistent: Next turn start, if opponent beyond range 4-O, deal 6 damage"
    
    int damage = 3;
    opponent->health -= damage;
    
    // Maximum purification power
    if (kaguya->health > 1) {
        kaguya->health -= 1;
        kaguya_state->purification_stacks += 2; // Extra purification for high level
        printf("Painful Purification: Maximum purification achieved\n");
    }
    
    printf("Painful Purification: Ultimate range punishment prepared\n");
}

void scorching_bamboo_effect(void* self, void* target) {
    Player* kaguya = (Player*)self;
    KaguyaState* kaguya_state = get_kaguya_state(kaguya);
    
    // "Persistent: Until next turn start, you don't take damage or lose life"
    
    kaguya_state->invulnerability_active = true;
    
    printf("Scorching Bamboo: Kaguya becomes invulnerable until next turn!\n");
    // In full implementation, would set up persistent effect
}

void destined_judgment_effect(void* self, void* target) {
    Player* kaguya = (Player*)self;
    Player* opponent = (Player*)target;
    KaguyaState* kaguya_state = get_kaguya_state(kaguya);
    
    // "Defense 6, Persistent: Next turn start, if opponent's defense < yours"
    // "They lose X life where X = defense difference"
    
    kaguya->defense = MIN(kaguya->defense + 6, kaguya_state->defense.current_defense_cap);
    
    // Set up judgment for next turn
    printf("Destined Judgment: Divine judgment prepared based on defense comparison\n");
    
    // Immediate check for demonstration
    if (opponent->defense < kaguya->defense) {
        int life_loss = kaguya->defense - opponent->defense;
        printf("Judgment preview: Would deal %d life loss next turn\n", life_loss);
    }
}

void restless_blood_effect(void* self, void* target) {
    Player* kaguya = (Player*)self;
    Player* opponent = (Player*)target;
    KaguyaState* kaguya_state = get_kaguya_state(kaguya);
    
    // "Move opponent adjacent and deal 3 damage"
    // "Persistent: Next turn start, can move opponent adjacent and deal 3 damage"
    
    // Move opponent adjacent
    opponent->pos = kaguya->pos + ((opponent->pos < kaguya->pos) ? -1 : 1);
    opponent->pos = MAX(-4, MIN(4, opponent->pos));
    
    // Deal damage
    opponent->health -= 3;
    
    printf("Restless Blood: Opponent repositioned and damaged, effect continues!\n");
}

//=============================================================================
// KAGUYA CHARACTER INITIALIZATION
// Shows how to set up a defense-focused character with filtering capabilities
//=============================================================================

void init_kaguya_fable(void) {
    kaguya_fable = (Fable){
        .name = "Kaguya",
        .Piece = {200, 200, 255, 255}, // Soft blue for moonlight
        .health = 32,
        .energy = 0,
        .defense = 6, // High base defense cap
        .epic_threshold = 16,
        .lane = 0
    };
    
    // Set up skill decks emphasizing defense and filtering
    kaguya_fable.skill[0] = (Deck){ // Attack skills that scale with defense
        .cards = {&enlightenment_light, &enlightenment_glory, &enlightenment_glory, 
                 &enlightenment_avatar, &enlightenment_avatar},
        .cnt = 5
    };
    
    kaguya_fable.skill[1] = (Deck){ // Defense skills with filtering
        .cards = {&confusing_echo, &ancient_resonance, &ancient_resonance, 
                 &divine_summon, &divine_summon},
        .cnt = 5
    };
    
    kaguya_fable.skill[2] = (Deck){ // Movement skills with purification
        .cards = {&focused_introspection, &enlightened_resolve, &enlightened_resolve, 
                 &painful_purification, &painful_purification},
        .cnt = 5
    };
    
    // Epic cards with ultimate defensive effects
    kaguya_fable.epic[0] = scorching_bamboo;
    kaguya_fable.epic[1] = destined_judgment;
    kaguya_fable.epic[2] = restless_blood;
}

void setup_kaguya_player(Player* player) {
    if (!player) return;
    
    init_kaguya_fable();
    player->fable = &kaguya_fable;
    player->health = kaguya_fable.health;
    player->power = 0;
    player->defense = 0;
    player->pos = kaguya_fable.lane;
    
    // Initialize Kaguya's defensive systems
    init_kaguya_state(player);
    
    // Set up starting deck with extra defense cards for filtering synergy
    for (int i = 0; i < 3; i++) {
        add_deck(&player->draw, &Attack1);
        add_deck(&player->draw, &Defense1);
        add_deck(&player->draw, &Move1);
    }
    
    // Add extra defense cards for filtering
    add_deck(&player->draw, &Defense1);
    add_deck(&player->draw, &Defense2);
    
    // Add starting skills
    add_deck(&player->draw, &enlightenment_light);
    add_deck(&player->draw, &confusing_echo);
    add_deck(&player->draw, &focused_introspection);
    
    shuffle_deck(&player->draw);
    draw_hand(player, HAND_SIZE);
    
    printf("Kaguya descends from the moon! Her defensive systems are ready.\n");
    printf("Remember: Kaguya excels at filtering cards and defensive play!\n");
}

//=============================================================================
// UTILITY FUNCTIONS FOR DEFENSIVE GAMEPLAY
// These help integrate Kaguya's systems with the broader game
//=============================================================================

bool kaguya_can_filter_cards(Player* player) {
    KaguyaState* kaguya_state = get_kaguya_state(player);
    return kaguya_state && (kaguya_state->filtering.cards_filtered_this_turn < 
                           kaguya_state->filtering.max_filters_per_turn);
}

int kaguya_get_defense_cap(Player* player) {
    KaguyaState* kaguya_state = get_kaguya_state(player);
    return kaguya_state ? kaguya_state->defense.current_defense_cap : 6;
}

bool kaguya_is_in_punishment_mode(Player* player) {
    KaguyaState* kaguya_state = get_kaguya_state(player);
    return kaguya_state && kaguya_state->defense.punishment_mode_active;
}

// This demonstrates how filtering can be used by other game systems
void kaguya_emergency_filter(Player* player) {
    // Emergency filtering when hand is empty or in danger
    if (player->hand.cnt == 0) {
        printf("Emergency filtering activated!\n");
        kaguya_filter_and_choose(player, 3, FILTER_DEFENSE_CARDS);
    }
}

// Educational note: Kaguya's design teaches us about:
// 1. Filtering algorithms - essential for any data processing
// 2. Conditional effects - making abilities that respond to game state
// 3. Resource management with dynamic caps
// 4. Defensive programming patterns that protect and enhance
// These concepts apply far beyond games into general software development!
