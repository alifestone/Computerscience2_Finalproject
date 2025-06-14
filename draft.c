#include "base.h"
#include "red_hood.h"
#include "cards.h"
#include "alice.h"
#include "snow_white.h"
#include "mulan.h"
#include "kaguya.h"
#include "match-girl.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Enhanced game state management following the architectural guide
typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_CHARACTER_SELECT,
    GAME_STATE_BATTLE_INIT,
    GAME_STATE_TURN_START,
    GAME_STATE_MAIN_PHASE,
    GAME_STATE_CARD_SELECTION,
    GAME_STATE_TARGET_SELECTION,
    GAME_STATE_EFFECT_RESOLUTION,
    GAME_STATE_TURN_END,
    GAME_STATE_BATTLE_END,
    GAME_STATE_PAUSE
} GameState;

typedef enum {
    TURN_PHASE_START,
    TURN_PHASE_MAIN,
    TURN_PHASE_END,
    TURN_PHASE_CLEANUP
} TurnPhase;

// Effect system for handling complex card interactions
typedef struct GameEffect {
    int effect_id;
    int priority;
    int duration;
    int source_player;
    void (*apply)(struct GameContext* ctx, struct GameEffect* effect);
    void (*remove)(struct GameContext* ctx, struct GameEffect* effect);
    bool (*condition)(struct GameContext* ctx);
    void* effect_data;
    struct GameEffect* next;
} GameEffect;

// Enhanced game context that tracks all game state
typedef struct GameContext {
    GameState current_state;
    GameState previous_state;
    TurnPhase current_phase;
    int current_player;
    int turn_number;
    Player* players[2];
    GameEffect* active_effects;
    Card* selected_card;
    Card* linked_card;
    int selected_target;
    bool game_over;
    int winner;
    
    // Epic card system
    bool p1_epic_available;
    bool p2_epic_available;
    int p1_epic_selected;
    int p2_epic_selected;
    
    // UI state
    SDL_Renderer* renderer;
    TTF_Font* font;
    bool show_debug;
} GameContext;

// Global game context
static GameContext* g_ctx = NULL;

// Function declarations for the enhanced system
void init_game_context(SDL_Renderer* ren);
void cleanup_game_context(void);
void change_game_state(GameState new_state);
void update_game_logic(float delta_time);
void handle_input_event(SDL_Event* event);
void render_game_state(void);

// Effect system functions
void add_effect(GameEffect* effect);
void remove_effect(int effect_id);
void process_effects(void);
GameEffect* create_effect(int id, int priority, int duration);

// Enhanced turn management
void start_turn(int player_id);
void end_turn(void);
void advance_turn_phase(void);

// Card playing system with proper validation
typedef enum {
    PLAY_RESULT_SUCCESS,
    PLAY_RESULT_INVALID_CARD,
    PLAY_RESULT_INSUFFICIENT_ENERGY,
    PLAY_RESULT_INVALID_TARGET,
    PLAY_RESULT_REQUIRES_LINK,
    PLAY_RESULT_OUT_OF_RANGE
} PlayResult;

PlayResult attempt_play_card(Player* player, Card* card, int target_pos);
PlayResult attempt_link_cards(Player* player, Card* skill_card, Card* basic_card, int target_pos);

// Character-specific ability system
void activate_character_ability(Player* player, int ability_index);
void check_epic_availability(Player* player);
bool can_use_epic(Player* player);

// Enhanced UI rendering with better organization
void render_battlefield(void);
void render_player_ui(Player* player, bool is_bottom);
void render_cards_in_hand(Player* player, bool is_bottom);
void render_epic_cards(Player* player, bool is_bottom);
void render_turn_indicator(void);
void render_debug_info(void);

// Utility functions for game logic
bool is_valid_target(Card* card, Player* caster, Player* target, int position);
int calculate_distance(int pos1, int pos2);
void apply_damage(Player* target, int damage, Player* source);
void apply_healing(Player* target, int healing);

