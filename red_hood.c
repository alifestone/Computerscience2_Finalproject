#include "red_hood.h"
#include "cards.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Enhanced Red Hood demonstrates CLEAN SYSTEM DESIGN and RANGE-BASED COMBAT
// Sometimes the best programming is the simplest that gets the job done effectively.
// Red Hood teaches us:
// 1. Clean, readable code structure - easy to understand and maintain
// 2. Range-based combat mechanics - spatial tactical gameplay
// 3. Equipment/tech systems - modular ability enhancement
// 4. Straightforward but powerful abilities - elegance through simplicity
// 5. Balanced design - not every character needs complex mechanics

// Red Hood's tech/equipment system - simple but effective
typedef struct TechSystem {
    bool overcharge_available;         // Can spend extra energy for bonuses
    int tech_charges;                  // Accumulated tech charges
    int max_tech_charges;              // Maximum charges storable
    bool targeting_system_active;      // Enhanced range/accuracy
    bool armor_system_active;          // Enhanced defense
} TechSystem;

// Range combat system - Red Hood's specialty
typedef struct RangeSystem {
    int base_range;                    // Character's base attack range
    int range_bonuses;                 // Temporary range increases
    bool sniper_mode_active;           // Extended range mode
    int shots_fired_this_turn;         // Track for balance
    int max_shots_per_turn;            // Rapid fire limitation
} RangeSystem;

// Red Hood's enhanced state - focused but powerful
typedef struct RedHoodState {
    TechSystem tech;
    RangeSystem range;
    bool overdrive_burn_active;        // Twist effect for discarding skills
    bool onboard_cache_cards;          // Stored cards system
    int consecutive_tech_uses;         // Momentum tracking
    Card* cached_card;                 // Onboard cache storage
} RedHoodState;

// Forward declarations for enhanced Red Hood effects
void pot_shot_enhanced_effect(void* self, void* target);
void crack_shot_enhanced_effect(void* self, void* target);
void sniper_shot_enhanced_effect(void* self, void* target);
void energy_shield_enhanced_effect(void* self, void* target);
void electrified_shield_enhanced_effect(void* self, void* target);
void omega_shield_enhanced_effect(void* self, void* target);
void unleashed_ballistics_enhanced_effect(void* self, void* target);
void unleashed_firepower_enhanced_effect(void* self, void* target);
void unleashed_fury_enhanced_effect(void* self, void* target);

// Epic effects
void hungry_wolf_devour_effect(void* self, void* target);
void system_intrusion_effect(void* self, void* target);
void revenge_rain_effect(void* self, void* target);

//=============================================================================
// TECH SYSTEM - SIMPLE BUT EFFECTIVE ENHANCEMENT MECHANICS
// This shows how to build powerful systems with clean, understandable code
//=============================================================================

void init_tech_system(TechSystem* tech) {
    if (!tech) return;
    
    tech->overcharge_available = true;
    tech->tech_charges = 0;
    tech->max_tech_charges = 5;           // Reasonable limit for balance
    tech->targeting_system_active = false;
    tech->armor_system_active = false;
    
    printf("Tech system initialized - Red Hood's equipment ready\n");
}

void gain_tech_charge(RedHoodState* rh_state, int amount, const char* source) {
    if (!rh_state) return;
    
    int old_charges = rh_state->tech.tech_charges;
    rh_state->tech.tech_charges = MIN(rh_state->tech.tech_charges + amount, 
                                     rh_state->tech.max_tech_charges);
    int gained = rh_state->tech.tech_charges - old_charges;
    
    if (gained > 0) {
        printf("Red Hood gains %d tech charges from %s (now %d/%d)\n", 
               gained, source, rh_state->tech.tech_charges, rh_state->tech.max_tech_charges);
        
        rh_state->consecutive_tech_uses++;
    }
}

bool spend_tech_charge(RedHoodState* rh_state, int amount, const char* purpose) {
    if (!rh_state || rh_state->tech.tech_charges < amount) {
        printf("Insufficient tech charges for %s (need %d, have %d)\n", 
               purpose, amount, rh_state->tech.tech_charges);
        return false;
    }
    
    rh_state->tech.tech_charges -= amount;
    printf("Red Hood spends %d tech charges for %s (remaining: %d)\n", 
           amount, purpose, rh_state->tech.tech_charges);
    return true;
}

