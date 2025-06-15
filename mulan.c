#include "mulan.h"
#include "cards.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Mulan's counter-attack system demonstrates EVENT-DRIVEN PROGRAMMING
// This is a fundamental concept in game development where actions trigger
// reactions automatically. Unlike Alice's internal state or Snow White's
// resources, Mulan's system RESPONDS to external events.
//
// Key Programming Concepts We're Learning:
// 1. Event-driven architecture - actions trigger reactions
// 2. Callback systems - functions that respond to events
// 3. Energy/Chi management - resource accumulation over time
// 4. Defensive programming - countering opponent actions
// 5. Timing-based effects - responses that must happen immediately

// Chi (Energy) System - Mulan's unique resource that accumulates from combat
// Unlike mana that resets each turn, Chi accumulates and can be spent strategically
typedef struct ChiSystem {
    int current_chi;           // Current Chi available
    int max_chi;              // Maximum Chi that can be stored
    int chi_per_damage;       // Chi gained when taking damage
    int chi_per_counter;      // Chi gained when successfully countering
    bool gathering_storm;     // From "Storm Gathering" twist - gains Chi each turn
} ChiSystem;

// Counter-attack system - this is the heart of Mulan's reactive gameplay
typedef struct CounterSystem {
    bool can_counter_this_turn;        // Whether counter is available
    int max_counters_per_turn;         // Limit on reactive actions
    int counters_used_this_turn;       // Track usage for balance
    Card* prepared_counter_card;       // Card prepared for counter-attack
    bool auto_counter_enabled;         // For advanced players
} CounterSystem;

// Mulan's complete state - combines reactive and resource systems
typedef struct MulanState {
    ChiSystem chi;
    CounterSystem counter;
    bool channeled_qi_active;     // Twist effect state
    int consecutive_blocks;            // Tracks defensive momentum
    int damage_taken_this_turn;        // For Chi generation
} MulanState;

// Forward declarations for Mulan's card effects
void underestimated_effect(void* self, void* target);
void undeniable_effect(void* self, void* target);
void undiminished_effect(void* self, void* target);
void diverted_fire_effect(void* self, void* target);
void diverted_might_effect(void* self, void* target);
void diverted_wrath_effect(void* self, void* target);
void no_retreat_effect(void* self, void* target);
void no_quarter_effect(void* self, void* target);
void no_mercy_effect(void* self, void* target);

// Epic effects
void legends_call_effect(void* self, void* target);
void confronting_chaos_effect(void* self, void* target);
void honors_thunder_effect(void* self, void* target);

//=============================================================================
// CHI ENERGY SYSTEM
// This demonstrates resource accumulation mechanics
//=============================================================================

void init_chi_system(ChiSystem* chi) {
    if (!chi) return;
    
    chi->current_chi = 0;
    chi->max_chi = 10;          // Reasonable upper limit for balance
    chi->chi_per_damage = 1;    // Gain 1 Chi per point of damage taken
    chi->chi_per_counter = 2;   // Bonus Chi for successful counters
    chi->gathering_storm = false;
    
    printf("Chi system initialized - Mulan can accumulate up to %d Chi\n", chi->max_chi);
}

void gain_chi(MulanState* mulan_state, int amount, const char* source) {
    if (!mulan_state) return;
    
    int old_chi = mulan_state->chi.current_chi;
    mulan_state->chi.current_chi = MIN(mulan_state->chi.current_chi + amount, 
                                       mulan_state->chi.max_chi);
    int gained = mulan_state->chi.current_chi - old_chi;
    
    if (gained > 0) {
        printf("Mulan gains %d Chi from %s (now %d/%d)\n", 
               gained, source, mulan_state->chi.current_chi, mulan_state->chi.max_chi);
    }
}

bool spend_chi(MulanState* mulan_state, int amount, const char* purpose) {
    if (!mulan_state || mulan_state->chi.current_chi < amount) {
        printf("Insufficient Chi for %s (need %d, have %d)\n", 
               purpose, amount, mulan_state->chi.current_chi);
        return false;
    }
    
    mulan_state->chi.current_chi -= amount;
    printf("Mulan spends %d Chi for %s (remaining: %d)\n", 
           amount, purpose, mulan_state->chi.current_chi);
    return true;
}

//=============================================================================
// COUNTER-ATTACK SYSTEM
// This is where we implement the event-driven reactive mechanics
//=============================================================================

