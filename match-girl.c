#include "match-girl.h"
#include "cards.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Enhanced Match Girl system demonstrates RESOURCE ACCUMULATION and RISK/REWARD MECHANICS
// This teaches us advanced programming concepts:
// 1. Cross-player interaction systems - actions that affect both players
// 2. Resource accumulation - gaining power from opponent's actions  
// 3. Risk/reward mechanics - trading health for power
// 4. Energy spending systems - flexible resource allocation
// 5. Persistent effects - ongoing game state modifications

// Enhanced Energy System - Match Girl's unique resource that grows from opponent actions
/*typedef struct EnergySystem {
    int current_energy;              // Current energy available
    int max_energy;                  // Maximum energy that can be stored
    int energy_per_fire_card;        // Energy gained when opponent uses fire cards
    int energy_per_turn;             // Passive energy generation
    bool flame_enhancement_active;   // From twist cards
    int energy_spent_this_turn;      // Track spending for balance
} EnergySystem;

// Fire Card Management System - Managing the external fire card resource
typedef struct FireSystem {
    int fire_cards_placed_this_turn;    // Track placement limits
    int max_fire_per_turn;              // Balance limitation
    bool can_place_fire_adjacent;       // Special rule activation
    int fire_cards_in_opponent_deck;    // Track for strategic decisions
    int fire_cards_in_opponent_discard; // Track for retrieval abilities
} FireSystem;

// Risk/Reward System - Trading health for power
typedef struct RiskRewardSystem {
    int max_life_loss_per_turn;         // Safety limitation
    int life_lost_this_turn;            // Track for limits
    int cards_drawn_from_life_loss;     // Track efficiency
    bool desperate_measures_active;     // Enhanced life trading
} RiskRewardSystem;

// Match Girl's complete enhanced state
typedef struct MatchGirlState {
    EnergySystem energy;
    FireSystem fire;
    RiskRewardSystem risk_reward;
    bool ritual_of_pain_active;         // Twist effect
    bool storm_eve_stacks;              // Accumulating twist effect
    int consecutive_energy_turns;       // Momentum tracking
} MatchGirlState;

// Enhanced fire card with more detailed tracking
typedef struct EnhancedFireCard {
    int level;                          // 1, 2, or 3
    int damage_on_use;                  // Damage when opponent uses it
    int energy_given_to_match_girl;     // Energy Match Girl gets
    bool can_be_used_as_attack;         // Rule restrictions
    bool can_be_used_as_defense;        // Rule restrictions  
    bool can_be_used_as_movement;       // Rule restrictions
    char restriction_reason[64];        // Why it's restricted
} EnhancedFireCard;*/

// Global enhanced fire supply with better tracking
static EnhancedFireCard enhanced_fire_supply[12];
static bool enhanced_fire_initialized = false;

// Forward declarations for enhanced card effects
void make_a_wish_effect(void* self, void* target);
void regret_a_wish_effect(void* self, void* target);
void requite_a_wish_effect(void* self, void* target);
void diabolical_offer_effect(void* self, void* target);
void diabolical_gamble_effect(void* self, void* target);
void diabolical_deal_effect(void* self, void* target);
void your_soul_weighed_effect(void* self, void* target);
void your_soul_wanting_effect(void* self, void* target);
void your_soul_ravaged_effect(void* self, void* target);

// Enhanced epic effects
void inferno_overload_effect(void* self, void* target);
void the_demons_due_effect(void* self, void* target);
void a_surfeit_of_wishes_effect(void* self, void* target);

//=============================================================================
// ENHANCED FIRE CARD SYSTEM
// This demonstrates external resource management with detailed tracking
//=============================================================================

