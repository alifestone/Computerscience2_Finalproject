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

//=============================================================================
// ENHANCED GAME STATE MANAGEMENT (from draft.c)
//=============================================================================

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

typedef enum {
    PLAY_RESULT_SUCCESS,
    PLAY_RESULT_INVALID_CARD,
    PLAY_RESULT_INSUFFICIENT_ENERGY,
    PLAY_RESULT_INVALID_TARGET,
    PLAY_RESULT_REQUIRES_LINK,
    PLAY_RESULT_OUT_OF_RANGE
} PlayResult;

// Forward declaration
typedef struct GameContext GameContext;

// Effect system for handling complex card interactions
typedef struct GameEffect {
    int effect_id;
    int priority;
    int duration;
    int source_player;
    void (*apply)(GameContext* ctx, struct GameEffect* effect);
    void (*remove)(GameContext* ctx, struct GameEffect* effect);
    bool (*condition)(GameContext* ctx);
    void* effect_data;
    struct GameEffect* next;
} GameEffect;

// Enhanced game context that tracks all game state
struct GameContext {
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
};

// Global game context
static GameContext* g_ctx = NULL;

//=============================================================================
// FUNCTION FORWARD DECLARATIONS
//=============================================================================

// Game state functions
void change_game_state(GameState new_state);
void start_turn(int player_id);
void end_turn(void);

// Effect system functions
void add_effect(GameEffect* effect);
void remove_effect(int effect_id);
void process_effects(void);
GameEffect* create_effect(int id, int priority, int duration);

// Card playing system
PlayResult attempt_play_card(Player* player, Card* card, int target_pos);
bool is_valid_target(Card* card, Player* caster, Player* target, int position);

// Character abilities
void check_epic_availability(Player* player);
void apply_damage(Player* target, int damage, Player* source);

// Character state initialization functions
// These functions are implemented in their respective character .c files
void init_alice_state(Player* player);
void init_kaguya_state(Player* player);
void init_mulan_state(Player* player);
void init_snow_white_state(Player* player);
void init_enhanced_match_girl_state(Player* player);
void init_red_hood_state(Player* player);

// Enhanced setup functions
void enhanced_starting_deck(Player *p);
void enhanced_setup_player(Player *p, Fable *fable);
void enhanced_battle_setup(Player *p1, Player *p2, int starting);

//=============================================================================
// UI SYSTEM (from UI and main)
//=============================================================================

typedef struct {
    const char *txt;
    int32_t x,y;
    int32_t a;
    bool activate;
} Floating;

TTF_Font *font = NULL;
bool Playing = 0;

void WriteText(SDL_Renderer *ren, const char *text, SDL_Color color, int32_t x, int32_t y, int font_size) {
    TTF_Init();
    if (!font) {
        font = TTF_OpenFont("NotoSans-Regular.ttf", font_size);
        if (!font) {
            printf("Error opening font: %s\n", TTF_GetError());
            return;
        }
    }

    char *text_copy = strdup(text);
    if (!text_copy) return;

    const int line_spacing = font_size + 10;
    int line_count = 0;

    char *line = strtok(text_copy, "\n");
    while (line) {
        SDL_Surface *sur = TTF_RenderText_Solid(font, line, color);
        if (!sur) {
            printf("Error creating surface: %s\n", SDL_GetError());
            free(text_copy);
            return;
        }

        SDL_Texture *txture = SDL_CreateTextureFromSurface(ren, sur);
        if (!txture) {
            printf("Error creating texture: %s\n", SDL_GetError());
            SDL_FreeSurface(sur);
            free(text_copy);
            return;
        }

        int32_t wtext = sur->w, htext = sur->h;
        SDL_FreeSurface(sur);

        SDL_Rect dest = { x - wtext / 2, y - htext / 2 + line_count * line_spacing, wtext, htext };
        SDL_RenderCopy(ren, txture, NULL, &dest);
        SDL_DestroyTexture(txture);

        line = strtok(NULL, "\n");
        line_count++;
    }

    free(text_copy);
}

void DrawRect(SDL_Renderer *ren,SDL_Rect rect,int32_t r,SDL_Color color){
    SDL_Rect cen={rect.x-rect.w/2,rect.y-rect.h/2,rect.w,rect.h};
    roundedBoxRGBA(ren,cen.x,cen.y,cen.x+cen.w,cen.y+cen.h,r,color.r,color.g,color.b,color.a);
}