void init_counter_system(CounterSystem* counter) {
    if (!counter) return;
    
    counter->can_counter_this_turn = true;
    counter->max_counters_per_turn = 1;    // Balanced limitation
    counter->counters_used_this_turn = 0;
    counter->prepared_counter_card = NULL;
    counter->auto_counter_enabled = false;
}

bool prepare_counter_attack(MulanState* mulan_state, Card* defense_card) {
    if (!mulan_state || !defense_card) return false;
    
    // Check if we can prepare a counter
    if (!mulan_state->counter.can_counter_this_turn) {
        printf("No counter attacks available this turn\n");
        return false;
    }
    
    if (mulan_state->counter.counters_used_this_turn >= mulan_state->counter.max_counters_per_turn) {
        printf("Maximum counters per turn reached\n");
        return false;
    }
    
    // Only defense cards or universal cards can be used for counters
    if (defense_card->type != BASIC_DEF && defense_card->type != SKILL_DEF && defense_card->type != UNIVERSAL) {
        printf("Only defense cards can be used for counter attacks\n");
        return false;
    }
    
    mulan_state->counter.prepared_counter_card = defense_card;
    printf("Mulan prepares %s for counter attack (level %d)\n", 
           defense_card->name, defense_card->val);
    return true;
}

// This is the heart of the reactive system - when opponent attacks, this triggers
bool execute_counter_attack(MulanState* mulan_state, Player* mulan_player, 
                           Player* attacker, int incoming_damage) {
    if (!mulan_state || !mulan_player || !attacker) return false;
    
    // Check if counter is prepared and available
    if (!mulan_state->counter.prepared_counter_card) {
        printf("No counter attack prepared\n");
        return false;
    }
    
    Card* counter_card = mulan_state->counter.prepared_counter_card;
    int counter_level = counter_card->val;
    
    // Calculate counter effects
    int damage_reduced = MIN(incoming_damage, counter_level);
    int chi_gained = mulan_state->chi.chi_per_damage * damage_reduced + 
                    mulan_state->chi.chi_per_counter;
    
    // Apply counter effects
    gain_chi(mulan_state, chi_gained, "counter attack");
    mulan_state->counter.counters_used_this_turn++;
    mulan_state->counter.prepared_counter_card = NULL; // Counter is consumed
    
    printf("Mulan executes counter attack! Reduces %d damage and gains %d Chi\n", 
           damage_reduced, chi_gained);
    
    return true;
}

//=============================================================================
// MULAN'S CARD DEFINITIONS
// Each card shows different aspects of the Chi and counter systems
//=============================================================================

// Attack cards that can be enhanced with Chi
Card underestimated = {
    .name = "Underestimated", .type = SKILL_ATK, .val = 1, .cst = 0, .dmg = 1,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = underestimated_effect
};

Card undeniable = {
    .name = "Undeniable", .type = SKILL_ATK, .val = 2, .cst = 2, .dmg = 2,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = undeniable_effect
};

Card undiminished = {
    .name = "Undiminished", .type = SKILL_ATK, .val = 3, .cst = 4, .dmg = 3,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = undiminished_effect
};

// Defense cards that enable counter attacks
Card diverted_fire = {
    .name = "Diverted Fire", .type = SKILL_DEF, .val = 1, .cst = 0, .dmg = 0,
    .defense = 1, .mov = 0, .rng = 0, .link = false, .effect = diverted_fire_effect
};

Card diverted_might = {
    .name = "Diverted Might", .type = SKILL_DEF, .val = 2, .cst = 2, .dmg = 0,
    .defense = 2, .mov = 0, .rng = 0, .link = false, .effect = diverted_might_effect
};

Card diverted_wrath = {
    .name = "Diverted Wrath", .type = SKILL_DEF, .val = 3, .cst = 4, .dmg = 0,
    .defense = 3, .mov = 0, .rng = 0, .link = false, .effect = diverted_wrath_effect
};

// Movement cards with aggressive repositioning
Card no_retreat = {
    .name = "No Retreat", .type = SKILL_MOV, .val = 1, .cst = 1, .dmg = 1,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = no_retreat_effect
};

Card no_quarter = {
    .name = "No Quarter", .type = SKILL_MOV, .val = 2, .cst = 2, .dmg = 2,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = no_quarter_effect
};