void init_enhanced_fire_supply(void) {
    if (enhanced_fire_initialized) return;
    
    // Initialize 12 fire cards with enhanced properties
    for (int i = 0; i < 12; i++) {
        enhanced_fire_supply[i].level = (i / 4) + 1; // 4 cards each of levels 1, 2, 3
        enhanced_fire_supply[i].damage_on_use = enhanced_fire_supply[i].level;
        enhanced_fire_supply[i].energy_given_to_match_girl = 1; // Always 1 energy
        
        // Fire cards have usage restrictions based on twist effects
        enhanced_fire_supply[i].can_be_used_as_attack = false;     // Base restriction
        enhanced_fire_supply[i].can_be_used_as_defense = false;    // Base restriction
        enhanced_fire_supply[i].can_be_used_as_movement = false;   // Base restriction
        
        strcpy(enhanced_fire_supply[i].restriction_reason, 
               "Fire cards cannot be used as attack/defense/movement cards");
    }
    
    enhanced_fire_initialized = true;
    printf("Enhanced fire supply initialized with strategic tracking\n");
}

EnhancedFireCard* take_enhanced_fire_card(int preferred_level) {
    if (!enhanced_fire_initialized) {
        init_enhanced_fire_supply();
    }
    
    // Try to find card of preferred level first
    for (int i = (preferred_level - 1) * 4; i < preferred_level * 4; i++) {
        if (!enhanced_fire_supply[i].can_be_used_as_attack) { // Using this as "available" flag
            enhanced_fire_supply[i].can_be_used_as_attack = true; // Mark as taken
            printf("Enhanced fire card L%d taken from supply\n", preferred_level);
            return &enhanced_fire_supply[i];
        }
    }
    
    // If preferred level not available, try any level
    for (int i = 0; i < 12; i++) {
        if (!enhanced_fire_supply[i].can_be_used_as_attack) {
            enhanced_fire_supply[i].can_be_used_as_attack = true;
            printf("Enhanced fire card L%d taken (preferred L%d not available)\n", 
                   enhanced_fire_supply[i].level, preferred_level);
            return &enhanced_fire_supply[i];
        }
    }
    
    printf("No enhanced fire cards available!\n");
    return NULL;
}

//=============================================================================
// ENHANCED ENERGY SYSTEM
// Shows advanced resource accumulation mechanics
//=============================================================================

void init_enhanced_energy_system(EnergySystem* energy) {
    if (!energy) return;
    
    energy->current_energy = 0;
    energy->max_energy = 15;              // Higher cap for strategic depth
    energy->energy_per_fire_card = 1;     // Base energy per fire card used
    energy->energy_per_turn = 0;          // No passive generation (earned through play)
    energy->flame_enhancement_active = false;
    energy->energy_spent_this_turn = 0;
    
    printf("Enhanced energy system initialized - Match Girl can accumulate up to %d energy\n", 
           energy->max_energy);
}

void gain_enhanced_energy(MatchGirlState* mg_state, int amount, const char* source) {
    if (!mg_state) return;
    
    // Apply flame enhancement multiplier
    if (mg_state->energy.flame_enhancement_active) {
        amount *= 2;
        printf("Flame enhancement doubles energy gain!\n");
    }
    
    int old_energy = mg_state->energy.current_energy;
    mg_state->energy.current_energy = MIN(mg_state->energy.current_energy + amount, 
                                          mg_state->energy.max_energy);
    int gained = mg_state->energy.current_energy - old_energy;
    
    if (gained > 0) {
        printf("Match Girl gains %d energy from %s (now %d/%d)\n", 
               gained, source, mg_state->energy.current_energy, mg_state->energy.max_energy);
        
        // Track momentum
        if (gained > 0) {
            mg_state->consecutive_energy_turns++;
        }
    }
}

bool spend_enhanced_energy(MatchGirlState* mg_state, int amount, const char* purpose) {
    if (!mg_state || mg_state->energy.current_energy < amount) {
        printf("Insufficient energy for %s (need %d, have %d)\n", 
               purpose, amount, mg_state->energy.current_energy);
        return false;
    }
    
    mg_state->energy.current_energy -= amount;
    mg_state->energy.energy_spent_this_turn += amount;
    
    printf("Match Girl spends %d energy for %s (remaining: %d)\n", 
           amount, purpose, mg_state->energy.current_energy);
    return true;
}

