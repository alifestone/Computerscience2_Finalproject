#include "alice.h"
#include "cards.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Alice's identity system represents a state machine within a character
// This demonstrates how to implement complex character transformations
// while maintaining clean separation of concerns

/*typedef enum {
    IDENTITY_RED_QUEEN,    // +1 attack, -1 movement
    IDENTITY_MAD_HATTER,   // +1 defense, -1 attack  
    IDENTITY_CHESHIRE_CAT, // +1 movement, -1 defense
    IDENTITY_COUNT
} AliceIdentity;*/

// Character-specific state that extends the base Player structure
typedef struct AliceState {
    AliceIdentity current_identity;
    AliceIdentity previous_identity;
    int identity_cooldowns[3];
    bool must_switch_identity; // Rule: must switch each turn
    int cards_gained_this_turn;
    int basic_cards_removed_this_turn;
} AliceState;

// Identity-specific card modifications
typedef struct IdentityModifier {
    const char* name;
    int attack_modifier;
    int defense_modifier; 
    int movement_modifier;
    SDL_Color identity_color;
} IdentityModifier;

// Static data for each identity - this encapsulates the identity rules
static const IdentityModifier identity_data[3] = {
    {"Red Queen",    1, 0, -1, {255, 0, 0, 255}},   // Red for aggression
    {"Mad Hatter",   -1, 1, 0, {128, 0, 128, 255}}, // Purple for madness
    {"Cheshire Cat", 0, -1, 1, {255, 192, 203, 255}} // Pink for mischief
};

// Function declarations for Alice's card effects
void deal_the_cards_effect(void* self, void* target);
void cut_the_cards_effect(void* self, void* target);
void rig_the_cards_effect(void* self, void* target);
void magic_trick_effect(void* self, void* target);
void mind_trick_effect(void* self, void* target);
void hat_trick_effect(void* self, void* target);
void curiously_agile_effect(void* self, void* target);
void curiously_covert_effect(void* self, void* target);
void curiously_curious_effect(void* self, void* target);

// Epic card effects
void important_date_effect(void* self, void* target);
void frabjous_day_effect(void* self, void* target);
void the_games_afoot_effect(void* self, void* target);

//=============================================================================
// ALICE'S CARD DEFINITIONS
// Each card implements the complex mechanics described in the documentation
//=============================================================================

// Attack cards with identity synergy
Card deal_the_cards = {
    .name = "Deal the Cards", .type = SKILL_ATK, .val = 1, .cst = 0, .dmg = 1, 
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = deal_the_cards_effect
};

Card cut_the_cards = {
    .name = "Cut the Cards", .type = SKILL_ATK, .val = 2, .cst = 2, .dmg = 2,
    .defense = 0, .mov = 0, .rng = 2, .link = false, .effect = cut_the_cards_effect
};

Card rig_the_cards = {
    .name = "Rig the cards", .type = SKILL_ATK, .val = 3, .cst = 4, .dmg = 3,
    .defense = 0, .mov = 0, .rng = 3, .link = false, .effect = rig_the_cards_effect
};

// Defense cards with card manipulation
Card magic_trick = {
    .name = "Magic Trick", .type = SKILL_DEF, .val = 1, .cst = 1, .dmg = 0,
    .defense = 1, .mov = 0, .rng = 0, .link = false, .effect = magic_trick_effect
};

Card mind_trick = {
    .name = "Mind Trick", .type = SKILL_DEF, .val = 2, .cst = 2, .dmg = 0,
    .defense = 2, .mov = 0, .rng = 0, .link = false, .effect = mind_trick_effect
};

Card hat_trick = {
    .name = "Hat Trick", .type = SKILL_DEF, .val = 3, .cst = 4, .dmg = 0,
    .defense = 3, .mov = 0, .rng = 0, .link = false, .effect = hat_trick_effect
};

// Movement cards with card draw synergy
Card curiously_agile = {
    .name = "Curiously Agile", .type = SKILL_MOV, .val = 1, .cst = 1, .dmg = 0,
    .defense = 0, .mov = 1, .rng = 0, .link = false, .effect = curiously_agile_effect
};

Card curiously_covert = {
    .name = "Curiously Covert", .type = SKILL_MOV, .val = 2, .cst = 2, .dmg = 0,
    .defense = 0, .mov = 2, .rng = 0, .link = false, .effect = curiously_covert_effect
};

Card curiously_curious = {
    .name = "Curiously Curious  ", .type = SKILL_MOV, .val = 3, .cst = 4, .dmg = 0,
    .defense = 0, .mov = 3, .rng = 0, .link = false, .effect = curiously_curious_effect
};