void DrawButton(SDL_Renderer *ren,SDL_Rect rect,int32_t r,SDL_Color color,SDL_Color txtclr,const char *line){
    DrawRect(ren,rect,r,color);
    WriteText(ren,line,txtclr,rect.x,rect.y,30);
}

void BattleLane(SDL_Renderer *ren,Player *p1, Player *p2){
    for(int i=-4;i<=4;i++){
        SDL_Rect rect={XCENTER+i*(WField+20),YCENTER,WField,HField};
        SDL_Color relic={0,0,0,255};
        DrawRect(ren,rect,10,relic);
    }
    filledCircleRGBA(ren,XCENTER+p1->pos*(WField+20),YCENTER,40,p1->fable->Piece.r,p1->fable->Piece.g,p1->fable->Piece.b,p1->fable->Piece.a);
    filledCircleRGBA(ren,XCENTER+p2->pos*(WField+20),YCENTER,40,p2->fable->Piece.r,p2->fable->Piece.g,p2->fable->Piece.b,p2->fable->Piece.a);
}

//=============================================================================
// CHARACTER STATE INITIALIZATION IMPLEMENTATIONS
//=============================================================================

// Note: All character initialization functions are now provided by their respective .c files
// Removed duplicate implementations to avoid linking conflicts

void init_game_context(SDL_Renderer* ren) {
    g_ctx = malloc(sizeof(GameContext));
    memset(g_ctx, 0, sizeof(GameContext));
    
    g_ctx->renderer = ren;
    g_ctx->current_state = GAME_STATE_MENU;
    g_ctx->current_phase = TURN_PHASE_START;
    g_ctx->current_player = 0;
    g_ctx->turn_number = 1;
    
    if (TTF_Init() == -1) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        return;
    }
    
    g_ctx->font = TTF_OpenFont("NotoSans-Regular.ttf", 24);
    if (!g_ctx->font) {
        printf("Error opening font: %s\n", TTF_GetError());
    }
    
    printf("Enhanced game context initialized\n");
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
    
    printf("Game context cleaned up\n");
}

//=============================================================================
// ENHANCED STATE MANAGEMENT
//=============================================================================