//=============================================================================
// RISK/REWARD SYSTEM
// Demonstrates trading health for strategic advantage
//=============================================================================

void init_risk_reward_system(RiskRewardSystem* risk_reward) {
    if (!risk_reward) return;
    
    risk_reward->max_life_loss_per_turn = 3;     // Safety limit
    risk_reward->life_lost_this_turn = 0;
    risk_reward->cards_drawn_from_life_loss = 0;
    risk_reward->desperate_measures_active = false;
}

bool trade_life_for_cards(MatchGirlState* mg_state, Player* match_girl, int life_to_lose, int cards_to_draw) {
    if (!mg_state || !match_girl) return false;
    
    // Check safety limits
    if (mg_state->risk_reward.life_lost_this_turn + life_to_lose > mg_state->risk_reward.max_life_loss_per_turn) {
        printf("Maximum life loss per turn reached (%d/%d)\n", 
               mg_state->risk_reward.life_lost_this_turn, mg_state->risk_reward.max_life_loss_per_turn);
        return false;
    }
    
    if (match_girl->health <= life_to_lose) {
        printf("Cannot lose %d life (would be fatal, current health: %d)\n", 
               life_to_lose, match_girl->health);
        return false;
    }
    
    // Execute the trade
    match_girl->health -= life_to_lose;
    mg_state->risk_reward.life_lost_this_turn += life_to_lose;
    mg_state->risk_reward.cards_drawn_from_life_loss += cards_to_draw;
    
    draw_hand(match_girl, cards_to_draw);
    
    printf("Match Girl trades %d life for %d cards (health: %d, cards drawn this turn: %d)\n",
           life_to_lose, cards_to_draw, match_girl->health, mg_state->risk_reward.cards_drawn_from_life_loss);
    
    return true;
}

bool can_trade_life_for_cards(MatchGirlState* mg_state, Player* match_girl, int life_cost) {
    if (!mg_state || !match_girl) return false;
    
    return (match_girl->health > life_cost && 
            mg_state->risk_reward.life_lost_this_turn + life_cost <= mg_state->risk_reward.max_life_loss_per_turn);
}

//=============================================================================
// ENHANCED MATCH GIRL CARD DEFINITIONS
// Each card demonstrates sophisticated energy and risk mechanics
//=============================================================================

// Enhanced attack cards with flexible energy spending
Card make_a_wish = {
    .name = "Make a Wish", .type = SKILL_ATK, .val = 1, .cst = 0, .dmg = 1,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = make_a_wish_effect
};

Card regret_a_wish = {
    .name = "Regret a Wish", .type = SKILL_ATK, .val = 2, .cst = 2, .dmg = 2,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = regret_a_wish_effect
};

Card requite_a_wish = {
    .name = "Requite a Wish", .type = SKILL_ATK, .val = 3, .cst = 4, .dmg = 3,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = requite_a_wish_effect
};

// Enhanced defense cards with life trading
Card diabolical_offer = {
    .name = "Diabolical Offer", .type = SKILL_DEF, .val = 1, .cst = 0, .dmg = 0,
    .defense = 1, .mov = 0, .rng = 0, .link = false, .effect = diabolical_offer_effect
};

Card diabolical_gamble = {
    .name = "Diabolical Gamble", .type = SKILL_DEF, .val = 2, .cst = 2, .dmg = 0,
    .defense = 1, .mov = 0, .rng = 0, .link = false, .effect = diabolical_gamble_effect
};

Card diabolical_deal = {
    .name = "Diabolical Deal", .type = SKILL_DEF, .val = 3, .cst = 4, .dmg = 0,
    .defense = 1, .mov = 0, .rng = 0, .link = false, .effect = diabolical_deal_effect
};

