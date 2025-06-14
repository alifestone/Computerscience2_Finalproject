#include "snow_white.h"
#include "cards.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Snow White's poison system demonstrates EXTERNAL RESOURCE MANAGEMENT
// This is different from Alice's internal state - Snow White manages a separate
// resource pool (poison cards) that affects the opponent's deck directly
// This teaches us about:
// 1. Managing finite external resources
// 2. Deck manipulation mechanics
// 3. Indirect damage over time effects
// 4. Resource tracking and validation

// The Poison Card System: A Finite External Resource
// Unlike normal cards, poison cards come from a limited external supply
// This creates interesting strategic decisions about resource allocation
typedef struct PoisonCard {
    int level;           // 1, 2, or 3
    int damage_value;    // Damage when discarded
    bool in_use;         // Track if this poison card is currently placed
    char name[32];       // For debugging and display
} PoisonCard;

// Global poison supply - this represents the external resource pool
// In a full implementation, this would be managed by the game system
static PoisonCard poison_supply[18]; // 6 cards each of levels 1, 2, 3
static bool poison_supply_initialized = false;

// Snow White's extended state for tracking poison effects
typedef struct SnowWhiteState {
    int poison_cards_placed_this_turn;
    int max_poison_per_turn;
    int poison_damage_multiplier; // From "Pure Poison" twist cards
    bool corrupted_banquet_active; // Special ability state
    int mirror_rain_uses_remaining; // Epic card tracking
} SnowWhiteState;

// Forward declarations for Snow White's card effects
void crystal_fragment_effect(void* self, void* target);
void crystal_vortex_effect(void* self, void* target);
void crystal_storm_effect(void* self, void* target);
void tainted_blessing_effect(void* self, void* target);
void tainted_feast_effect(void* self, void* target);
void tainted_revelry_effect(void* self, void* target);
void broken_fantasy_effect(void* self, void* target);
void broken_reality_effect(void* self, void* target);
void broken_destiny_effect(void* self, void* target);

// Epic effects
void seven_snakes_wrath_effect(void* self, void* target);
void mirror_rain_effect(void* self, void* target);
void brewing_disaster_effect(void* self, void* target);

//=============================================================================
// POISON SUPPLY MANAGEMENT SYSTEM
// This demonstrates how to manage finite external resources
//=============================================================================

void init_poison_supply(void) {
    if (poison_supply_initialized) return;
    
    // Initialize 18 poison cards: 6 each of levels 1, 2, 3
    for (int level = 1; level <= 3; level++) {
        for (int i = 0; i < 6; i++) {
            int index = (level - 1) * 6 + i;
            poison_supply[index].level = level;
            poison_supply[index].damage_value = level; // Level = damage
            poison_supply[index].in_use = false;
            snprintf(poison_supply[index].name, sizeof(poison_supply[index].name), 
                    "Poison L%d #%d", level, i + 1);
        }
    }
    
    poison_supply_initialized = true;
    printf("Poison supply initialized: 18 cards (6 each of levels 1-3)\n");
}

PoisonCard* take_poison_card(int level) {
    if (!poison_supply_initialized) {
        init_poison_supply();
    }
    
    // Find first available poison card of requested level
    for (int i = (level - 1) * 6; i < level * 6; i++) {
        if (!poison_supply[i].in_use) {
            poison_supply[i].in_use = true;
            printf("Taking %s from supply\n", poison_supply[i].name);
            return &poison_supply[i];
        }
    }
    
    printf("No poison cards of level %d available!\n", level);
    return NULL; // No cards of this level available
}

void return_poison_card(PoisonCard* card) {
    if (!card) return;
    
    card->in_use = false;
    printf("Returning %s to supply\n", card->name);
}

int count_available_poison(int level) {
    if (!poison_supply_initialized) return 0;
    
    int count = 0;
    for (int i = (level - 1) * 6; i < level * 6; i++) {
        if (!poison_supply[i].in_use) count++;
    }
    return count;
}

//=============================================================================
// SNOW WHITE'S CARD DEFINITIONS
// Each card demonstrates different aspects of the poison system
//=============================================================================