//=============================================================================
// INITIALIZATION AND CLEANUP
//=============================================================================

void init_game_context(SDL_Renderer* ren) {
    g_ctx = malloc(sizeof(GameContext));
    memset(g_ctx, 0, sizeof(GameContext));
    
    g_ctx->renderer = ren;
    g_ctx->current_state = GAME_STATE_MENU;
    g_ctx->current_phase = TURN_PHASE_START;
    g_ctx->current_player = 0;
    g_ctx->turn_number = 1;
    
    // Initialize TTF for text rendering
    if (TTF_Init() == -1) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        return;
    }
    
    g_ctx->font = TTF_OpenFont("NotoSans-Regular.ttf", 24);
    if (!g_ctx->font) {
        printf("Error opening font: %s\n", TTF_GetError());
    }
}

void cleanup_game_context(void) {
    if (!g_ctx) return;
    
    // Clean up effects
    GameEffect* effect = g_ctx->active_effects;
    while (effect) {
        GameEffect* next = effect->next;
        if (effect->effect_data) free(effect->effect_data);
        free(effect);
        effect = next;
    }
    
    if (g_ctx->font) TTF_CloseFont(g_ctx->font);
    TTF_Quit();
    
    free(g_ctx);
    g_ctx = NULL;
}

//=============================================================================
// STATE MANAGEMENT
//=============================================================================

void change_game_state(GameState new_state) {
    if (!g_ctx) return;
    
    g_ctx->previous_state = g_ctx->current_state;
    g_ctx->current_state = new_state;
    
    // State entry logic
    switch (new_state) {
        case GAME_STATE_BATTLE_INIT:
            // Initialize battle-specific data
            g_ctx->current_player = rand() % 2; // Random starting player
            g_ctx->turn_number = 1;
            break;
            
        case GAME_STATE_TURN_START:
            start_turn(g_ctx->current_player);
            break;
            
        case GAME_STATE_TURN_END:
            end_turn();
            break;
            
        default:
            break;
    }
}

//=============================================================================
// ENHANCED TURN MANAGEMENT
//=============================================================================

void start_turn(int player_id) {
    if (!g_ctx || !g_ctx->players[player_id]) return;
    
    Player* player = g_ctx->players[player_id];
    g_ctx->current_phase = TURN_PHASE_START;
    
    // Reset turn-specific flags
    player->power = 0; // Reset energy/power each turn
    
    // Draw cards (enhanced hand management)
    if (player->hand.cnt < HAND_SIZE) {
        int cards_to_draw = HAND_SIZE - player->hand.cnt;
        draw_hand(player, cards_to_draw);
    }
    
    // Character-specific turn start effects
    if (player->fable) {
        switch (player->fable->epic_threshold) { // Using epic_threshold to identify character
            case 15: // Red Hood
                // Red Hood specific turn start
                break;
            case 16: // Kaguya or Alice
                if (strcmp(player->fable->name, "Kaguya") == 0) {
                    kaguya_turn_start(player);
                } else {
                    alice_turn_start(player);
                }
                break;
            case 17: // Mulan or Snow White
                if (strcmp(player->fable->name, "Mulan") == 0) {
                    mulan_turn_start(player);
                } else {
                    snow_white_turn_start(player);
                }
                break;
            case 18: // Match Girl
                match_girl_turn_start(player);
                break;
        }
    }
    
    // Check if epic cards are available
    check_epic_availability(player);
    
    g_ctx->current_phase = TURN_PHASE_MAIN;
    change_game_state(GAME_STATE_MAIN_PHASE);
}