// Enhanced movement cards with fire retrieval
Card your_soul_weighed = {
    .name = "Your Soul Weighed", .type = SKILL_MOV, .val = 1, .cst = 0, .dmg = 1,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = your_soul_weighed_effect
};

Card your_soul_wanting = {
    .name = "Your Soul Wanting", .type = SKILL_MOV, .val = 2, .cst = 2, .dmg = 2,
    .defense = 0, .mov = 0, .rng = 2, .link = false, .effect = your_soul_wanting_effect
};

Card your_soul_ravaged = {
    .name = "Your Soul Ravaged", .type = SKILL_MOV, .val = 3, .cst = 4, .dmg = 3,
    .defense = 0, .mov = 0, .rng = 3, .link = false, .effect = your_soul_ravaged_effect
};

// Enhanced epic cards with ultimate energy effects
Card inferno_overload = {
    .name = "Inferno Overload", .type = EPIC, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = inferno_overload_effect
};

Card the_demons_due = {
    .name = "The Demon's Due", .type = EPIC, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 3, .link = false, .effect = the_demons_due_effect
};

Card a_surfeit_of_wishes = {
    .name = "A Surfeit of Wishes", .type = EPIC, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = a_surfeit_of_wishes_effect
};

// twist
Card ritual_of_pain = {
    .name = "Ritual of Pain", .type = TWIST, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL
};

Card inflamed_craving = {
    .name = "Inflamed Craving", .type = TWIST, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL
};

Card after_all_ive_given = {
    .name = "After All I've Given", .type = TWIST, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL
};

Card playing_with_fire = {
    .name = "Playing With Fire", .type = TWIST, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL
};

Fable match_girl_enhanced_fable={
    "Match Girl",{255,127,36,255},36,0,0,18,
    .skill={
        {
            .cards={&make_a_wish,&regret_a_wish,&ritual_of_pain,&requite_a_wish,&playing_with_fire},
            .cnt=5
        },
        {
            .cards={&diabolical_offer,&diabolical_gamble,&inflamed_craving,&diabolical_deal,&playing_with_fire},
            .cnt=5
        },
        {
            .cards={&your_soul_weighed,&your_soul_wanting,&after_all_ive_given,&your_soul_ravaged,&playing_with_fire},
            .cnt=5
        }
    },
    .epic={&inferno_overload,&the_demons_due,&a_surfeit_of_wishes}
};

//=============================================================================
// ENHANCED MATCH GIRL STATE MANAGEMENT
// Shows coordination of multiple interacting resource systems
//=============================================================================

MatchGirlState* get_enhanced_match_girl_state(Player* player) {
    return (MatchGirlState*)player->fable->skill;
}

void init_enhanced_match_girl_state(Player* player) {
    if (!player || !player->fable) return;
    
    MatchGirlState* mg_state = malloc(sizeof(MatchGirlState));
    memset(mg_state, 0, sizeof(MatchGirlState));
    
    // Initialize all subsystems
    init_enhanced_energy_system(&mg_state->energy);
    
    mg_state->fire.max_fire_per_turn = 1;        // Base limit - once per turn
    mg_state->fire.can_place_fire_adjacent = false;
    mg_state->fire.fire_cards_in_opponent_deck = 0;
    mg_state->fire.fire_cards_in_opponent_discard = 0;
    
    init_risk_reward_system(&mg_state->risk_reward);
    
    mg_state->ritual_of_pain_active = false;
    mg_state->storm_eve_stacks = false;
    mg_state->consecutive_energy_turns = 0;
    
    player->fable->character_state = mg_state;
    
    printf("Enhanced Match Girl systems initialized - energy accumulation ready!\n");
}

//=============================================================================
// ENHANCED TURN MANAGEMENT
// Shows how to coordinate multiple resource systems
//=============================================================================