Card no_mercy = {
    .name = "Np Mercy", .type = SKILL_MOV, .val = 3, .cst = 4, .dmg = 3,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = no_mercy_effect
};

// Twist cards that enhance Chi system
Card channeled_qi = {
    .name = "Channeled Qi", .type = TWIST, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL
};

Card evading_fate = {
    .name = "Evading Fate", .type = TWIST, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL
};

Card enlightened_assault = {
    .name = "Enlighted Assault", .type = TWIST, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL
};

Card gathering_storm = {
    .name = "Gathering Storm", .type = TWIST, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL
};

// Epic cards with massive Chi expenditure
Card legends_call = {
    .name = "Legend's Call", .type = EPIC, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = legends_call_effect
};

Card confronting_chaos = {
    .name = "Confronting Chaos", .type = EPIC, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = confronting_chaos_effect
};

Card honors_thunder = {
    .name = "Honor's Thunder", .type = EPIC, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = honors_thunder_effect
};

Fable mulan_fable={
    "Mulan",{255,215,0,255},34,0,3,17,
    .skill={
        {
            .cards={&underestimated,&undeniable,&channeled_qi,&undiminished,&gathering_storm},
            .cnt=5
        },
        {
            .cards={&diverted_fire,&diverted_might,&evading_fate,&diverted_wrath,&gathering_storm},
            .cnt=5
        },
        {
            .cards={&no_retreat,&no_quarter,&enlightened_assault,&no_mercy,&gathering_storm},
            .cnt=5
        }
    },
    .epic={&legends_call,&confronting_chaos,&honors_thunder}
};

//=============================================================================
// MULAN STATE MANAGEMENT
// Shows how to coordinate multiple interacting systems
//=============================================================================

MulanState* get_mulan_state(Player* player) {
    return (MulanState*)player->fable->skill;
}

void init_mulan_state(Player* player) {
    if (!player || !player->fable) return;
    
    MulanState* mulan_state = malloc(sizeof(MulanState));
    memset(mulan_state, 0, sizeof(MulanState));
    
    // Initialize subsystems
    init_chi_system(&mulan_state->chi);
    init_counter_system(&mulan_state->counter);
    
    mulan_state->channeled_qi_active = false;
    mulan_state->consecutive_blocks = 0;
    mulan_state->damage_taken_this_turn = 0;
    
    player->fable->character_state = mulan_state;
    
    printf("Mulan's combat systems initialized - ready for reactive combat!\n");
}

//=============================================================================
// EVENT HANDLERS - THE CORE OF REACTIVE PROGRAMMING
// These functions respond to events that happen during the game
//=============================================================================

// Called whenever Mulan takes damage - this is event-driven programming!
void mulan_on_damage_taken(Player* mulan_player, int damage, Player* source) {
    if (!mulan_player || damage <= 0) return;
    
    MulanState* mulan_state = get_mulan_state(mulan_player);
    if (!mulan_state) return;
    
    // Record damage for turn tracking
    mulan_state->damage_taken_this_turn += damage;
    
    // Opportunity for counter attack (reactive gameplay)
    printf("Mulan takes %d damage - counter attack opportunity available!\n", damage);
    
    // Check if a counter was prepared
    if (mulan_state->counter.prepared_counter_card) {
        // Ask player if they want to execute counter (in full game, this would be UI)
        bool execute_counter = true; // Simplified - always counter when available
        
        if (execute_counter) {
            execute_counter_attack(mulan_state, mulan_player, source, damage);
        }
    }
    
    // Gain Chi from taking damage (reactive resource generation)
    int chi_from_damage = damage * mulan_state->chi.chi_per_damage;
    gain_chi(mulan_state, chi_from_damage, "taking damage");
}

// Called at the start of each turn
void mulan_turn_start(Player* player) {
    MulanState* mulan_state = get_mulan_state(player);
    if (!mulan_state) return;
    
    // Reset turn-based counters
    mulan_state->counter.counters_used_this_turn = 0;
    mulan_state->counter.can_counter_this_turn = true;
    mulan_state->damage_taken_this_turn = 0;
    
    // Storm Gathering effect - passive Chi generation
    if (mulan_state->chi.gathering_storm) {
        gain_chi(mulan_state, 1, "Storm Gathering");
    }
    
    printf("Mulan begins turn - counter attacks available, Chi: %d/%d\n",
           mulan_state->chi.current_chi, mulan_state->chi.max_chi);
}