void change_game_state(GameState new_state) {
    if (!g_ctx) return;
    
    g_ctx->previous_state = g_ctx->current_state;
    g_ctx->current_state = new_state;
    
    printf("Game state changed: %d -> %d\n", g_ctx->previous_state, new_state);
    
    switch (new_state) {
        case GAME_STATE_BATTLE_INIT:
            g_ctx->current_player = rand() % 2;
            g_ctx->turn_number = 1;
            printf("Battle initialized, starting player: %d\n", g_ctx->current_player);
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
    
    printf("=== Player %d Turn %d Start ===\n", player_id + 1, g_ctx->turn_number);
    
    // Reset turn-specific flags
    player->power = 0;
    
    // Draw cards if needed
    if (player->hand.cnt < HAND_SIZE) {
        int cards_to_draw = HAND_SIZE - player->hand.cnt;
        draw_hand(player, cards_to_draw);
        printf("Player %d draws %d cards (hand size: %d)\n", player_id + 1, cards_to_draw, player->hand.cnt);
    }
    
    // Character-specific turn start effects (with safety checks)
    if (player->fable) {
        if (strcmp(player->fable->name, "Red Hood") == 0) {
            // red_hood_turn_start(player); // Commented out until proper implementation
            printf("Red Hood turn start\n");
        } else if (strcmp(player->fable->name, "Alice") == 0) {
            // alice_turn_start(player); // Commented out until proper implementation
            printf("Alice turn start\n");
        } else if (strcmp(player->fable->name, "Kaguya") == 0) {
            // kaguya_turn_start(player); // Commented out until proper implementation
            printf("Kaguya turn start\n");
        } else if (strcmp(player->fable->name, "Mulan") == 0) {
            // mulan_turn_start(player); // Commented out until proper implementation
            printf("Mulan turn start\n");
        } else if (strcmp(player->fable->name, "Snow White") == 0) {
            // snow_white_turn_start(player); // Commented out until proper implementation
            printf("Snow White turn start\n");
        } else if (strcmp(player->fable->name, "Match Girl") == 0) {
            // match_girl_turn_start(player); // Commented out until proper implementation
            printf("Match Girl turn start\n");
        }
    }
    
    // Check epic availability
    check_epic_availability(player);
    
    g_ctx->current_phase = TURN_PHASE_MAIN;
    change_game_state(GAME_STATE_MAIN_PHASE);
}

void end_turn(void) {
    if (!g_ctx) return;
    
    Player* player = g_ctx->players[g_ctx->current_player];
    g_ctx->current_phase = TURN_PHASE_END;
    
    printf("=== Player %d Turn End ===\n", g_ctx->current_player + 1);
    
    // Character-specific turn end effects (with safety checks)
    if (player->fable) {
        if (strcmp(player->fable->name, "Red Hood") == 0) {
            // red_hood_turn_end(player); // Commented out until proper implementation
            printf("Red Hood turn end\n");
        } else if (strcmp(player->fable->name, "Alice") == 0) {
            // alice_turn_end(player); // Commented out until proper implementation
            printf("Alice turn end\n");
        } else if (strcmp(player->fable->name, "Kaguya") == 0) {
            // kaguya_turn_end(player); // Commented out until proper implementation
            printf("Kaguya turn end\n");
        } else if (strcmp(player->fable->name, "Mulan") == 0) {
            // mulan_turn_end(player); // Commented out until proper implementation
            printf("Mulan turn end\n");
        } else if (strcmp(player->fable->name, "Snow White") == 0) {
            // snow_white_turn_end(player); // Commented out until proper implementation
            printf("Snow White turn end\n");
        } else if (strcmp(player->fable->name, "Match Girl") == 0) {
            // match_girl_turn_end(player); // Commented out until proper implementation
            printf("Match Girl turn end\n");
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
        printf("Player 2 wins!\n");
        return;
    }
    if (g_ctx->players[1]->health <= 0) {
        g_ctx->winner = 0;
        g_ctx->game_over = true;
        change_game_state(GAME_STATE_BATTLE_END);
        printf("Player 1 wins!\n");
        return;
    }
    
    change_game_state(GAME_STATE_TURN_START);
}

//=============================================================================
// ENHANCED CARD PLAYING SYSTEM
//=============================================================================

bool is_valid_target(Card* card, Player* caster, Player* target, int position) {
    if (!card || !caster || !target) return false;
    
    int distance = abs(caster->pos - target->pos);
    if (distance > card->rng) return false;
    
    switch (card->type) {
        case BASIC_ATK:
        case SKILL_ATK:
            return target != caster;
            
        case BASIC_DEF:
        case SKILL_DEF:
            return target == caster;
            
        default:
            return true;
    }
}

PlayResult attempt_play_card(Player* player, Card* card, int target_pos) {
    if (!player || !card) return PLAY_RESULT_INVALID_CARD;
    
    if (card->cst > player->power) return PLAY_RESULT_INSUFFICIENT_ENERGY;
    
    if (card->link && card->type >= SKILL_ATK && card->type <= SKILL_MOV) {
        return PLAY_RESULT_REQUIRES_LINK;
    }
    
    Player* target_player = (target_pos < 0) ? player : g_ctx->players[1 - g_ctx->current_player];
    
    if (!is_valid_target(card, player, target_player, target_pos)) {
        return PLAY_RESULT_INVALID_TARGET;
    }
    
    // Consume energy
    player->power -= card->cst;
    
    // Execute card effect
    switch (card->type) {
        case BASIC_ATK:
            if (abs(player->pos - target_player->pos) <= card->rng) {
                apply_damage(target_player, card->dmg, player);
                player->power += card->val;
            } else {
                return PLAY_RESULT_OUT_OF_RANGE;
            }
            break;
            
        case BASIC_DEF:
            player->defense = MIN(player->defense + card->defense, player->fable->defense);
            player->power += card->val;
            break;
            
        case BASIC_MOV:
            player->power += card->val;
            return PLAY_RESULT_SUCCESS;
            
        case SKILL_ATK:
        case SKILL_DEF:
        case SKILL_MOV:
            return PLAY_RESULT_REQUIRES_LINK;
            
        case EPIC:
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
            for (int j = i; j < player->hand.cnt - 1; j++) {
                player->hand.cards[j] = player->hand.cards[j + 1];
            }
            player->hand.cnt--;
            break;
        }
    }
    add_deck(&player->disc, card);
    
    printf("Card %s played successfully\n", card->name);
    return PLAY_RESULT_SUCCESS;
}

//=============================================================================
// EFFECT SYSTEM
//=============================================================================

void add_effect(GameEffect* effect) {
    if (!g_ctx || !effect) return;
    
    effect->next = g_ctx->active_effects;
    g_ctx->active_effects = effect;
    printf("Effect %d added\n", effect->effect_id);
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
            printf("Effect %d removed\n", effect_id);
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
    
    if (*epic_available) {
        printf("Player %d epic cards now available!\n", g_ctx->current_player + 1);
    }
}

void apply_damage(Player* target, int damage, Player* source) {
    if (!target || damage <= 0) return;
    
    int actual_damage = MAX(0, damage - target->defense);
    target->defense = MAX(0, target->defense - damage);
    target->health -= actual_damage;
    
    printf("Applied %d damage to %s (remaining health: %d)\n", 
           actual_damage, target->fable->name, target->health);
    
    // Character-specific damage reactions (with safety checks)
    if (target->fable) {
        if (strcmp(target->fable->name, "Mulan") == 0) {
            // mulan_on_damage_taken(target, actual_damage, source); // Commented out until proper implementation
            printf("Mulan damage reaction\n");
        } else if (strcmp(target->fable->name, "Kaguya") == 0) {
            // kaguya_on_damage_taken(target, actual_damage, source); // Commented out until proper implementation
            printf("Kaguya damage reaction\n");
        }
    }
}

//=============================================================================
// ENHANCED TITLE SCREEN
//=============================================================================

void TitleScreen(SDL_Renderer *ren){
    bool intitle=1;
    SDL_Event e;
    SDL_Rect button={WSCREEN/2-200,HSCREEN/2-50,400,100};
    SDL_Color txtclr={255,255,255,255},btnclr={194,194,194,255};
    int32_t r=10;
    
    while(intitle){
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT){
                exit(0);
                Playing=0;
            }
            if(e.type==SDL_MOUSEBUTTONDOWN){
                int32_t mx=e.button.x, my=e.button.y;
                if(mx>=XCENTER-200&&mx<=XCENTER+200&&my>=YCENTER+60&&my<=YCENTER+160){
                    intitle=0;
                }
            }
        }
        SDL_SetRenderDrawColor(ren,255,255,255,255);
        SDL_RenderClear(ren);
        SDL_Rect box={XCENTER,YCENTER+110,400,100};
        DrawButton(ren,box,r,btnclr,txtclr,"Play Enhanced 1v1");

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }
}