void match_girl_enhanced_turn_start(Player* player) {
    MatchGirlState* mg_state = get_enhanced_match_girl_state(player);
    if (!mg_state) return;
    
    // Reset turn-based counters
    mg_state->fire.fire_cards_placed_this_turn = 0;
    mg_state->energy.energy_spent_this_turn = 0;
    mg_state->risk_reward.life_lost_this_turn = 0;
    mg_state->risk_reward.cards_drawn_from_life_loss = 0;
    
    // Storm Eve effect - passive energy if stacks are active
    if (mg_state->storm_eve_stacks) {
        gain_enhanced_energy(mg_state, 1, "Storm Eve");
    }
    
    printf("Match Girl begins turn - Energy: %d/%d, Fire placement available\n",
           mg_state->energy.current_energy, mg_state->energy.max_energy);
}

void match_girl_enhanced_turn_end(Player* player) {
    MatchGirlState* mg_state = get_enhanced_match_girl_state(player);
    if (!mg_state) return;
    
    // Check if Match Girl gained energy this turn
    if (mg_state->energy.energy_spent_this_turn == 0 && mg_state->consecutive_energy_turns > 0) {
        mg_state->consecutive_energy_turns = 0;
        printf("Match Girl's energy momentum broken\n");
    }
    
    // Apply any end-of-turn effects
    if (mg_state->ritual_of_pain_active && mg_state->energy.current_energy >= 5) {
        printf("Ritual of Pain: High energy maintained\n");
    }
}

void match_girl_enhanced_update(Player* player, float delta_time) {
    // Match Girl doesn't need continuous updates, but this is where
    // we could add visual effects for fire/energy
    (void)delta_time;
    
    MatchGirlState* mg_state = get_enhanced_match_girl_state(player);
    if (!mg_state) return;
    
    // Could add energy pulsing animations based on current energy level
}

//=============================================================================
// CROSS-PLAYER INTERACTION SYSTEM
// This demonstrates how Match Girl's abilities affect the opponent
//=============================================================================

bool place_enhanced_fire_card(Player* match_girl, Player* opponent, int fire_level) {
    MatchGirlState* mg_state = get_enhanced_match_girl_state(match_girl);
    if (!mg_state) return false;
    
    // Check placement rules
    if (mg_state->fire.fire_cards_placed_this_turn >= mg_state->fire.max_fire_per_turn) {
        printf("Maximum fire cards placed this turn\n");
        return false;
    }
    
    // Check adjacency requirement
    if (!mg_state->fire.can_place_fire_adjacent && abs(match_girl->pos - opponent->pos) != 1) {
        printf("Must be adjacent to opponent to place fire cards\n");
        return false;
    }
    
    // Get fire card from supply
    EnhancedFireCard* fire_card = take_enhanced_fire_card(fire_level);
    if (!fire_card) return false;
    
    // Create game card representation
    Card* fire_game_card = malloc(sizeof(Card));
    memset(fire_game_card, 0, sizeof(Card));
    snprintf(fire_game_card->name, sizeof(fire_game_card->name), "Fire L%d", fire_level);
    fire_game_card->type = UNIVERSAL;
    fire_game_card->dmg = fire_card->damage_on_use;
    fire_game_card->val = -fire_level; // Negative to identify as fire card
    
    // Place on top of opponent's deck
    if (opponent->draw.cnt < MAX_CARD) {
        // Shift cards down
        for (int i = opponent->draw.cnt; i > 0; i--) {
            opponent->draw.cards[i] = opponent->draw.cards[i-1];
        }
        opponent->draw.cards[0] = fire_game_card;
        opponent->draw.cnt++;
        
        mg_state->fire.fire_cards_placed_this_turn++;
        mg_state->fire.fire_cards_in_opponent_deck++;
        
        printf("Match Girl places Fire L%d on top of opponent's deck\n", fire_level);
        return true;
    }
    
    return false;
}