//=============================================================================
// RANGE SYSTEM - SPATIAL TACTICAL COMBAT
// Shows how to implement position-based gameplay mechanics
//=============================================================================

void init_range_system(RangeSystem* range) {
    if (!range) return;
    
    range->base_range = 1;                // Red Hood starts with range 1
    range->range_bonuses = 0;
    range->sniper_mode_active = false;
    range->shots_fired_this_turn = 0;
    range->max_shots_per_turn = 3;        // Balance limitation
}

int calculate_total_range(RedHoodState* rh_state) {
    if (!rh_state) return 1;
    
    int total_range = rh_state->range.base_range + rh_state->range.range_bonuses;
    
    // Sniper mode doubles range
    if (rh_state->range.sniper_mode_active) {
        total_range *= 2;
    }
    
    // Targeting system adds precision (can be interpreted as effective range)
    if (rh_state->tech.targeting_system_active) {
        total_range += 1;
    }
    
    return total_range;
}

bool can_hit_target(Player* red_hood, Player* target, RedHoodState* rh_state, int weapon_range) {
    if (!red_hood || !target || !rh_state) return false;
    
    int distance = abs(red_hood->pos - target->pos);
    int effective_range = calculate_total_range(rh_state);
    
    // Use weapon range or character range, whichever is better
    int final_range = MAX(weapon_range, effective_range);
    
    bool can_hit = (distance <= final_range);
    
    if (!can_hit) {
        printf("Target out of range (distance: %d, range: %d)\n", distance, final_range);
    }
    
    return can_hit;
}

//=============================================================================
// ENHANCED RED HOOD CARD DEFINITIONS
// Each card demonstrates clean, effective design principles
//=============================================================================

// Attack cards with range focus
Card pot_shot_enhanced = {
    .name = "Pot Shot", .type = SKILL_ATK, .val = 1, .cst = 0, .dmg = 1,
    .defense = 0, .mov = 0, .rng = 1, .link = true, .effect = pot_shot_enhanced_effect
};

Card crack_shot_enhanced = {
    .name = "Crack Shot", .type = SKILL_ATK, .val = 2, .cst = 2, .dmg = 2,
    .defense = 0, .mov = 0, .rng = 2, .link = true, .effect = crack_shot_enhanced_effect
};

Card sniper_shot_enhanced = {
    .name = "Sniper Shot", .type = SKILL_ATK, .val = 3, .cst = 4, .dmg = 3,
    .defense = 0, .mov = 0, .rng = 3, .link = true, .effect = sniper_shot_enhanced_effect
};

// Defense cards with tech integration
Card energy_shield_enhanced = {
    .name = "Energy Shield", .type = SKILL_DEF, .val = 1, .cst = 1, .dmg = 1,
    .defense = 3, .mov = 0, .rng = 0, .link = false, .effect = energy_shield_enhanced_effect
};

Card electrified_shield_enhanced = {
    .name = "Electrified Shield", .type = SKILL_DEF, .val = 2, .cst = 2, .dmg = 2,
    .defense = 4, .mov = 0, .rng = 0, .link = false, .effect = electrified_shield_enhanced_effect
};

Card omega_shield_enhanced = {
    .name = "Omega Shield", .type = SKILL_DEF, .val = 3, .cst = 3, .dmg = 3,
    .defense = 6, .mov = 0, .rng = 0, .link = false, .effect = omega_shield_enhanced_effect
};

// Movement cards with tactical positioning
Card unleashed_ballistics_enhanced = {
    .name = "Unleashed Ballistics", .type = SKILL_MOV, .val = 1, .cst = 1, .dmg = 0,
    .defense = 0, .mov = 2, .rng = 0, .link = false, .effect = unleashed_ballistics_enhanced_effect
};

Card unleashed_firepower_enhanced = {
    .name = "Unleashed Firepower", .type = SKILL_MOV, .val = 2, .cst = 2, .dmg = 0,
    .defense = 0, .mov = 3, .rng = 0, .link = false, .effect = unleashed_firepower_enhanced_effect
};

Card unleashed_fury_enhanced = {
    .name = "Unleashed Fury", .type = SKILL_MOV, .val = 3, .cst = 3, .dmg = 0,
    .defense = 0, .mov = 4, .rng = 0, .link = false, .effect = unleashed_fury_enhanced_effect
};

// Special tech cards
Card overdrive_burn_enhanced = {
    .name = "Overdrive Burn", .type = TWIST, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL
};