// Attack cards that place poison cards in opponent's deck
Card crystal_fragment = {
    .name = "Crystal Fragment", .type = SKILL_ATK, .val = 1, .cst = 0, .dmg = 1,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = crystal_fragment_effect
};

Card crystal_vortex = {
    .name = "Crystal Vortex", .type = SKILL_ATK, .val = 2, .cst = 2, .dmg = 2,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = crystal_vortex_effect
};

Card crystal_storm = {
    .name = "Crystal Storm", .type = SKILL_ATK, .val = 3, .cst = 4, .dmg = 3,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = crystal_storm_effect
};

// Defense cards that also place poison (unusual design!)
Card tainted_blessing = {
    .name = "Tainted Blessing", .type = SKILL_DEF, .val = 1, .cst = 1, .dmg = 1,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = tainted_blessing_effect
};

Card tainted_feast = {
    .name = "Tainted Feast", .type = SKILL_DEF, .val = 2, .cst = 2, .dmg = 2,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = tainted_feast_effect
};

Card tainted_revelry = {
    .name = "Tainted Revelry", .type = SKILL_DEF, .val = 3, .cst = 4, .dmg = 3,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = tainted_revelry_effect
};

// Movement cards with positioning elements
Card broken_fantasy = {
    .name = "Broken Fantasy", .type = SKILL_MOV, .val = 1, .cst = 0, .dmg = 1,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = broken_fantasy_effect
};

Card broken_reality = {
    .name = "Broken Reality", .type = SKILL_MOV, .val = 2, .cst = 2, .dmg = 2,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = broken_reality_effect
};

Card broken_destiny = {
    .name = "Broken Destiny", .type = SKILL_MOV, .val = 3, .cst = 4, .dmg = 3,
    .defense = 0, .mov = 0, .rng = 2, .link = false, .effect = broken_destiny_effect
};

// Twist cards that enhance poison effects
Card crystal_coffin = {
    .name = "Crystal Coffin", .type = TWIST, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL
};

Card pure_poison = {
    .name = "Pure Poison", .type = TWIST, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL
};

Card corrupted_banquet = {
    .name = "Corrupted Banquet", .type = TWIST, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL
};

// Epic cards with major poison effects
Card seven_snakes_wrath = {
    .name = "Seven Snakes Wrath", .type = EPIC, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = seven_snakes_wrath_effect
};

Card mirror_rain = {
    .name = "Mirror Rain", .type = EPIC, .val = 0, .cst = 0, .dmg = 3,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = mirror_rain_effect
};

Card brewing_disaster = {
    .name = "Brewing Disaster", .type = EPIC, .val = 0, .cst = 0, .dmg = 3,
    .defense = 0, .mov = 0, .rng = 3, .link = false, .effect = brewing_disaster_effect
};

// Snow White's character data
Fable snow_white_fable;

//=============================================================================
// SNOW WHITE STATE MANAGEMENT
// This shows how to track character-specific resources and abilities
//=============================================================================

SnowWhiteState* get_snow_white_state(Player* player) {
    return (SnowWhiteState*)player->fable->skill; // Storing state in skill array
}

void init_snow_white_state(Player* player) {
    if (!player || !player->fable) return;
    
    SnowWhiteState* sw_state = malloc(sizeof(SnowWhiteState));
    memset(sw_state, 0, sizeof(SnowWhiteState));
    
    sw_state->max_poison_per_turn = 3; // Base limit
    sw_state->poison_damage_multiplier = 1; // Base multiplier
    
    player->fable->skill = (Deck*)sw_state;
}

//=============================================================================
// DECK MANIPULATION FUNCTIONS
// These demonstrate how to safely modify opponent's deck structure
//=============================================================================