//=============================================================================
// ENHANCED BATTLE SCREEN WITH IMPROVED STATE MANAGEMENT
//=============================================================================

void EnhancedBattleScreen(SDL_Renderer* ren, Player* p1, Player* p2) {
    // Initialize enhanced game context
    init_game_context(ren);
    g_ctx->players[0] = p1;
    g_ctx->players[1] = p2;
    
    change_game_state(GAME_STATE_BATTLE_INIT);
    
    bool in_battle = true;
    SDL_Event event;
    Uint32 last_time = SDL_GetTicks();
    
    printf("Enhanced battle system started\n");
    
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
            
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int32_t mx = event.button.x, my = event.button.y;
                
                // End turn button
                SDL_Rect end_turn_rect = {WSCREEN-250, HSCREEN/2-50, 200, 100};
                if (SDL_PointInRect(&(SDL_Point){mx, my}, &end_turn_rect)) {
                    if (g_ctx->current_state == GAME_STATE_MAIN_PHASE) {
                        change_game_state(GAME_STATE_TURN_END);
                    }
                    continue;
                }
                
                // Card selection (simplified)
                if (g_ctx->current_state == GAME_STATE_MAIN_PHASE) {
                    Player* current_player = g_ctx->players[g_ctx->current_player];
                    
                    // Check hand cards for current player
                    for (int i = 0; i < current_player->hand.cnt; i++) {
                        SDL_Rect card_rect;
                        if (g_ctx->current_player == 0) {
                            card_rect = (SDL_Rect){50 + i * (WCARD + 20), HSCREEN - HCARD + 40, WCARD, HCARD};
                        } else {
                            card_rect = (SDL_Rect){WSCREEN - (50 + (i + 1) * (WCARD + 20)), -40, WCARD, HCARD};
                        }
                        
                        if (SDL_PointInRect(&(SDL_Point){mx, my}, &card_rect)) {
                            Card* selected = current_player->hand.cards[i];
                            Player* opponent = g_ctx->players[1 - g_ctx->current_player];
                            
                            PlayResult result = attempt_play_card(current_player, selected, opponent->pos);
                            
                            switch (result) {
                                case PLAY_RESULT_SUCCESS:
                                    printf("Card played successfully\n");
                                    break;
                                case PLAY_RESULT_INSUFFICIENT_ENERGY:
                                    printf("Insufficient energy\n");
                                    break;
                                case PLAY_RESULT_OUT_OF_RANGE:
                                    printf("Target out of range\n");
                                    break;
                                case PLAY_RESULT_REQUIRES_LINK:
                                    printf("Card requires linking\n");
                                    break;
                                default:
                                    printf("Cannot play card\n");
                                    break;
                            }
                            break;
                        }
                    }
                }
            }
        }
        
        // Update character-specific systems (with safety checks)
        for (int i = 0; i < 2; i++) {
            Player* player = g_ctx->players[i];
            if (!player || !player->fable) continue;
            
            if (strcmp(player->fable->name, "Match Girl") == 0) {
                // match_girl_update(player, delta_time); // Commented out until proper implementation
            } else if (strcmp(player->fable->name, "Alice") == 0) {
                // alice_update(player, delta_time); // Commented out until proper implementation
            }
        }
        
        // Process ongoing effects
        process_effects();
        
        // Render
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderClear(ren);
        
        // Render battlefield
        BattleLane(ren, p1, p2);
        
        // Render P1 hand (bottom)
        for (int i = 0; i < p1->hand.cnt; i++) {
            SDL_Rect rect = {50 + i * (WCARD + 20), HSCREEN - HCARD + 40, WCARD, HCARD};
            SDL_SetRenderDrawColor(ren, 200, 0, 0, 255);
            SDL_RenderFillRect(ren, &rect);
            WriteText(ren, p1->hand.cards[i]->name, (SDL_Color){255, 255, 255, 255}, 
                     rect.x + WCARD/2, rect.y + HCARD/2, 16);
        }
        
        // Render P2 hand (top)
        for (int i = 0; i < p2->hand.cnt; i++) {
            SDL_Rect rect = {WSCREEN - (50 + (i + 1) * (WCARD + 20)), -40, WCARD, HCARD};
            SDL_SetRenderDrawColor(ren, 0, 0, 200, 255);
            SDL_RenderFillRect(ren, &rect);
            WriteText(ren, p2->hand.cards[i]->name, (SDL_Color){255, 255, 255, 255}, 
                     rect.x + WCARD/2, rect.y + HCARD/2, 16);
        }
        
        // Render health bars
        SDL_Rect p1_health = {WSCREEN-350, HSCREEN-100, p1->health * 300 / p1->fable->health, 30};
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        SDL_RenderFillRect(ren, &p1_health);
        
        SDL_Rect p2_health = {50, 200, p2->health * 300 / p2->fable->health, 30};
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        SDL_RenderFillRect(ren, &p2_health);
        
        // Render turn indicator
        if (g_ctx->current_player == 0) {
            WriteText(ren, "P1's Turn", (SDL_Color){0, 0, 0, 255}, 100, HSCREEN/2, 30);
            SDL_Rect end_turn = {WSCREEN-150, HSCREEN/2, 200, 100};
            DrawButton(ren, end_turn, 10, (SDL_Color){200, 0, 0, 255}, 
                      (SDL_Color){255, 255, 255, 255}, "End Turn");
        } else {
            WriteText(ren, "P2's Turn", (SDL_Color){0, 0, 0, 255}, 100, HSCREEN/2, 30);
            SDL_Rect end_turn = {WSCREEN-150, HSCREEN/2, 200, 100};
            DrawButton(ren, end_turn, 10, (SDL_Color){0, 0, 200, 255}, 
                      (SDL_Color){255, 255, 255, 255}, "End Turn");
        }
        
        // Render game state info
        char state_text[64];
        snprintf(state_text, sizeof(state_text), "Turn: %d | State: %d", 
                g_ctx->turn_number, g_ctx->current_state);
        WriteText(ren, state_text, (SDL_Color){0, 0, 0, 255}, WSCREEN/2, 50, 20);
        
        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }
    
    // Game over screen
    if (g_ctx->game_over) {
        char winner_text[64];
        snprintf(winner_text, sizeof(winner_text), "Player %d Wins!", g_ctx->winner + 1);
        WriteText(ren, winner_text, (SDL_Color){255, 0, 0, 255}, WSCREEN/2, HSCREEN/2, 48);
        SDL_RenderPresent(ren);
        SDL_Delay(3000);
    }
    
    cleanup_game_context();
}