void end_turn(void) {
    if (!g_ctx) return;
    
    Player* player = g_ctx->players[g_ctx->current_player];
    g_ctx->current_phase = TURN_PHASE_END;
    
    // Character-specific turn end effects
    if (player->fable) {
        switch (player->fable->epic_threshold) {
            case 15: // Red Hood
                break;
            case 16: // Kaguya or Alice  
                if (strcmp(player->fable->name, "Kaguya") == 0) {
                    kaguya_turn_end(player);
                } else {
                    alice_turn_end(player);
                }
                break;
            case 17: // Mulan or Snow White
                if (strcmp(player->fable->name, "Mulan") == 0) {
                    mulan_turn_end(player);
                } else {
                    snow_white_turn_end(player);
                }
                break;
            case 18: // Match Girl
                match_girl_turn_end(player);
                break;
        }
    }
    
    // Process end-of-turn effects
    process_effects();
    
    // Switch to next player
    g_ctx->current_player = 1 - g_ctx->current_player;
    g_ctx->turn_number++;
    
    // Check win conditions
    if (g_ctx->players[0]->health <= 0) {
        g_ctx->winner = 1;
        g_ctx->game_over = true;
        change_game_state(GAME_STATE_BATTLE_END);
        return;
    }
    if (g_ctx->players[1]->health <= 0) {
        g_ctx->winner = 0;
        g_ctx->game_over = true;
        change_game_state(GAME_STATE_BATTLE_END);
        return;
    }
    
    change_game_state(GAME_STATE_TURN_START);
}

//=============================================================================
// ENHANCED CARD PLAYING SYSTEM
//=============================================================================

PlayResult attempt_play_card(Player* player, Card* card, int target_pos) {
    if (!player || !card) return PLAY_RESULT_INVALID_CARD;
    
    // Check energy cost
    if (card->cst > player->power) return PLAY_RESULT_INSUFFICIENT_ENERGY;
    
    // Check if card requires linking
    if (card->link && card->type >= SKILL_ATK && card->type <= SKILL_MOV) {
        return PLAY_RESULT_REQUIRES_LINK;
    }
    
    // Validate target based on card type and range
    Player* target_player = (target_pos < 0) ? player : g_ctx->players[1 - g_ctx->current_player];
    
    if (!is_valid_target(card, player, target_player, target_pos)) {
        return PLAY_RESULT_INVALID_TARGET;
    }
    
    // Consume energy
    player->power -= card->cst;
    
    // Execute card effect based on type
    switch (card->type) {
        case BASIC_ATK:
            if (calculate_distance(player->pos, target_player->pos) <= card->rng) {
                apply_damage(target_player, card->dmg, player);
                player->power += card->val; // Gain power from basic cards
            } else {
                return PLAY_RESULT_OUT_OF_RANGE;
            }
            break;
            
        case BASIC_DEF:
            player->defense = MIN(player->defense + card->defense, player->fable->defense);
            player->power += card->val;
            break;
            
        case BASIC_MOV:
            // Movement will be handled by UI interaction
            player->power += card->val;
            return PLAY_RESULT_SUCCESS; // Special handling for movement
            
        case SKILL_ATK:
        case SKILL_DEF:
        case SKILL_MOV:
            // These should not reach here without linking
            return PLAY_RESULT_REQUIRES_LINK;
            
        case EPIC:
            // Epic cards have special effects
            if (card->effect) {
                card->effect(player, target_player);
            }
            break;
            
        default:
            break;
    }
    
    // Remove card from hand and add to discard
    for (int i = 0; i < player->hand.cnt; i++) {
        if (player->hand.cards[i] == card) {
            // Shift remaining cards
            for (int j = i; j < player->hand.cnt - 1; j++) {
                player->hand.cards[j] = player->hand.cards[j + 1];
            }
            player->hand.cnt--;
            break;
        }
    }
    add_deck(&player->disc, card);
    
    return PLAY_RESULT_SUCCESS;
}