Card onboard_cache_enhanced = {
    .name = "Onboard Cache", .type = TWIST, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL
};

// Epic cards with devastating effects
Card hungry_wolf_devour = {
    .name = "Hungry Wolf Devour", .type = EPIC, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = hungry_wolf_devour_effect
};

Card system_intrusion = {
    .name = "System Intrusion", .type = EPIC, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = system_intrusion_effect
};

Card revenge_rain = {
    .name = "Revenge Rain", .type = EPIC, .val = 0, .cst = 0, .dmg = 3,
    .defense = 0, .mov = 0, .rng = 3, .link = false, .effect = revenge_rain_effect
};

Fable red_hood_enhanced_fable;

//=============================================================================
// RED HOOD STATE MANAGEMENT
// Shows clean, straightforward state management
//=============================================================================

RedHoodState* get_red_hood_state(Player* player) {
    return (RedHoodState*)player->fable->skill;
}

void init_red_hood_state(Player* player) {
    if (!player || !player->fable) return;
    
    RedHoodState* rh_state = malloc(sizeof(RedHoodState));
    memset(rh_state, 0, sizeof(RedHoodState));
    
    // Initialize subsystems
    init_tech_system(&rh_state->tech);
    init_range_system(&rh_state->range);
    
    rh_state->overdrive_burn_active = false;
    rh_state->onboard_cache_cards = false;
    rh_state->consecutive_tech_uses = 0;
    rh_state->cached_card = NULL;
    
    player->fable->character_state = rh_state;
    
    printf("Red Hood's combat systems online - ready for tactical combat!\n");
}

//=============================================================================
// TURN MANAGEMENT - CLEAN AND SIMPLE
// Shows that not every system needs to be complex
//=============================================================================

void red_hood_turn_start(Player* player) {
    RedHoodState* rh_state = get_red_hood_state(player);
    if (!rh_state) return;
    
    // Reset turn-based counters
    rh_state->range.shots_fired_this_turn = 0;
    
    // Gain tech charge each turn (simple resource generation)
    gain_tech_charge(rh_state, 1, "turn start");
    
    printf("Red Hood begins turn - Tech charges: %d/%d, Range: %d\n",
           rh_state->tech.tech_charges, rh_state->tech.max_tech_charges,
           calculate_total_range(rh_state));
}

void red_hood_turn_end(Player* player) {
    RedHoodState* rh_state = get_red_hood_state(player);
    if (!rh_state) return;
    
    // Check tech momentum
    if (rh_state->consecutive_tech_uses >= 3) {
        printf("Red Hood's tech systems are running hot! (+1 range next turn)\n");
        rh_state->range.range_bonuses += 1;
    }
    
    // Reset temporary bonuses
    rh_state->range.sniper_mode_active = false;
    rh_state->tech.targeting_system_active = false;
}

//=============================================================================
// ENHANCED CARD EFFECT IMPLEMENTATIONS
// These demonstrate clean, effective ability design
//=============================================================================

void pot_shot_enhanced_effect(void* self, void* target) {
    Player* red_hood = (Player*)self;
    Player* opponent = (Player*)target;
    RedHoodState* rh_state = get_red_hood_state(red_hood);
    
    // "Range 1, Damage 1+O" - Simple but effective
    
    int base_damage = 1;
    
    if (can_hit_target(red_hood, opponent, rh_state, 1)) {
        opponent->health -= base_damage;
        rh_state->range.shots_fired_this_turn++;
        
        // Gain tech charge for successful hit
        gain_tech_charge(rh_state, 1, "successful shot");
        
        printf("Pot Shot hits for %d damage\n", base_damage);
    }
}

void crack_shot_enhanced_effect(void* self, void* target) {
    Player* red_hood = (Player*)self;
    Player* opponent = (Player*)target;
    RedHoodState* rh_state = get_red_hood_state(red_hood);
    
    // "Range 2, Damage 2+O" - Enhanced range and damage
    
    int base_damage = 2;
    
    if (can_hit_target(red_hood, opponent, rh_state, 2)) {
        opponent->health -= base_damage;
        rh_state->range.shots_fired_this_turn++;
        
        // Better tech charge gain for higher level shots
        gain_tech_charge(rh_state, 2, "crack shot");
        
        printf("Crack Shot hits for %d damage at range 2\n", base_damage);
    }
}