//=============================================================================
// ENHANCED SETUP SYSTEM
//=============================================================================

Deck general_shop = {
    .cards = {&Attack1, &Defense1, &Move1, &Wild1, &Attack2, &Defense2, &Move2, &Attack3, &Defense3, &Move3},
    .cnt = 10
};

void enhanced_starting_deck(Player *p) {
    for (int i = 0; i < 3; i++) {
        add_deck(&p->draw, &Attack1);
        add_deck(&p->draw, &Defense1);
        add_deck(&p->draw, &Move1);
    }
    
    // Add character-specific starting skills if available
    if (p->fable) {
        // Add basic skill cards (placeholder since actual skills need proper implementation)
        add_deck(&p->draw, &Attack1); // Placeholder for level 1 attack skill
        add_deck(&p->draw, &Defense1); // Placeholder for level 1 defense skill
        add_deck(&p->draw, &Move1);   // Placeholder for level 1 movement skill
    }
    
    shuffle_deck(&p->draw);
}

void enhanced_setup_player(Player *p, Fable *fable) {
    p->fable = fable;
    p->health = fable->health;
    p->power = 0;
    p->defense = 0;
    p->pos = fable->lane;
    
    // Initialize character-specific state
    if (strcmp(fable->name, "Alice") == 0) {
        init_alice_state(p);
    } else if (strcmp(fable->name, "Kaguya") == 0) {
        init_kaguya_state(p);
    } else if (strcmp(fable->name, "Mulan") == 0) {
        init_mulan_state(p);
    } else if (strcmp(fable->name, "Snow White") == 0) {
        init_snow_white_state(p);
    } else if (strcmp(fable->name, "Match Girl") == 0) {
        init_enhanced_match_girl_state(p);
    } else if (strcmp(fable->name, "Red Hood") == 0) {
        init_red_hood_state(p);
    }
}