PlayResult attempt_link_cards(Player* player, Card* skill_card, Card* basic_card, int target_pos) {
    if (!player || !skill_card || !basic_card) return PLAY_RESULT_INVALID_CARD;
    
    // Validate link compatibility
    Type required_basic = (Type)(skill_card->type - 4); // Convert SKILL_* to BASIC_*
    if (basic_card->type != required_basic) return PLAY_RESULT_INVALID_CARD;
    
    // Check total energy cost
    int total_cost = skill_card->cst + basic_card->cst;
    if (total_cost > player->power) return PLAY_RESULT_INSUFFICIENT_ENERGY;
    
    // Validate target
    Player* target_player = (target_pos < 0) ? player : g_ctx->players[1 - g_ctx->current_player];
    if (!is_valid_target(skill_card, player, target_player, target_pos)) {
        return PLAY_RESULT_INVALID_TARGET;
    }
    
    // Consume energy
    player->power -= total_cost;
    
    // Execute combined effect
    switch (skill_card->type) {
        case SKILL_ATK:
            if (calculate_distance(player->pos, target_player->pos) <= skill_card->rng) {
                int total_damage = skill_card->dmg + basic_card->dmg;
                apply_damage(target_player, total_damage, player);
                
                // Character-specific skill effects
                if (skill_card->effect) {
                    skill_card->effect(player, target_player);
                }
            } else {
                return PLAY_RESULT_OUT_OF_RANGE;
            }
            break;
            
        case SKILL_DEF:
            {
                int total_defense = skill_card->defense + basic_card->defense;
                player->defense = MIN(player->defense + total_defense, player->fable->defense);
                
                if (skill_card->effect) {
                    skill_card->effect(player, target_player);
                }
            }
            break;
            
        case SKILL_MOV:
            // Movement with skill effects
            if (skill_card->effect) {
                skill_card->effect(player, target_player);
            }
            return PLAY_RESULT_SUCCESS; // Special handling for movement
            
        default:
            break;
    }
    
    // Remove both cards from hand
    for (int i = player->hand.cnt - 1; i >= 0; i--) {
        if (player->hand.cards[i] == skill_card || player->hand.cards[i] == basic_card) {
            for (int j = i; j < player->hand.cnt - 1; j++) {
                player->hand.cards[j] = player->hand.cards[j + 1];
            }
            player->hand.cnt--;
        }
    }
    
    // Add to discard
    add_deck(&player->disc, skill_card);
    add_deck(&player->disc, basic_card);
    
    return PLAY_RESULT_SUCCESS;
}

//=============================================================================
// EFFECT SYSTEM
//=============================================================================

void add_effect(GameEffect* effect) {
    if (!g_ctx || !effect) return;
    
    effect->next = g_ctx->active_effects;
    g_ctx->active_effects = effect;
}