// Called when opponent uses a fire card - this is where Match Girl gains energy
void on_opponent_uses_enhanced_fire_card(Player* match_girl, Card* fire_card) {
    if (!match_girl || !fire_card) return;
    
    MatchGirlState* mg_state = get_enhanced_match_girl_state(match_girl);
    if (!mg_state) return;
    
    // Identify fire card level
    int fire_level = abs(fire_card->val); // Fire cards have negative val
    
    // Gain energy
    int energy_gain = mg_state->energy.energy_per_fire_card;
    
    // Apply twist card bonuses
    if (mg_state->ritual_of_pain_active) {
        energy_gain += 1; // Bonus energy from twist
    }
    
    gain_enhanced_energy(mg_state, energy_gain, "opponent using fire card");
    
    // Track fire card movement
    mg_state->fire.fire_cards_in_opponent_deck--;
    mg_state->fire.fire_cards_in_opponent_discard++;
    
    printf("Opponent uses Fire L%d - Match Girl gains %d energy\n", fire_level, energy_gain);
}

//=============================================================================
// ENHANCED CARD EFFECT IMPLEMENTATIONS
// These show sophisticated energy spending and risk/reward mechanics
//=============================================================================

void regret_a_wish_effect(void* self, void* target) {
    Player* match_girl = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Match Girl uses Regret a Wish\n");
    if (opponent) {
        opponent->health -= 2;
    }
}

void requite_a_wish_effect(void* self, void* target) {
    Player* match_girl = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Match Girl uses Requite a Wish\n");
    if (opponent) {
        opponent->health -= 3;
    }
}

void diabolical_gamble_effect(void* self, void* target) {
    Player* match_girl = (Player*)self;
    
    printf("Match Girl uses Diabolical Gamble\n");
    if (match_girl) {
        match_girl->defense = MIN(match_girl->defense + 1, match_girl->fable->defense);
    }
}

void diabolical_deal_effect(void* self, void* target) {
    Player* match_girl = (Player*)self;
    
    printf("Match Girl uses Diabolical Deal\n");
    if (match_girl) {
        match_girl->defense = MIN(match_girl->defense + 2, match_girl->fable->defense);
    }
}

void your_soul_wanting_effect(void* self, void* target) {
    Player* match_girl = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Match Girl uses Your Soul Wanting\n");
    if (opponent) {
        opponent->health -= 2;
    }
}

void your_soul_ravaged_effect(void* self, void* target) {
    Player* match_girl = (Player*)self;
    Player* opponent = (Player*)target;
    
    printf("Match Girl uses Your Soul Ravaged\n");
    if (opponent) {
        opponent->health -= 3;
    }
}

void make_a_wish_effect(void* self, void* target) {
    Player* match_girl = (Player*)self;
    Player* opponent = (Player*)target;
    MatchGirlState* mg_state = get_enhanced_match_girl_state(match_girl);
    
    // "Range 1, Damage 1+O, can spend energy to enhance damage (every 3 energy = +1 damage)"
    
    int base_damage = 1;
    int max_energy_spend = mg_state->energy.current_energy;
    int energy_per_damage = 3;
    int max_bonus_damage = max_energy_spend / energy_per_damage;
    
    // Player choice simulation - spend maximum available for demo
    int energy_to_spend = (max_bonus_damage * energy_per_damage);
    energy_to_spend = MIN(energy_to_spend, max_energy_spend);
    
    int bonus_damage = energy_to_spend / energy_per_damage;
    int total_damage = base_damage + bonus_damage;
    
    if (energy_to_spend > 0 && spend_enhanced_energy(mg_state, energy_to_spend, "enhancing Phantom Wish")) {
        opponent->health -= total_damage;
        printf("Phantom Wish enhanced with %d energy - total damage: %d\n", energy_to_spend, total_damage);
    } else {
        opponent->health -= base_damage;
        printf("Phantom Wish deals base damage: %d\n", base_damage);
    }
}