void enhanced_battle_setup(Player *p1, Player *p2, int starting) {
    // Create character instances (these should be properly defined in character files)
    static Fable red_hood_char = {
        .name = "Red Hood",
        .Piece = {255, 0, 0, 255},
        .health = 30,
        .energy = 25,
        .defense = 6,
        .epic_threshold = 15,
        .lane = 0
    };
    
    static Fable alice_char = {
        .name = "Alice",
        .Piece = {0, 150, 255, 255},
        .health = 32,
        .energy = 0,
        .defense = 6,
        .epic_threshold = 16,
        .lane = 0
    };
    
    // Initialize characters with enhanced setup
    enhanced_setup_player(p1, &red_hood_char);
    p1->pos = -1;
    enhanced_starting_deck(p1);
    
    enhanced_setup_player(p2, &alice_char);
    p2->pos = 1;
    enhanced_starting_deck(p2);
    
    // Initial hand sizes based on starting player
    if (starting == 1) {
        draw_hand(p1, 4);
        draw_hand(p2, 6);
    } else {
        draw_hand(p1, 6);
        draw_hand(p2, 4);
    }
    
    printf("Enhanced battle setup complete\n");
    printf("P1: %s (Health: %d)\n", p1->fable->name, p1->health);
    printf("P2: %s (Health: %d)\n", p2->fable->name, p2->health);
}

//=============================================================================
// ENHANCED MAIN FUNCTION
//=============================================================================

int main() {
    srand(time(NULL));
    Playing = 1;
    Player p1 = {0}, p2 = {0};

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("Twisted Fables - Enhanced Edition",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WSCREEN, HSCREEN, 0);
    if (!win) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!ren) {
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    printf("Enhanced Twisted Fables starting...\n");

    // Main game loop with enhanced systems
    while (Playing) {
        TitleScreen(ren);
        
        // Determine starting player
        int starting_player = (rand() % 2) + 1;
        enhanced_battle_setup(&p1, &p2, starting_player);
        
        EnhancedBattleScreen(ren, &p1, &p2);
    }

    // Cleanup
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    
    printf("Enhanced game shutdown complete\n");
    return 0;
}
