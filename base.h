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
#define WCARD 100
#define HCARD 140
#define WSCREEN 1920
#define HSCREEN 1080
#define HAND_SIZE 5
#define XCENTER 960
#define YCENTER 540

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
#endif