void mulan_turn_end(Player* player) {
    MulanState* mulan_state = get_mulan_state(player);
    if (!mulan_state) return;
    
    // Check if Mulan successfully defended this turn
    if (mulan_state->damage_taken_this_turn == 0) {
        mulan_state->consecutive_blocks++;
        printf("Mulan successfully defended this turn (streak: %d)\n", 
               mulan_state->consecutive_blocks);
        
        // Bonus Chi for consecutive successful defenses
        if (mulan_state->consecutive_blocks >= 3) {
            gain_chi(mulan_state, 1, "defensive mastery");
        }
    } else {
        mulan_state->consecutive_blocks = 0;
    }
    
    // Clear prepared counter if not used
    if (mulan_state->counter.prepared_counter_card) {
        printf("Prepared counter attack expires\n");
        mulan_state->counter.prepared_counter_card = NULL;
    }
}

//=============================================================================
// MULAN'S CARD EFFECT IMPLEMENTATIONS
// These show how Chi enhances different types of actions
//=============================================================================

void underestimated_effect(void* self, void* target) {
    Player* mulan = (Player*)self;
    Player* opponent = (Player*)target;
    MulanState* mulan_state = get_mulan_state(mulan);
    
    // "Range 1, Damage 1+O, can spend X Chi for +X damage (X ≤ 3)"
    
    int base_damage = 1; // + armor value in full implementation
    int max_chi_spend = MIN(3, mulan_state->chi.current_chi);
    
    // In full implementation, player would choose how much Chi to spend
    int chi_to_spend = max_chi_spend; // Simplified - spend maximum available
    
    if (chi_to_spend > 0 && spend_chi(mulan_state, chi_to_spend, "enhancing attack")) {
        int total_damage = base_damage + chi_to_spend;
        printf("Underestimate enhanced with %d Chi - total damage: %d\n", 
               chi_to_spend, total_damage);
        
        // Apply enhanced damage
        opponent->health -= total_damage;
    } else {
        opponent->health -= base_damage;
        printf("Underestimate deals base damage: %d\n", base_damage);
    }
}

void diverted_fire_effect(void* self, void* target) {
    Player* mulan = (Player*)self;
    MulanState* mulan_state = get_mulan_state(mulan);
    
    // "Defense O, when drawing cards at turn end, can spend X Chi for +X cards (X ≤ 1)"
    
    int base_defense = 1; // Level 1 defense skill
    mulan->defense = MIN(mulan->defense + base_defense, mulan->fable->defense);
    
    // Prepare this card for counter attack
    prepare_counter_attack(mulan_state, &diverted_fire);
    
    printf("Calm Control: +%d defense and counter attack prepared\n", base_defense);
}

void no_retreat_effect(void* self, void* target) {
    Player* mulan = (Player*)self;
    Player* opponent = (Player*)target;
    MulanState* mulan_state = get_mulan_state(mulan);
    
    // "Range 1, Damage 1, knock opponent back up to O spaces"
    // "Then move to adjacent space to opponent"
    // "If opponent at edge, discard 1 random card from their hand"
    
    int damage = 1;
    opponent->health -= damage;
    
    // Knockback effect (simplified position system)
    int knockback_distance = 1; // Level 1 movement
    int old_pos = opponent->pos;
    opponent->pos += (mulan->pos < opponent->pos) ? knockback_distance : -knockback_distance;
    opponent->pos = MAX(-4, MIN(4, opponent->pos)); // Clamp to battlefield
    
    // Move Mulan adjacent to opponent
    mulan->pos = opponent->pos + ((mulan->pos < opponent->pos) ? -1 : 1);
    mulan->pos = MAX(-4, MIN(4, mulan->pos));
    
    // Check if opponent is at edge
    if (abs(opponent->pos) == 4) {
        // Discard random card from hand
        if (opponent->hand.cnt > 0) {
            int random_index = rand() % opponent->hand.cnt;
            Card* discarded = opponent->hand.cards[random_index];
            
            // Remove from hand
            for (int i = random_index; i < opponent->hand.cnt - 1; i++) {
                opponent->hand.cards[i] = opponent->hand.cards[i + 1];
            }
            opponent->hand.cnt--;
            
            // Add to discard
            add_deck(&opponent->disc, discarded);
            
            printf("Never Retreat: Opponent forced to edge, discards card!\n");
        }
    }
    
    printf("Never Retreat: Aggressive repositioning complete\n");
}