void remove_effect(int effect_id) {
    if (!g_ctx) return;
    
    GameEffect* prev = NULL;
    GameEffect* current = g_ctx->active_effects;
    
    while (current) {
        if (current->effect_id == effect_id) {
            if (current->remove) {
                current->remove(g_ctx, current);
            }
            
            if (prev) {
                prev->next = current->next;
            } else {
                g_ctx->active_effects = current->next;
            }
            
            if (current->effect_data) free(current->effect_data);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

void process_effects(void) {
    if (!g_ctx) return;
    
    GameEffect* effect = g_ctx->active_effects;
    while (effect) {
        GameEffect* next = effect->next;
        
        if (effect->condition && !effect->condition(g_ctx)) {
            remove_effect(effect->effect_id);
        } else {
            if (effect->apply) {
                effect->apply(g_ctx, effect);
            }
            
            effect->duration--;
            if (effect->duration <= 0) {
                remove_effect(effect->effect_id);
            }
        }
        
        effect = next;
    }
}

GameEffect* create_effect(int id, int priority, int duration) {
    GameEffect* effect = malloc(sizeof(GameEffect));
    memset(effect, 0, sizeof(GameEffect));
    
    effect->effect_id = id;
    effect->priority = priority;
    effect->duration = duration;
    
    return effect;
}

//=============================================================================
// CHARACTER ABILITIES
//=============================================================================

void check_epic_availability(Player* player) {
    if (!g_ctx || !player) return;
    
    bool* epic_available = (g_ctx->current_player == 0) ? &g_ctx->p1_epic_available : &g_ctx->p2_epic_available;
    
    *epic_available = (player->health <= player->fable->epic_threshold);
}

bool can_use_epic(Player* player) {
    if (!g_ctx || !player) return false;
    
    bool epic_available = (g_ctx->current_player == 0) ? g_ctx->p1_epic_available : g_ctx->p2_epic_available;
    int epic_selected = (g_ctx->current_player == 0) ? g_ctx->p1_epic_selected : g_ctx->p2_epic_selected;
    
    return epic_available && epic_selected == 0;
}

//=============================================================================
// UTILITY FUNCTIONS
//=============================================================================

bool is_valid_target(Card* card, Player* caster, Player* target, int position) {
    if (!card || !caster || !target) return false;
    
    // Check range
    int distance = calculate_distance(caster->pos, target->pos);
    if (distance > card->rng) return false;
    
    // Additional validation based on card type
    switch (card->type) {
        case BASIC_ATK:
        case SKILL_ATK:
            return target != caster; // Can't attack self
            
        case BASIC_DEF:
        case SKILL_DEF:
            return target == caster; // Can only defend self
            
        default:
            return true;
    }
}

int calculate_distance(int pos1, int pos2) {
    return abs(pos1 - pos2);
}

void apply_damage(Player* target, int damage, Player* source) {
    if (!target || damage <= 0) return;
    
    // Apply defense reduction first
    int actual_damage = MAX(0, damage - target->defense);
    target->defense = MAX(0, target->defense - damage);
    
    // Apply remaining damage to health
    target->health -= actual_damage;
    
    // Character-specific damage reactions
    if (target->fable) {
        switch (target->fable->epic_threshold) {
            case 17: // Mulan
                if (strcmp(target->fable->name, "Mulan") == 0) {
                    mulan_on_damage_taken(target, actual_damage, source);
                }
                break;
            case 16: // Kaguya
                if (strcmp(target->fable->name, "Kaguya") == 0) {
                    kaguya_on_damage_taken(target, actual_damage, source);
                }
                break;
        }
    }
}

//=============================================================================
// MAIN GAME LOOP INTEGRATION
//=============================================================================

void update_game_logic(float delta_time) {
    if (!g_ctx) return;
    
    // Process ongoing effects
    process_effects();
    
    // Update character-specific systems
    for (int i = 0; i < 2; i++) {
        Player* player = g_ctx->players[i];
        if (!player || !player->fable) continue;
        
        switch (player->fable->epic_threshold) {
            case 18: // Match Girl
                match_girl_update(player, delta_time);
                break;
            case 16: // Alice
                if (strcmp(player->fable->name, "Alice") == 0) {
                    alice_update(player, delta_time);
                }
                break;
        }
    }
}

// This function integrates with the existing main game loop
void enhanced_battle_screen(SDL_Renderer* ren, Player* p1, Player* p2) {
    // Initialize enhanced game context
    init_game_context(ren);
    g_ctx->players[0] = p1;
    g_ctx->players[1] = p2;
    
    change_game_state(GAME_STATE_BATTLE_INIT);
    
    bool in_battle = true;
    SDL_Event event;
    Uint32 last_time = SDL_GetTicks();
    
    while (in_battle && !g_ctx->game_over) {
        Uint32 current_time = SDL_GetTicks();
        float delta_time = (current_time - last_time) / 1000.0f;
        last_time = current_time;
        
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                in_battle = false;
                break;
            }
            handle_input_event(&event);
        }
        
        // Update game logic
        update_game_logic(delta_time);
        
        // Render
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderClear(ren);
        
        render_game_state();
        
        SDL_RenderPresent(ren);
        SDL_Delay(16); // ~60 FPS
    }
    
    cleanup_game_context();
}

// Note: The original BattleScreen function should be replaced with enhanced_battle_screen
// This provides much better architecture for handling complex character interactions