bool place_poison_in_deck(Player* target, int poison_level, bool shuffle_in) {
    if (!target) return false;
    
    // Try to get poison card from supply
    PoisonCard* poison = take_poison_card(poison_level);
    if (!poison) return false;
    
    // Create a temporary card representation for the poison
    // In a full implementation, you'd have a proper poison card type
    Card* poison_card = malloc(sizeof(Card));
    memset(poison_card, 0, sizeof(Card));
    strncpy(poison_card->name, poison->name, sizeof(poison_card->name) - 1);
    poison_card->type = UNIVERSAL; // Special type for poison
    poison_card->dmg = poison->damage_value;
    poison_card->val = -poison_level; // Negative value to identify as poison
    
    if (shuffle_in) {
        // Shuffle into deck - more unpredictable but less immediate
        add_deck(&target->draw, poison_card);
        shuffle_deck(&target->draw);
        printf("Poison shuffled into %s's deck\n", target->fable->name);
    } else {
        // Place on top - more immediate threat
        // We need to shift cards down and place on top
        if (target->draw.cnt < MAX_CARD) {
            for (int i = target->draw.cnt; i > 0; i--) {
                target->draw.cards[i] = target->draw.cards[i-1];
            }
            target->draw.cards[0] = poison_card;
            target->draw.cnt++;
            printf("Poison placed on top of %s's deck\n", target->fable->name);
        }
    }
    
    return true;
}

int count_poison_in_discard(Player* player) {
    if (!player) return 0;
    
    int poison_count = 0;
    for (int i = 0; i < player->disc.cnt; i++) {
        Card* card = player->disc.cards[i];
        if (card && card->val < 0) { // Negative val indicates poison
            poison_count++;
        }
    }
    return poison_count;
}

void remove_poison_from_discard(Player* player, int max_remove) {
    if (!player) return;
    
    int removed = 0;
    for (int i = player->disc.cnt - 1; i >= 0 && removed < max_remove; i--) {
        Card* card = player->disc.cards[i];
        if (card && card->val < 0) { // This is a poison card
            // Return poison to supply
            // In full implementation, would track which poison card this is
            
            // Remove from discard pile
            for (int j = i; j < player->disc.cnt - 1; j++) {
                player->disc.cards[j] = player->disc.cards[j + 1];
            }
            player->disc.cnt--;
            
            free(card); // Clean up the temporary poison card
            removed++;
            
            printf("Poison card removed from discard pile\n");
        }
    }
}

//=============================================================================
// SNOW WHITE'S TURN MANAGEMENT
// Shows how to reset per-turn resources and track ongoing effects
//=============================================================================

void snow_white_turn_start(Player* player) {
    SnowWhiteState* sw_state = get_snow_white_state(player);
    if (!sw_state) return;
    
    // Reset per-turn counters
    sw_state->poison_cards_placed_this_turn = 0;
    
    printf("Snow White begins her turn. Poison cards available: L1:%d L2:%d L3:%d\n",
           count_available_poison(1), count_available_poison(2), count_available_poison(3));
}

void snow_white_turn_end(Player* player) {
    SnowWhiteState* sw_state = get_snow_white_state(player);
    if (!sw_state) return;
    
    // Process any end-of-turn poison effects
    if (sw_state->corrupted_banquet_active) {
        printf("Corrupted Banquet effect continues...\n");
    }
}

//=============================================================================
// SNOW WHITE'S CARD EFFECT IMPLEMENTATIONS
// These show different patterns of resource management and deck manipulation
//=============================================================================

void crystal_fragment_effect(void* self, void* target) {
    Player* snow_white = (Player*)self;
    Player* opponent = (Player*)target;
    SnowWhiteState* sw_state = get_snow_white_state(snow_white);
    
    // "Range 1, Damage 1+O, discard top 1 card from opponent's deck"
    
    // Discard top card from opponent's deck
    if (opponent->draw.cnt > 0) {
        Card* discarded = draw_deck(&opponent->draw);
        if (discarded) {
            add_deck(&opponent->disc, discarded);
            printf("Snow White discards top card from opponent's deck\n");
        }
    }
    
    // This is the basic poison attack - simple and direct
    printf("Crystal Fragment: Basic poison attack executed\n");
}

void crystal_vortex_effect(void* self, void* target) {
    Player* snow_white = (Player*)self;
    Player* opponent = (Player*)target;
    
    // "Range 1, Damage 2+O, discard top 2 cards from opponent's deck"
    
    // Discard multiple cards - more powerful version
    for (int i = 0; i < 2; i++) {
        if (opponent->draw.cnt > 0) {
            Card* discarded = draw_deck(&opponent->draw);
            if (discarded) {
                add_deck(&opponent->disc, discarded);
            }
        }
    }
    
    printf("Crystal Vortex: Enhanced deck disruption\n");
}