void legends_call_effect(void* self, void* target) {
    Player* mulan = (Player*)self;
    MulanState* mulan_state = get_mulan_state(mulan);
    
    // "When drawing cards at turn end, draw +4 additional cards"
    
    // This is a delayed effect that triggers at turn end
    // In full implementation, would set a flag for turn end processing
    draw_hand(mulan, 4);
    
    printf("Soaring Spirit: Mulan draws 4 additional cards!\n");
}

void confronting_chaos_effect(void* self, void* target) {
    Player* mulan = (Player*)self;
    Player* opponent = (Player*)target;
    MulanState* mulan_state = get_mulan_state(mulan);
    
    // "Move to adjacent space to opponent and immediately gain 3 Chi"
    
    // Move adjacent to opponent
    mulan->pos = opponent->pos + ((mulan->pos < opponent->pos) ? -1 : 1);
    mulan->pos = MAX(-4, MIN(4, mulan->pos));
    
    // Gain Chi immediately
    gain_chi(mulan_state, 3, "Face Chaos");
    
    printf("Face Chaos: Mulan moves adjacent and gains combat focus!\n");
}

void honors_thunder_effect(void* self, void* target) {
    Player* mulan = (Player*)self;
    Player* opponent = (Player*)target;
    MulanState* mulan_state = get_mulan_state(mulan);
    
    // "Range 1, immediately spend all Chi to deal X damage (X = Chi spent)"
    
    int chi_to_spend = mulan_state->chi.current_chi;
    if (chi_to_spend > 0) {
        spend_chi(mulan_state, chi_to_spend, "Thunder Strike");
        opponent->health -= chi_to_spend;
        
        printf("Thunder Strike: Mulan unleashes all %d Chi as damage!\n", chi_to_spend);
    } else {
        printf("Thunder Strike: No Chi available to unleash\n");
    }
}

//=============================================================================
// MULAN CHARACTER INITIALIZATION
// Shows how to set up complex interdependent systems
//=============================================================================

void setup_mulan_player(Player* player) {
    if (!player) return;
    
    player->fable = &mulan_fable;
    player->health = mulan_fable.health;
    player->power = 0;
    player->defense = 0;
    
    // Initialize Mulan's reactive systems
    init_mulan_state(player);
    
    // Set up starting deck with emphasis on defense for counter attacks
    for (int i = 0; i < 3; i++) {
        add_deck(&player->draw, &Attack1);
        add_deck(&player->draw, &Defense1);
        add_deck(&player->draw, &Move1);
    }
    
    // Add starting skills - focus on counter attack capability
    add_deck(&player->draw, &underestimated);
    add_deck(&player->draw, &diverted_fire);
    add_deck(&player->draw, &no_retreat);
    
    shuffle_deck(&player->draw);
    draw_hand(player, HAND_SIZE);
    
    printf("Mulan takes the field! Her reactive combat systems are ready.\n");
    printf("Remember: Mulan gains power by responding to opponent actions!\n");
}

//=============================================================================
// UTILITY FUNCTIONS FOR REACTIVE GAMEPLAY
// These help integrate Mulan's systems with the broader game
//=============================================================================

bool mulan_has_counter_prepared(Player* player) {
    MulanState* mulan_state = get_mulan_state(player);
    return mulan_state && mulan_state->counter.prepared_counter_card != NULL;
}

int mulan_get_current_chi(Player* player) {
    MulanState* mulan_state = get_mulan_state(player);
    return mulan_state ? mulan_state->chi.current_chi : 0;
}

// This function should be called by the main game loop whenever ANY player takes damage
void check_mulan_counter_opportunity(Player* damaged_player, int damage, Player* attacker) {
    // Check if the damaged player is Mulan
    if (damaged_player && damaged_player->fable && 
        strcmp(damaged_player->fable->name, "Mulan") == 0) {
        mulan_on_damage_taken(damaged_player, damage, attacker);
    }
}

// Educational note: This demonstrates the Observer pattern in game programming
// Mulan "observes" damage events and reacts to them automatically
// This is a key concept in event-driven game architecture
