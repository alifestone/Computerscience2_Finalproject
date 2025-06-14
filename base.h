#ifndef BASE_H_INCLUDED
#define BASE_H_INCLUDED
#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<math.h>
#include<stdint.h>
#include<stdbool.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include<SDL2/SDL2_gfxPrimitives.h>

#define MAX_CARD 100
#define WCARD 200
#define HCARD 280
#define WSCREEN 1920
#define HSCREEN 1080
#define WField 100
#define HField 140
#define HAND_SIZE 5
#define XCENTER 960
#define YCENTER 540

// Add missing MIN/MAX macros
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

typedef enum {
    BASIC_ATK,  //基礎攻擊牌
    BASIC_DEF,  //基礎防禦牌
    BASIC_MOV,  //基礎移動牌
    UNIVERSAL,  //通用牌
    SKILL_ATK,  //技能攻擊牌
    SKILL_DEF,  //技能防禦牌
    SKILL_MOV,  //技能移動牌
    EPIC,       //必殺牌
    TWIST       //蛻變牌、反轉牌
} Type;

typedef struct {
    char name[50];
    Type type;
    int val;
    int32_t cst;        // energy cost
    int32_t dmg;        // for attack
    int32_t defense;        // for defense
    int32_t mov;        // lane movement
    int32_t rng;        // 0 = melee , 1 = adjacent, 2 = global
    bool link;
    void (*effect)(void *self, void *target);
} Card;

typedef struct {
    Card *cards[MAX_CARD];
    int32_t cnt;
} Deck;

typedef struct{
    Deck Attack;
    Deck Defense;
    Deck Move;
}FableShop;

typedef struct {
    char name[50];
    SDL_Color Piece;
    int32_t health;
    int32_t energy;
    int32_t defense;
    int32_t epic_threshold;
    int32_t lane;       //0, 1, or 2
    Deck skill[3];  //技能牌庫
    Card epic[3];
    void* character_state; // Generic pointer for character-specific state
} Fable;

typedef struct {
    Fable *fable;
    int health;
    int power;
    int defense;
    int pos;
    Deck draw;          //牌庫（抽牌區）
    Deck disc;          //棄牌區
    Deck bani;          //出牌區
    Deck hand;          //手牌
    Deck twist;         //反轉牌區
} Player;

// Forward declarations for character management functions
void alice_turn_start(Player* player);
void alice_turn_end(Player* player);
void alice_update(Player* player, float delta_time);

void kaguya_turn_start(Player* player);
void kaguya_turn_end(Player* player);
void kaguya_on_damage_taken(Player* target, int damage, Player* source);

void mulan_turn_start(Player* player);
void mulan_turn_end(Player* player);
void mulan_on_damage_taken(Player* target, int damage, Player* source);

void snow_white_turn_start(Player* player);
void snow_white_turn_end(Player* player);

void match_girl_turn_start(Player* player);
void match_girl_turn_end(Player* player);
void match_girl_update(Player* player, float delta_time);

void red_hood_turn_start(Player* player);
void red_hood_turn_end(Player* player);

#endif