void sniper_shot_enhanced_effect(void* self, void* target) {
    Player* red_hood = (Player*)self;
    Player* opponent = (Player*)target;
    RedHoodState* rh_state = get_red_hood_state(red_hood);
    
    // "Range 3, Damage 3+O" - Maximum range and power
    
    int base_damage = 3;
    
    // Sniper shot activates sniper mode
    rh_state->range.sniper_mode_active = true;
    
    if (can_hit_target(red_hood, opponent, rh_state, 3)) {
        opponent->health -= base_damage;
        rh_state->range.shots_fired_this_turn++;
        
        // Maximum tech charge gain
        gain_tech_charge(rh_state, 3, "sniper shot");
        
        printf("Sniper Shot hits for %d damage at maximum range!\n", base_damage);
    }
}

void energy_shield_enhanced_effect(void* self, void* target) {
    Player* red_hood = (Player*)self;
    RedHoodState* rh_state = get_red_hood_state(red_hood);
    
    // "Range 1, Damage 1, Defense O"
    // "Persistent: Next turn start, if defense > 0, deal 2 damage to range 1"
    
    int defense_gain = 3; // From card definition
    red_hood->defense = MIN(red_hood->defense + defense_gain, red_hood->fable->defense);
    
    // Activate armor system
    rh_state->tech.armor_system_active = true;
    
    printf("Energy Shield: +%d defense and armor system activated\n", defense_gain);
    
    // Set up persistent effect for next turn
    // In full implementation, would add to effect system
}

void unleashed_ballistics_enhanced_effect(void* self, void* target) {
    Player* red_hood = (Player*)self;
    RedHoodState* rh_state = get_red_hood_state(red_hood);
    
    // "Movement 2" - Enhanced tactical positioning
    
    int movement_range = 2;
    
    // Red Hood can move tactically and gain range bonus
    rh_state->range.range_bonuses += 1; // Tactical positioning bonus
    
    printf("Unleashed Ballistics: Enhanced movement and +1 range bonus\n");
    
    // In full implementation, would handle movement UI
}

void hungry_wolf_devour_effect(void* self, void* target) {
    Player* red_hood = (Player*)self;
    RedHoodState* rh_state = get_red_hood_state(red_hood);
    
    // "Add 2 skill cards from skill supply to hand"
    // "This is not a purchase action"
    
    // Simulate adding cards from skill supply
    // In full implementation, would have proper skill supply system
    draw_hand(red_hood, 2);
    
    printf("Hungry Wolf Devour: Red Hood gains 2 skill cards from supply!\n");
}

void system_intrusion_effect(void* self, void* target) {
    Player* red_hood = (Player*)self;
    Player* opponent = (Player*)target;
    RedHoodState* rh_state = get_red_hood_state(red_hood);
    
    // "Repeat the effect of an action or attack/movement skill used this turn"
    // "Including twist card effects"
    
    // This is complex to implement fully, but we can simulate
    printf("System Intrusion: Red Hood hacks the system to repeat last action!\n");
    
    // In full implementation, would track and repeat last used effect
    // For demo, just give bonus tech charges
    gain_tech_charge(rh_state, 5, "system intrusion");
}

void revenge_rain_effect(void* self, void* target) {
    Player* red_hood = (Player*)self;
    Player* opponent = (Player*)target;
    RedHoodState* rh_state = get_red_hood_state(red_hood);
    
    // "Range 3, Damage 3, knock opponent back up to 3 spaces"
    // "Opponent must discard 3 hand cards"
    
    int damage = 3;
    int knockback = 3;
    
    if (can_hit_target(red_hood, opponent, rh_state, 3)) {
        // Deal damage
        opponent->health -= damage;
        
        // Knockback
        int direction = (red_hood->pos < opponent->pos) ? 1 : -1;
        opponent->pos += direction * knockback;
        opponent->pos = MAX(-4, MIN(4, opponent->pos)); // Clamp to battlefield
        
        // Force discard
        int cards_to_discard = MIN(3, opponent->hand.cnt);
        for (int i = 0; i < cards_to_discard; i++) {
            if (opponent->hand.cnt > 0) {
                Card* discarded = opponent->hand.cards[opponent->hand.cnt - 1];
                opponent->hand.cnt--;
                add_deck(&opponent->disc, discarded);
            }
        }
        
        printf("Revenge Rain: %d damage, %d knockback, %d cards discarded!\n", 
               damage, knockback, cards_to_discard);
    }
}