// Twist cards that modify identity rules
Card off_with_her_head = {
    .name = "Off With Her Head", .type = TWIST, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL
};

Card wonderland = {
    .name = "Wonderland", .type = TWIST, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL
};

Card were_all_mad_here = {
    .name = "We're All Mad Here", .type = TWIST, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL
};

Card its_showtime = {
    .name = "It's Showtime", .type = TWIST, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = NULL
};

// Epic cards with game-changing effects
Card important_date = {
    .name = "Important Date", .type = EPIC, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = important_date_effect
};

Card frabjous_day = {
    .name = "Frabjous Day", .type = EPIC, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = frabjous_day_effect
};

Card the_games_afoot = {
    .name = "The Game's Afoot", .type = EPIC, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 3, .link = false, .effect = the_games_afoot_effect
};

// Alice's complete character definition
Fable alice_fable={
    "Alice",{0,150,255,255},32,0,6,16,
    .skill={
        {
            .cards={&deal_the_cards,&cut_the_cards,&off_with_her_head,&rig_the_cards,&its_showtime},
            .cnt=5
        },
        {
            .cards={&magic_trick,&mind_trick,&wonderland,&hat_trick,&its_showtime},
            .cnt=5
        },
        {
            .cards={&curiously_agile,&curiously_covert,&were_all_mad_here,&curiously_curious,&its_showtime},
            .cnt=5
        }
    },
    .epic={&important_date,&frabjous_day,&the_games_afoot}
};

//=============================================================================
// ALICE STATE MANAGEMENT FUNCTIONS
// These functions handle the complex identity switching mechanics
//=============================================================================

AliceState* get_alice_state(Player* player) {
    // We store Alice's extended state in the player's fable data
    // This is a common pattern for character-specific state management
    return (AliceState*)player->fable->character_state; // Reusing skill array for state storage
}

void init_alice_state(Player* player) {
    if (!player || !player->fable) return;
    
    AliceGameState* alice_state = malloc(sizeof(AliceGameState));
    memset(alice_state, 0, sizeof(AliceGameState));
    
    // Set initial identity (player chooses at game start)
    alice_state->current_identity = ALICE_IDENTITY_QUEEN_OF_HEARTS; // Default start
    alice_state->identity_changed_this_turn = true;
    
    // Store state in character data - FIX: Use character_state instead of skill
    player->fable->character_state = alice_state;
    
    printf("Alice's identity systems initialized - ready for transformation!\n");
}

void alice_switch_identity(Player* player, AliceIdentity new_identity) {
    AliceState* alice_state = get_alice_state(player);
    if (!alice_state) return;
    
    // Validate identity switch rules
    if (new_identity == alice_state->current_identity) {
        printf("Alice cannot stay in the same identity!\n");
        return;
    }
    
    if (alice_state->identity_cooldowns[new_identity] > 0) {
        printf("Identity %s is on cooldown!\n", identity_data[new_identity].name);
        return;
    }
    
    // Apply identity switch
    alice_state->previous_identity = alice_state->current_identity;
    alice_state->current_identity = new_identity;
    alice_state->must_switch_identity = false;
    
    // Set cooldown for previous identity
    if (alice_state->previous_identity < 3) {
        alice_state->identity_cooldowns[alice_state->previous_identity] = 2; // 2 turn cooldown
    }
    
    printf("Alice transforms into %s!\n", identity_data[new_identity].name);
}

int alice_get_card_modifier(Player* player, Card* card, const char* stat_type) {
    AliceState* alice_state = get_alice_state(player);
    if (!alice_state || !card) return 0;
    
    const IdentityModifier* identity = &identity_data[alice_state->current_identity];
    
    // Apply identity modifiers based on card type
    if (strcmp(stat_type, "attack") == 0 && (card->type == BASIC_ATK || card->type == SKILL_ATK)) {
        return identity->attack_modifier;
    }
    if (strcmp(stat_type, "defense") == 0 && (card->type == BASIC_DEF || card->type == SKILL_DEF)) {
        return identity->defense_modifier;
    }
    if (strcmp(stat_type, "movement") == 0 && (card->type == BASIC_MOV || card->type == SKILL_MOV)) {
        return identity->movement_modifier;
    }
    
    return 0;
}

//=============================================================================
// ALICE'S TURN MANAGEMENT
// Implements the mandatory identity switching rule
//=============================================================================