void diabolical_offer_effect(void* self, void* target) {
    Player* match_girl = (Player*)self;
    MatchGirlState* mg_state = get_enhanced_match_girl_state(match_girl);
    
    // "Defense 1, can lose X life to draw X cards (X ≤ 1)"
    
    int defense_gain = 1;
    match_girl->defense = MIN(match_girl->defense + defense_gain, match_girl->fable->defense);
    
    // Offer life trading option
    int max_life_trade = MIN(1, match_girl->health - 1); // Never go to 0 health
    
    if (max_life_trade > 0 && can_trade_life_for_cards(mg_state, match_girl, max_life_trade)) {
        // In full implementation, player would choose
        bool use_life_trade = (mg_state->risk_reward.cards_drawn_from_life_loss < 2); // Limit per turn
        
        if (use_life_trade) {
            trade_life_for_cards(mg_state, match_girl, max_life_trade, max_life_trade);
        }
    }
    
    printf("Devil's Sacrifice: +%d defense and risk/reward option\n", defense_gain);
}

void your_soul_weighed_effect(void* self, void* target) {
    Player* match_girl = (Player*)self;
    Player* opponent = (Player*)target;
    MatchGirlState* mg_state = get_enhanced_match_girl_state(match_girl);
    
    // "Range 1, Damage 1, can return up to O fire cards from opponent's discard to fire pile"
    // "Gain 1 energy per fire card returned"
    
    int damage = 1;
    opponent->health -= damage;
    
    // Look for fire cards in opponent's discard
    int fire_cards_found = 0;
    int max_returns = 1; // Level 1 skill
    
    for (int i = opponent->disc.cnt - 1; i >= 0 && fire_cards_found < max_returns; i--) {
        Card* card = opponent->disc.cards[i];
        if (card && card->val < 0) { // Fire cards have negative val
            // Return to fire supply (simplified)
            for (int j = i; j < opponent->disc.cnt - 1; j++) {
                opponent->disc.cards[j] = opponent->disc.cards[j + 1];
            }
            opponent->disc.cnt--;
            
            // Gain energy
            gain_enhanced_energy(mg_state, 1, "fire card retrieval");
            fire_cards_found++;
            
            free(card); // Clean up fire card
            printf("Fire card returned to supply\n");
        }
    }
    
    printf("Weightless Soul: %d damage and %d fire cards retrieved\n", damage, fire_cards_found);
}

void inferno_overload_effect(void* self, void* target) {
    Player* match_girl = (Player*)self;
    Player* opponent = (Player*)target;
    MatchGirlState* mg_state = get_enhanced_match_girl_state(match_girl);
    
    // "Range 1, Damage X where X = half of current energy (rounded up)"
    
    int energy_damage = (mg_state->energy.current_energy + 1) / 2; // Round up
    
    if (energy_damage > 0) {
        opponent->health -= energy_damage;
        printf("Hell Flame consumes Match Girl's energy - %d damage from %d energy!\n", 
               energy_damage, mg_state->energy.current_energy);
    } else {
        printf("Hell Flame fizzles - no energy available\n");
    }
}

void the_demons_due_effect(void* self, void* target) {
    Player* match_girl = (Player*)self;
    Player* opponent = (Player*)target;
    MatchGirlState* mg_state = get_enhanced_match_girl_state(match_girl);
    
    // "Range 3, discard top 6 cards from opponent's deck"
    // "Deal 1 damage for each fire card discarded"
    
    int cards_to_discard = 6;
    int fire_cards_hit = 0;
    
    for (int i = 0; i < cards_to_discard && opponent->draw.cnt > 0; i++) {
        Card* discarded = draw_deck(&opponent->draw);
        if (discarded) {
            if (discarded->val < 0) { // Fire card
                fire_cards_hit++;
                on_opponent_uses_enhanced_fire_card(match_girl, discarded); // Match Girl gains energy
            }
            add_deck(&opponent->disc, discarded);
        }
    }
    
    if (fire_cards_hit > 0) {
        opponent->health -= fire_cards_hit;
        printf("Doomfall hits %d fire cards - %d damage and energy gained!\n", 
               fire_cards_hit, fire_cards_hit);
    } else {
        printf("Doomfall discards 6 cards but hits no fire cards\n");
    }
}