//=============================================================================
// RED HOOD CHARACTER INITIALIZATION
// Shows clean setup of a straightforward but effective character
//=============================================================================

void init_red_hood_enhanced_fable(void) {
    red_hood_enhanced_fable = (Fable){
        .name = "Red Hood",
        .Piece = {255, 0, 0, 255}, // Classic red
        .health = 30,
        .energy = 25, // Uses energy for tech systems
        .defense = 6,
        .epic_threshold = 15,
        .lane = 0
    };
    
    // Set up skill decks emphasizing range and tech
    red_hood_enhanced_fable.skill[0] = (Deck){ // Attack skills with range scaling
        .cards = {&pot_shot_enhanced, &crack_shot_enhanced, &crack_shot_enhanced,
                 &sniper_shot_enhanced, &sniper_shot_enhanced},
        .cnt = 5
    };
    
    red_hood_enhanced_fable.skill[1] = (Deck){ // Defense skills with tech integration
        .cards = {&energy_shield_enhanced, &electrified_shield_enhanced, 
                 &electrified_shield_enhanced, &omega_shield_enhanced, &omega_shield_enhanced},
        .cnt = 5
    };
    
    red_hood_enhanced_fable.skill[2] = (Deck){ // Movement skills with tactical bonuses
        .cards = {&unleashed_ballistics_enhanced, &unleashed_firepower_enhanced, 
                 &unleashed_firepower_enhanced, &unleashed_fury_enhanced, &unleashed_fury_enhanced},
        .cnt = 5
    };
    
    // Epic cards with powerful range effects
    red_hood_enhanced_fable.epic[0] = hungry_wolf_devour;
    red_hood_enhanced_fable.epic[1] = system_intrusion;
    red_hood_enhanced_fable.epic[2] = revenge_rain;
}

void setup_red_hood_enhanced_player(Player* player) {
    if (!player) return;
    
    init_red_hood_enhanced_fable();
    player->fable = &red_hood_enhanced_fable;
    player->health = red_hood_enhanced_fable.health;
    player->power = 0;
    player->defense = 0;
    player->pos = red_hood_enhanced_fable.lane;
    
    // Initialize Red Hood's tech systems
    init_red_hood_state(player);
    
    // Set up starting deck with balanced composition
    for (int i = 0; i < 3; i++) {
        add_deck(&player->draw, &Attack1);
        add_deck(&player->draw, &Defense1);
        add_deck(&player->draw, &Move1);
    }
    
    // Add starting skills for immediate impact
    add_deck(&player->draw, &pot_shot_enhanced);
    add_deck(&player->draw, &energy_shield_enhanced);
    add_deck(&player->draw, &unleashed_ballistics_enhanced);
    
    shuffle_deck(&player->draw);
    draw_hand(player, HAND_SIZE);
    
    printf("Red Hood locks and loads! Tech systems online and ready for combat.\n");
    printf("Remember: Red Hood excels at range and can hit targets others can't reach!\n");
}

//=============================================================================
// UTILITY FUNCTIONS FOR RANGE COMBAT
// These help integrate Red Hood's systems with the broader game
//=============================================================================

bool red_hood_can_hit_at_range(Player* player, int target_distance) {
    RedHoodState* rh_state = get_red_hood_state(player);
    return rh_state && (calculate_total_range(rh_state) >= target_distance);
}

int red_hood_get_current_range(Player* player) {
    RedHoodState* rh_state = get_red_hood_state(player);
    return rh_state ? calculate_total_range(rh_state) : 1;
}

bool red_hood_can_use_tech(Player* player, int tech_cost) {
    RedHoodState* rh_state = get_red_hood_state(player);
    return rh_state && rh_state->tech.tech_charges >= tech_cost;
}

void red_hood_activate_overdrive(Player* player) {
    RedHoodState* rh_state = get_red_hood_state(player);
    if (!rh_state) return;
    
    // Overdrive: spend skill cards for immediate power boost
    rh_state->overdrive_burn_active = true;
    printf("Red Hood activates overdrive! Can discard skills for immediate power!\n");
}

// Educational note: Red Hood's design demonstrates that:
// 1. Clean, simple code can be just as powerful as complex systems
// 2. Range-based mechanics add tactical depth without complexity
// 3. Sometimes the best design is the one that's easy to understand and use
// 4. Straightforward systems can still be engaging and fun
// These lessons apply to all software development - elegance through simplicity!