void crystal_storm_effect(void* self, void* target) {
    Player* snow_white = (Player*)self;
    Player* opponent = (Player*)target;
    SnowWhiteState* sw_state = get_snow_white_state(snow_white);
    
    // "Range 1, Damage 3+O, discard top 3 cards from opponent's deck"
    // Also triggers Crystal Coffin twist if available
    
    for (int i = 0; i < 3; i++) {
        if (opponent->draw.cnt > 0) {
            Card* discarded = draw_deck(&opponent->draw);
            if (discarded) {
                add_deck(&opponent->disc, discarded);
            }
        }
    }
    
    // Check for Crystal Coffin activation (deals 2+ damage)
    if (crystal_storm.dmg >= 2) {
        printf("Crystal Coffin triggered! Placing poison card...\n");
        place_poison_in_deck(opponent, 1, false); // Place level 1 poison on top
    }
    
    printf("Crystal Storm: Maximum deck disruption\n");
}

void tainted_blessing_effect(void* self, void* target) {
    Player* snow_white = (Player*)self;
    Player* opponent = (Player*)target;
    SnowWhiteState* sw_state = get_snow_white_state(snow_white);
    
    // "Range 1, Damage 1, place up to O poison cards in opponent's discard"
    // This is unusual - a defense card that places poison!
    
    int max_poison = 1; // Level 1 defense skill
    for (int i = 0; i < max_poison && sw_state->poison_cards_placed_this_turn < sw_state->max_poison_per_turn; i++) {
        if (place_poison_in_deck(opponent, 1, false)) {
            sw_state->poison_cards_placed_this_turn++;
        }
    }
    
    printf("Tainted Blessing: Defense with poison placement\n");
}

void broken_fantasy_effect(void* self, void* target) {
    Player* snow_white = (Player*)self;
    Player* opponent = (Player*)target;
    
    // "Range O, Damage 1, move to adjacent position of opponent"
    // "Can lose 1 life to remove 1 card from hand/discard"
    
    // Move to adjacent position (would need position system implementation)
    snow_white->pos = opponent->pos + ((snow_white->pos < opponent->pos) ? -1 : 1);
    snow_white->pos = MAX(-4, MIN(4, snow_white->pos)); // Clamp to valid range
    
    // Option to remove card by losing life
    printf("Snow White may lose 1 life to remove a card\n");
    // In full implementation, would give player choice
    
    printf("Broken Fantasy: Repositioning with card removal option\n");
}

void seven_snakes_wrath_effect(void* self, void* target) {
    Player* snow_white = (Player*)self;
    Player* opponent = (Player*)target;
    
    // "Range 1, opponent loses X life where X = poison cards in their discard pile"
    
    int poison_count = count_poison_in_discard(opponent);
    if (poison_count > 0) {
        opponent->health -= poison_count;
        printf("Seven Snakes Wrath: %d poison cards deal %d life loss!\n", poison_count, poison_count);
    } else {
        printf("Seven Snakes Wrath: No poison cards in discard pile\n");
    }
}

void mirror_rain_effect(void* self, void* target) {
    Player* snow_white = (Player*)self;
    Player* opponent = (Player*)target;
    SnowWhiteState* sw_state = get_snow_white_state(snow_white);
    
    // "Range 1, Damage 3, opponent discards all hand cards and draws 4"
    
    // Discard all hand cards
    while (opponent->hand.cnt > 0) {
        Card* discarded = opponent->hand.cards[opponent->hand.cnt - 1];
        opponent->hand.cnt--;
        add_deck(&opponent->disc, discarded);
    }
    
    // Draw 4 new cards
    draw_hand(opponent, 4);
    
    printf("Mirror Rain: Complete hand disruption!\n");
}

