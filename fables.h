#ifndef FABLES_H_INCLUDED
#define FABLES_H_INCLUDED
#include "base.h"

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
} Fable;

typedef struct {
    Fable *fable;
    Deck draw;          //牌庫（抽牌區）
    Deck disc;          //棄牌區
    Deck bani;          //出牌區
    Deck hand;          //手牌
    Deck twist;         //反轉牌區
} Player;

Card potshot={
    .name="Pot Shot",
    .type=SKILL_ATK,
    .cst=0,
    .dmg=1,
    .defense=0,
    .mov=0,
    .rng=1,
    .link=true,
    .effect=NULL //請填入效果函式
};
Card crackshot={
    .name="Crack Shot",
    .type=SKILL_ATK,
    .cst=2,
    .dmg=2,
    .defense=0,
    .mov=0,
    .rng=2,
    .link=true,
    .effect=NULL //請填入效果函式
};
Card overdrive_burn={
    .name="Overdrive Burn",
    .type=SKILL_ATK,
    .cst=0,
    .dmg=0,
    .defense=0,
    .mov=0,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card sniper_shot={
    .name="Sniper Shot",
    .type=SKILL_ATK,
    .cst=4,
    .dmg=3,
    .defense=0,
    .mov=0,
    .rng=3,
    .link=true,
    .effect=NULL //請填入效果函式
};
Card onboard_cacheA={
    .name="Onboard Cache",
    .type=SKILL_ATK,
    .cst=1,
    .dmg=0,
    .defense=0,
    .mov=0,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card energy_shield={
    .name="Energy Shield",
    .type=SKILL_DEF,
    .cst=1,
    .dmg=0,
    .defense=3,
    .mov=0,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card electrified_shield={
    .name="Electrified Shield",
    .type=SKILL_DEF,
    .cst=2,
    .dmg=0,
    .defense=4,
    .mov=0,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card hooded_system={
    .name="Hooded System",
    .type=SKILL_DEF,
    .cst=1,
    .dmg=0,
    .defense=5,
    .mov=0,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card omega_shield={
    .name="Omega Shield",
    .type=SKILL_DEF,
    .cst=3,
    .dmg=0,
    .defense=6,
    .mov=0,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card onboard_cacheD={
    .name="Onboard Cache",
    .type=SKILL_DEF,
    .cst=1,
    .dmg=0,
    .defense=0,
    .mov=0,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card unleashed_ballistics={
    .name="Unleashed Ballistics",
    .type=SKILL_MOV,
    .cst=1,
    .dmg=0,
    .defense=0,
    .mov=2,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card unleashed_firepower={
    .name="Unleashed Firepower",
    .type=SKILL_MOV,
    .cst=2,
    .dmg=0,
    .defense=0,
    .mov=3,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card transformed_senses={
    .name="Transformed Senses",
    .type=SKILL_MOV,
    .cst=1,
    .dmg=0,
    .defense=0,
    .mov=1,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card unleashed_fury={
    .name="Unleashed Fury",
    .type=SKILL_MOV,
    .cst=3,
    .dmg=0,
    .defense=0,
    .mov=4,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card onboard_cacheM={
    .name="Onboard Cache",
    .type=SKILL_MOV,
    .cst=1,
    .dmg=0,
    .defense=0,
    .mov=0,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Fable red_hood={
    .name="Red Riding Hood",
    .Piece={255, 0, 0, 255},
    .health=30,
    .energy=0,
    .defense=0,
    .epic_threshold=15,
    .lane=0,
    .skill[0]={
        .cards={&potshot, &crackshot, &overdrive_burn, &sniper_shot, &onboard_cacheA},
        .cnt=5
    },
    .skill[1]={
        .cards={&energy_shield, &electrified_shield, &hooded_system, &omega_shield, &onboard_cacheD},
        .cnt=5
    },
    .skill[2]={
        .cards={&unleashed_ballistics, &unleashed_firepower, &transformed_senses, &unleashed_fury, &onboard_cacheM},
        .cnt=5
    },
};
#endif