void alice_turn_start(Player* player) {
    AliceState* alice_state = get_alice_state(player);
    if (!alice_state) return;
    
    // Decrease cooldowns
    for (int i = 0; i < 3; i++) {
        if (alice_state->identity_cooldowns[i] > 0) {
            alice_state->identity_cooldowns[i]--;
        }
    }
    
    // Reset turn counters
    alice_state->cards_gained_this_turn = 0;
    alice_state->basic_cards_removed_this_turn = 0;
    alice_state->must_switch_identity = true;
    
    printf("Alice must switch identity this turn!\n");
}

void alice_turn_end(Player* player) {
    AliceState* alice_state = get_alice_state(player);
    if (!alice_state) return;
    
    // Check if Alice switched identity (mandatory rule)
    if (alice_state->must_switch_identity) {
        printf("Alice failed to switch identity! Penalty applied.\n");
        // Apply penalty - lose 1 health for rule violation
        player->health = MAX(1, player->health - 1);
    }
}

void alice_update(Player* player, float delta_time) {
    // Alice doesn't need continuous updates, but this is where
    // we could add visual effects for identity transformations
    (void)delta_time; // Suppress unused parameter warning
    
    AliceState* alice_state = get_alice_state(player);
    if (!alice_state) return;
    
    // Could add identity transformation animations here
}

//=============================================================================
// ALICE'S CARD EFFECT IMPLEMENTATIONS
// These implement the complex card manipulation mechanics
//=============================================================================

void cut_the_cards_effect(void* self, void* target) {
    Player* alice = (Player*)self;
    Player* opponent = (Player*)target;

    printf("Alice uses Cut the Cards\n");
    if (opponent) {
        opponent->health -= 2; // 基礎傷害
    }
}

void rig_the_cards_effect(void* self, void* target) {
    Player* alice = (Player*)self;
    Player* opponent = (Player*)target;

    printf("Alice uses Rig the Cards\n");
    if (opponent) {
        opponent->health -= 3; // 基礎傷害
    }
}

void mind_trick_effect(void* self, void* target) {
    Player* alice = (Player*)self;

    printf("Alice uses Mind Trick\n");
    if (alice) {
        alice->defense = MIN(alice->defense + 2, alice->fable->defense);
    }
}

void curiously_covert_effect(void* self, void* target) {
    Player* alice = (Player*)self;

    printf("Alice uses Curiously Covert\n");
    if (alice) {
        // 移動效果的基礎實作
        alice->pos = MIN(alice->pos + 2, 4);
    }
}

void curiously_curious_effect(void* self, void* target) {
    Player* alice = (Player*)self;

    printf("Alice uses Curiously Curious\n");
    if (alice) {
        // 移動效果的基礎實作
        alice->pos = MIN(alice->pos + 3, 4);
    }
}

void deal_the_cards_effect(void* self, void* target) {
    Player* alice = (Player*)self;
    Player* opponent = (Player*)target;
    
    // "Opponent shows 3 basic cards, for each they can't/won't show, you gain +1 damage"
    // "You may choose one shown card to gain a copy from basic supply"
    
    int cards_shown = 0;
    int basic_cards_in_hand = 0;
    
    // Count basic cards in opponent's hand
    for (int i = 0; i < opponent->hand.cnt; i++) {
        Card* card = opponent->hand.cards[i];
        if (card && card->type >= BASIC_ATK && card->type <= BASIC_MOV) {
            basic_cards_in_hand++;
        }
    }
    
    cards_shown = MIN(3, basic_cards_in_hand);
    int bonus_damage = 3 - cards_shown;
    
    // Apply damage (this would need target selection in full implementation)
    if (bonus_damage > 0) {
        printf("Alice gains +%d damage from hidden cards!\n", bonus_damage);
        // In full implementation, would modify the attack being made
    }
    
    // Alice can switch to Red Queen identity after this effect
    printf("Alice may transform into Red Queen!\n");
}

void magic_trick_effect(void* self, void* target) {
    Player* alice = (Player*)self;
    AliceState* alice_state = get_alice_state(alice);
    
    // "Remove 1 basic card from hand/discard, gain 1 basic card of level ≤ O"
    // "May transform into Mad Hatter"
    
    // Find basic card to remove
    for (int i = 0; i < alice->hand.cnt; i++) {
        Card* card = alice->hand.cards[i];
        if (card && card->type >= BASIC_ATK && card->type <= BASIC_MOV) {
            // Remove card (simplified - would need player choice)
            for (int j = i; j < alice->hand.cnt - 1; j++) {
                alice->hand.cards[j] = alice->hand.cards[j + 1];
            }
            alice->hand.cnt--;
            
            // Add new basic card (would come from supply in full implementation)
            alice_state->cards_gained_this_turn++;
            printf("Alice removed a basic card and gained a new one!\n");
            break;
        }
    }
    
    printf("Alice may transform into Mad Hatter!\n");
}