void brewing_disaster_effect(void* self, void* target) {
    Player* snow_white = (Player*)self;
    Player* opponent = (Player*)target;
    
    // "Range 3, Damage 3, choose up to 3 cards from opponent's discard to shuffle into their deck"
    
    int cards_shuffled = 0;
    for (int i = 0; i < opponent->disc.cnt && cards_shuffled < 3; i++) {
        Card* card = opponent->disc.cards[i];
        if (card) {
            // Remove from discard
            for (int j = i; j < opponent->disc.cnt - 1; j++) {
                opponent->disc.cards[j] = opponent->disc.cards[j + 1];
            }
            opponent->disc.cnt--;
            
            // Add to deck and shuffle
            add_deck(&opponent->draw, card);
            cards_shuffled++;
            i--; // Adjust index since we removed a card
        }
    }
    
    if (cards_shuffled > 0) {
        shuffle_deck(&opponent->draw);
        printf("Brewing Disaster: Shuffled %d cards back into deck\n", cards_shuffled);
    }
}

//=============================================================================
// SNOW WHITE CHARACTER INITIALIZATION
// Shows how to set up a character with external resource dependencies
//=============================================================================

void init_snow_white_fable(void) {
    // Initialize the poison supply system first
    init_poison_supply();
    
    snow_white_fable = (Fable){
        .name = "Snow White",
        .Piece = {255, 255, 255, 255}, // Pure white
        .health = 34,
        .energy = 0,
        .defense = 6,
        .epic_threshold = 17,
        .lane = 0
    };
    
    // Set up skill decks
    snow_white_fable.skill[0] = (Deck){ // Attack skills
        .cards = {&crystal_fragment, &crystal_vortex, &crystal_vortex, &crystal_storm, &crystal_storm},
        .cnt = 5
    };
    
    snow_white_fable.skill[1] = (Deck){ // Defense skills  
        .cards = {&tainted_blessing, &tainted_feast, &tainted_feast, &tainted_revelry, &tainted_revelry},
        .cnt = 5
    };
    
    snow_white_fable.skill[2] = (Deck){ // Movement skills
        .cards = {&broken_fantasy, &broken_reality, &broken_reality, &broken_destiny, &broken_destiny},
        .cnt = 5
    };
    
    // Epic cards
    snow_white_fable.epic[0] = seven_snakes_wrath;
    snow_white_fable.epic[1] = mirror_rain;
    snow_white_fable.epic[2] = brewing_disaster;
}

void setup_snow_white_player(Player* player) {
    if (!player) return;
    
    init_snow_white_fable();
    player->fable = &snow_white_fable;
    player->health = snow_white_fable.health;
    player->power = 0;
    player->defense = 0;
    player->pos = snow_white_fable.lane;
    
    // Initialize Snow White's state
    init_snow_white_state(player);
    
    // Set up starting deck
    for (int i = 0; i < 3; i++) {
        add_deck(&player->draw, &Attack1);
        add_deck(&player->draw, &Defense1);
        add_deck(&player->draw, &Move1);
    }
    
    // Add starting skills
    add_deck(&player->draw, &crystal_fragment);
    add_deck(&player->draw, &tainted_blessing);
    add_deck(&player->draw, &broken_fantasy);
    
    shuffle_deck(&player->draw);
    draw_hand(player, HAND_SIZE);
    
    printf("Snow White awakens! The poison supply is ready...\n");
}

//=============================================================================
// UTILITY FUNCTIONS FOR POISON MANAGEMENT
// These help other parts of the game interact with Snow White's systems
//=============================================================================

bool snow_white_can_place_poison(Player* player) {
    SnowWhiteState* sw_state = get_snow_white_state(player);
    return sw_state && (sw_state->poison_cards_placed_this_turn < sw_state->max_poison_per_turn);
}

void on_opponent_uses_poison_card(Player* snow_white_player, Card* poison_card) {
    if (!snow_white_player || !poison_card) return;
    
    SnowWhiteState* sw_state = get_snow_white_state(snow_white_player);
    if (!sw_state) return;
    
    // Apply "Pure Poison" twist effect if active
    int bonus_damage = (sw_state->poison_damage_multiplier - 1);
    if (bonus_damage > 0) {
        printf("Pure Poison triggers! +%d damage from poison card\n", bonus_damage);
        // Would apply additional damage to opponent here
    }
}

// This demonstrates how external resources need cleanup
void cleanup_snow_white_resources(void) {
    // Return all poison cards to supply
    for (int i = 0; i < 18; i++) {
        poison_supply[i].in_use = false;
    }
    printf("Snow White's poison supply cleaned up\n");
}