void a_surfeit_of_wishes_effect(void* self, void* target) {
    Player* match_girl = (Player*)self;
    Player* opponent = (Player*)target;
    MatchGirlState* mg_state = get_enhanced_match_girl_state(match_girl);
    
    // "Place up to 3 fire cards on top of opponent's deck"
    
    int fire_cards_placed = 0;
    for (int i = 0; i < 3; i++) {
        if (place_enhanced_fire_card(match_girl, opponent, 1)) { // Level 1 fire cards
            fire_cards_placed++;
        } else {
            break; // Can't place more
        }
    }
    
    printf("Curse of Greed: Placed %d fire cards on opponent's deck!\n", fire_cards_placed);
    
    // This creates a massive energy gain opportunity for Match Girl
    if (fire_cards_placed > 0) {
        printf("Match Girl will gain %d energy as opponent uses these fire cards\n", fire_cards_placed);
    }
}

//=============================================================================
// ENHANCED MATCH GIRL CHARACTER INITIALIZATION
// Shows setup of complex interdependent systems
//=============================================================================

void setup_enhanced_match_girl_player(Player* player) {
    if (!player) return;
    
    player->fable = &match_girl_enhanced_fable;
    player->health = match_girl_enhanced_fable.health;
    player->power = 0;
    player->defense = 0;
    
    // Initialize Match Girl's enhanced systems
    init_enhanced_match_girl_state(player);
    
    // Set up starting deck
    for (int i = 0; i < 3; i++) {
        add_deck(&player->draw, &Attack1);
        add_deck(&player->draw, &Defense1);
        add_deck(&player->draw, &Move1);
    }
    
    // Add starting skills
    add_deck(&player->draw, &make_a_wish);
    add_deck(&player->draw, &diabolical_offer);
    add_deck(&player->draw, &your_soul_weighed);
    
    shuffle_deck(&player->draw);
    draw_hand(player, HAND_SIZE);
    
    printf("Match Girl lights her first match! Energy accumulation system ready.\n");
    printf("Remember: Match Girl gains power when opponents use fire cards!\n");
}

//=============================================================================
// UTILITY FUNCTIONS FOR CROSS-PLAYER INTERACTIONS
// These help integrate Match Girl's systems with the broader game
//=============================================================================

bool match_girl_can_place_fire(Player* player) {
    MatchGirlState* mg_state = get_enhanced_match_girl_state(player);
    return mg_state && (mg_state->fire.fire_cards_placed_this_turn < mg_state->fire.max_fire_per_turn);
}

int match_girl_get_current_energy(Player* player) {
    MatchGirlState* mg_state = get_enhanced_match_girl_state(player);
    return mg_state ? mg_state->energy.current_energy : 0;
}

bool match_girl_can_enhance_with_energy(Player* player, int energy_cost) {
    MatchGirlState* mg_state = get_enhanced_match_girl_state(player);
    return mg_state && mg_state->energy.current_energy >= energy_cost;
}

// This function should be called by the main game when ANY fire card is used
void check_match_girl_energy_gain(Card* card_used, Player* card_user) {
    if (!card_used || card_used->val >= 0) return; // Not a fire card
    
    // Find Match Girl in the game (simplified - assumes 2 player game)
    // In full implementation, would have better player tracking
    printf("Fire card used - checking for Match Girl energy gain\n");
}

// Educational note: Match Girl's design teaches us about:
// 1. Resource accumulation from opponent actions - reactive power systems
// 2. Risk/reward mechanics - trading health for strategic advantage
// 3. Cross-player interaction - abilities that affect both players
// 4. Energy spending systems - flexible resource allocation
// These concepts are essential for any strategic game system!
