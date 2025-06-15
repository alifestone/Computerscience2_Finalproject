#ifndef MATCH_GIRL_H
#define MATCH_GIRL_H

#include "base.h"

// Enhanced Energy System
typedef struct {
    int current_energy;              // Current energy available
    int max_energy;                  // Maximum energy that can be stored
    int energy_per_fire_card;        // Energy gained when opponent uses fire cards
    int energy_per_turn;             // Passive energy generation
    bool flame_enhancement_active;   // From twist cards
    int energy_spent_this_turn;      // Track spending for balance
} EnergySystem;

// Fire Card Management System
typedef struct {
    int fire_cards_placed_this_turn;    // Track placement limits
    int max_fire_per_turn;              // Balance limitation
    bool can_place_fire_adjacent;       // Special rule activation
    int fire_cards_in_opponent_deck;    // Track for strategic decisions
    int fire_cards_in_opponent_discard; // Track for retrieval abilities
} FireSystem;

// Risk/Reward System
typedef struct {
    int max_life_loss_per_turn;         // Safety limitation
    int life_lost_this_turn;            // Track for limits
    int cards_drawn_from_life_loss;     // Track efficiency
    bool desperate_measures_active;     // Enhanced life trading
} RiskRewardSystem;

// Match Girl's complete enhanced state
typedef struct {
    EnergySystem energy;
    FireSystem fire;
    RiskRewardSystem risk_reward;
    bool ritual_of_pain_active;         // Twist effect
    bool storm_eve_stacks;              // Accumulating twist effect
    int consecutive_energy_turns;       // Momentum tracking
} MatchGirlState;

// Enhanced fire card
typedef struct {
    int level;                          // 1, 2, or 3
    int damage_on_use;                  // Damage when opponent uses it
    int energy_given_to_match_girl;     // Energy Match Girl gets
    bool can_be_used_as_attack;         // Rule restrictions
    bool can_be_used_as_defense;        // Rule restrictions
    bool can_be_used_as_movement;       // Rule restrictions
    char restriction_reason[64];        // Why it's restricted
} EnhancedFireCard;

// Match Girl role data
extern Fable match_girl_enhanced_fable;

// Initialization functions
void init_enhanced_match_girl_state(Player *player);
void setup_enhanced_match_girl_player(Player *player);
void init_enhanced_fire_supply(void);

// Match Girl mechanics
void match_girl_enhanced_turn_start(Player *player);
void match_girl_enhanced_turn_end(Player *player);
void match_girl_enhanced_update(Player *player, float delta_time);
bool place_enhanced_fire_card(Player *match_girl, Player *opponent, int fire_level);
void on_opponent_uses_enhanced_fire_card(Player *match_girl, Card *fire_card);
bool match_girl_can_place_fire(Player *player);
int match_girl_get_current_energy(Player *player);
bool match_girl_can_enhance_with_energy(Player *player, int energy_cost);
void check_match_girl_energy_gain(Card *card_used, Player *card_user);

// Card effect function declarations
void make_a_wish_effect(void *self, void *target);
void regret_a_wish_effect(void *self, void *target);
void requite_a_wish_effect(void *self, void *target);
void diabolical_offer_effect(void *self, void *target);
void diabolical_gamble_effect(void *self, void *target);
void diabolical_deal_effect(void *self, void *target);
void your_soul_weighed_effect(void *self, void *target);
void your_soul_wanting_effect(void *self, void *target);
void your_soul_ravaged_effect(void *self, void *target);
void inferno_overload_effect(void *self, void *target);
void the_demons_due_effect(void *self, void *target);
void a_surfeit_of_wishes_effect(void *self, void *target);

// Attack cards
extern Card make_a_wish;    // Phantom Wish
extern Card regret_a_wish;   // Secret Desire
extern Card requite_a_wish;  // Endless Luxury

// Defense cards
extern Card diabolical_offer; // Devil's Sacrifice
extern Card diabolical_gamble;       // Devil's Bet
extern Card diabolical_deal;  // Devil's Contract

// Movement cards
extern Card your_soul_weighed; // Weightless Soul
extern Card your_soul_wanting;   // Indebted Soul
extern Card your_soul_ravaged;     // Broken Soul

// Twist cards
extern Card ritual_of_pain;           // Ritual of Pain
extern Card inflamed_craving;              // Flame Trick
extern Card after_all_ive_given;          // Torment of Fate
extern Card playing_with_fire;         // Indulgent Desire

// Epic cards
extern Card inferno_overload;      // Hell Flame
extern Card the_demons_due;        // Doomfall
extern Card a_surfeit_of_wishes;  // Curse of Greed

#endif