void curiously_agile_effect(void* self, void* target) {
    Player* alice = (Player*)self;
    Player* opponent = (Player*)target;
    
    // "Move up to 1+O spaces in one direction"
    // "If you pass through opponent, draw 1 card"
    // "May transform into Cheshire Cat"
    
    int movement_range = 1 + alice_get_card_modifier(alice, &curiously_agile, "movement");
    
    // Check if movement would pass through opponent
    bool passes_through = false; // Would need position calculation in full implementation
    
    if (passes_through) {
        draw_hand(alice, 1);
        printf("Alice passes through opponent and draws a card!\n");
    }
    
    printf("Alice may transform into Cheshire Cat!\n");
}

void important_date_effect(void* self, void* target) {
    Player* alice = (Player*)self;
    
    // "After drawing cards at end of turn, discard down to 4 cards and immediately start a new turn"
    // "Cannot use more than 3 times"
    
    // Discard excess cards
    while (alice->hand.cnt > 4) {
        // Remove last card (would need player choice in full implementation)
        Card* discarded = alice->hand.cards[alice->hand.cnt - 1];
        alice->hand.cnt--;
        add_deck(&alice->disc, discarded);
    }
    
    printf("Alice starts an endless party! Beginning new turn...\n");
    // In full implementation, would trigger additional turn
}

void frabjous_day_effect(void* self, void* target) {
    Player* alice = (Player*)self;
    
    // "Until end of turn, attack/defense/movement cards get +1 to their values"
    // This would need to be implemented as a temporary effect in the effect system
    
    printf("Alice declares a wonderful day! All cards get +1 to their values!\n");
    // Would create a temporary effect that modifies card values
}

void the_games_afoot_effect(void* self, void* target) {
    Player* alice = (Player*)self;
    Player* opponent = (Player*)target;
    
    // "Range 3, damage 3"
    // "Shuffle up to 5 basic cards from your hand/discard into opponent's deck"
    
    int cards_shuffled = 0;
    
    // Find basic cards to shuffle into opponent's deck
    for (int i = alice->hand.cnt - 1; i >= 0 && cards_shuffled < 5; i--) {
        Card* card = alice->hand.cards[i];
        if (card && card->type >= BASIC_ATK && card->type <= BASIC_MOV) {
            // Remove from Alice's hand
            for (int j = i; j < alice->hand.cnt - 1; j++) {
                alice->hand.cards[j] = alice->hand.cards[j + 1];
            }
            alice->hand.cnt--;
            
            // Add to opponent's deck
            add_deck(&opponent->draw, card);
            cards_shuffled++;
        }
    }
    
    if (cards_shuffled > 0) {
        shuffle_deck(&opponent->draw);
        printf("Alice shuffles %d basic cards into opponent's deck!\n", cards_shuffled);
    }
    
    // Apply damage
    opponent->health -= 3;
    printf("Alice deals 3 damage with Game Control!\n");
}

//=============================================================================
// ALICE CHARACTER INITIALIZATION
// Sets up Alice's complete character data and deck composition
//=============================================================================

void setup_alice_player(Player* player) {
    if (!player) return;
    
    // Initialize base character
    player->fable = &alice_fable;
    player->health = alice_fable.health;
    player->power = 0;
    player->defense = 0;
    
    // Initialize Alice-specific state
    init_alice_state(player);
    
    // Set up starting deck with basic cards and one skill from each category
    for (int i = 0; i < 3; i++) {
        add_deck(&player->draw, &Attack1);
        add_deck(&player->draw, &Defense1);
        add_deck(&player->draw, &Move1);
    }
    
    // Add one level 1 skill from each category
    add_deck(&player->draw, &deal_the_cards);
    add_deck(&player->draw, &magic_trick);
    add_deck(&player->draw, &curiously_agile);
    
    // Shuffle and draw initial hand
    shuffle_deck(&player->draw);
    draw_hand(player, HAND_SIZE);
    
    printf("Alice enters Wonderland! She must switch identities each turn.\n");
}

// Utility function to check if Alice can perform certain actions
bool alice_can_switch_identity(Player* player, AliceIdentity target_identity) {
    AliceState* alice_state = get_alice_state(player);
    if (!alice_state) return false;
    
    return (target_identity != alice_state->current_identity && 
            alice_state->identity_cooldowns[target_identity] == 0);
}
