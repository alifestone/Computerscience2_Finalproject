#include "snow_white.h"

//-------------------- 中毒牌庫 --------------------
// 中毒牌庫：等級1 2 3中毒各6張
Card poison1 = {
    .name = "Poison\nLv1",
    .type = TWIST,
    .val = 1,
    .cst = 0,
    .dmg = 0,
    .defense = 0,
    .mov = 0,
    .rng = 0,
    .link = false,
    .effect = NULL
};

Card poison2 = {
    .name = "Poison\nLv2", 
    .type = TWIST,
    .val = 2,
    .cst = 0,
    .dmg = 0,
    .defense = 0,
    .mov = 0,
    .rng = 0,
    .link = false,
    .effect = NULL
};

Card poison3 = {
    .name = "Poison\nLv3",
    .type = TWIST,
    .val = 3,
    .cst = 0,
    .dmg = 0,
    .defense = 0,
    .mov = 0,
    .rng = 0,
    .link = false,
    .effect = NULL
};

// 中毒牌庫
Deck poison_deck = {
    .cards = {&poison1, &poison1, &poison1, &poison1, &poison1, &poison1,
              &poison2, &poison2, &poison2, &poison2, &poison2, &poison2,
              &poison3, &poison3, &poison3, &poison3, &poison3, &poison3},
    .cnt = 18
};

//-------------------- 攻擊技能卡 --------------------
Card crystal_fragment = {
    .name = "Crystal\nFragment",
    .type = SKILL_ATK,
    .val = 1,
    .cst = 1,
    .dmg = 1,
    .defense = 0,
    .mov = 0,
    .rng = 1,
    .link = false,
    .effect = snow_white_effect
};

Card crystal_vortex = {
    .name = "Crystal\nVortex",
    .type = SKILL_ATK,
    .val = 2,
    .cst = 2,
    .dmg = 2,
    .defense = 0,
    .mov = 0,
    .rng = 1,
    .link = false,
    .effect = snow_white_effect
};

Card crystal_coffin = {
    .name = "Crystal\nCoffin",
    .type = TWIST,
    .val = 1,
    .cst = 0,
    .dmg = 0,
    .defense = 0,
    .mov = 0,
    .rng = 0,
    .link = false,
    .effect = snow_white_effect
};

Card crystal_storm = {
    .name = "Crystal\nStorm",
    .type = SKILL_ATK,
    .val = 3,
    .cst = 4,
    .dmg = 3,
    .defense = 0,
    .mov = 0,
    .rng = 1,
    .link = false,
    .effect = snow_white_effect
};

Card pure_poison = {
    .name = "Pure\nPoison",
    .type = TWIST,
    .val = 2,
    .cst = 0,
    .dmg = 0,
    .defense = 0,
    .mov = 0,
    .rng = 0,
    .link = false,
    .effect = snow_white_effect
};

//-------------------- 防禦技能卡 --------------------
Card tainted_blessing = {
    .name = "Tainted\nBlessing",
    .type = SKILL_DEF,
    .val = 1,
    .cst = 1,
    .dmg = 1,
    .defense = 0,
    .mov = 0,
    .rng = 1,
    .link = false,
    .effect = snow_white_effect
};

Card tainted_feast = {
    .name = "Tainted\nFeast", 
    .type = SKILL_DEF,
    .val = 2,
    .cst = 2,
    .dmg = 2,
    .defense = 0,
    .mov = 0,
    .rng = 1,
    .link = false,
    .effect = snow_white_effect
};

Card fallen_calamity = {
    .name = "Fallen\nCalamity",
    .type = TWIST,
    .val = 1,
    .cst = 0,
    .dmg = 0,
    .defense = 0,
    .mov = 0,
    .rng = 0,
    .link = false,
    .effect = snow_white_effect
};

Card tainted_carnival = {
    .name = "Tainted\nCarnival",
    .type = SKILL_DEF,
    .val = 3,
    .cst = 4,
    .dmg = 3,
    .defense = 0,
    .mov = 0,
    .rng = 1,
    .link = false,
    .effect = snow_white_effect
};

//-------------------- 移動技能卡 --------------------
Card broken_fantasy = {
    .name = "Broken\nFantasy",
    .type = SKILL_MOV,
    .val = 1,
    .cst = 1,
    .dmg = 1,
    .defense = 0,
    .mov = 0,
    .rng = 0,
    .link = false,
    .effect = snow_white_effect
};

Card broken_reality = {
    .name = "Broken\nReality",
    .type = SKILL_MOV,
    .val = 2,
    .cst = 2,
    .dmg = 2,
    .defense = 0,
    .mov = 0,
    .rng = 1,
    .link = false,
    .effect = snow_white_effect
};

Card toxic_erosion = {
    .name = "Toxic\nErosion",
    .type = TWIST,
    .val = 1,
    .cst = 0,
    .dmg = 0,
    .defense = 0,
    .mov = 0,
    .rng = 0,
    .link = false,
    .effect = snow_white_effect
};

Card broken_destiny = {
    .name = "Broken\nDestiny",
    .type = SKILL_MOV,
    .val = 3,
    .cst = 4,
    .dmg = 3,
    .defense = 0,
    .mov = 0,
    .rng = 2,
    .link = false,
    .effect = snow_white_effect
};

//-------------------- 必殺技 --------------------
Card seven_serpent_rage = {
    .name = "Seven Serpent\nRage",
    .type = EPIC,
    .val = 0,
    .cst = 0,
    .dmg = 0,
    .defense = 0,
    .mov = 0,
    .rng = 1,
    .link = false,
    .effect = snow_white_effect
};

Card mirror_rain = {
    .name = "Mirror\nRain",
    .type = EPIC,
    .val = 0,
    .cst = 0,
    .dmg = 3,
    .defense = 0,
    .mov = 0,
    .rng = 1,
    .link = false,
    .effect = snow_white_effect
};

Card brewing_disaster = {
    .name = "Brewing\nDisaster",
    .type = EPIC,
    .val = 0,
    .cst = 0,
    .dmg = 3,
    .defense = 0,
    .mov = 0,
    .rng = 3,
    .link = false,
    .effect = snow_white_effect
};

//-------------------- 角色定義 --------------------
Fable snow_white = {
    .name = "Snow White",
    .Piece = {255, 255, 255, 255}, // 白色
    .health = 34,
    .energy = 25,
    .defense = 6,
    .epic_threshold = 17,
    .lane = 0,
    .skill[0] = {
        .cards = {&crystal_fragment, &crystal_vortex, &crystal_coffin, &crystal_storm, &pure_poison},
        .cnt = 5
    },
    .skill[1] = {
        .cards = {&tainted_blessing, &tainted_feast, &fallen_calamity, &tainted_carnival, &pure_poison},
        .cnt = 5
    },
    .skill[2] = {
        .cards = {&broken_fantasy, &broken_reality, &toxic_erosion, &broken_destiny, &pure_poison},
        .cnt = 5
    },
    .epic[0] = seven_serpent_rage,
    .epic[1] = mirror_rain,
    .epic[2] = brewing_disaster
};

//-------------------- 效果函數 --------------------
void snow_white_effect(void *self, void *target) {
    Player *attacker = (Player *)self;
    Player *defender = (Player *)target;
    
    // 這裡實作各種卡牌效果
    // 由於效果函數需要知道是哪張卡牌被使用，我們需要額外的參數
    // 在實際遊戲中，可能需要修改函數簽名來傳遞卡牌資訊
    
    // 暫時的範例實作：
    // 如果是攻擊卡牌，額外將對手牌庫頂部卡牌移到棄牌堆
    if (attacker->hand.cnt > 0) {
        Card *used_card = attacker->hand.cards[0]; // 假設使用第一張卡
        
        if (used_card->type == SKILL_ATK) {
            // 水晶碎片/漩渦/風暴效果：將對手牌庫頂部卡牌棄掉
            for (int i = 0; i < used_card->val && defender->draw.cnt > 0; i++) {
                Card *discarded = draw_deck(&defender->draw);
                if (discarded) {
                    add_deck(&defender->disc, discarded);
                }
            }
        }
        
        if (used_card->type == SKILL_DEF) {
            // 玷污系列效果：將中毒牌放入對手棄牌堆
            for (int i = 0; i < used_card->val && poison_deck.cnt > 0; i++) {
                Card *poison = draw_deck(&poison_deck);
                if (poison) {
                    add_deck(&defender->disc, poison);
                    // 如果有純毒效果（pure_poison），對手額外失去1點生命
                    // 這需要檢查是否有蛻變牌生效
                }
            }
        }
        
        if (used_card->type == SKILL_MOV) {
            // 破碎系列效果：移動到對手相鄰位置
            // 這需要在主遊戲邏輯中處理位置移動
        }
        
        if (used_card->type == EPIC) {
            // 必殺技效果
            if (used_card == &seven_serpent_rage) {
                // 七蛇之怒：對手失去X點生命，X等於他棄牌堆中中毒牌的數量
                int poison_count = 0;
                for (int i = 0; i < defender->disc.cnt; i++) {
                    if (defender->disc.cards[i]->type == TWIST && 
                        strstr(defender->disc.cards[i]->name, "Poison") != NULL) {
                        poison_count++;
                    }
                }
                defender->health -= poison_count;
            }
            
            if (used_card == &mirror_rain) {
                // 魔鏡之雨：傷害3，對手必須棄掉全部手牌之後抽取4張牌
                defender->health -= 3;
                // 棄掉全部手牌
                for (int i = 0; i < defender->hand.cnt; i++) {
                    add_deck(&defender->disc, defender->hand.cards[i]);
                }
                defender->hand.cnt = 0;
                // 抽取4張牌
                draw_hand(defender, 4);
            }
            
            if (used_card == &brewing_disaster) {
                // 醞釀之災：傷害3，選擇將對手棄牌堆中至多3張牌洗入他的牌庫
                defender->health -= 3;
                // 這個效果需要玩家選擇，在實際實作中需要UI支援
            }
        }
    }
}

//-------------------- 設置函數 --------------------
void setup_snow_white(Player *p) {
    // 設置角色基本資料
    p->fable = &snow_white;
    p->health = snow_white.health;
    p->power = 0;
    p->defense = 0;
    p->pos = 0; // 初始位置
    
    // 清空所有牌組
    p->draw.cnt = 0;
    p->disc.cnt = 0;
    p->bani.cnt = 0;
    p->hand.cnt = 0;
    p->twist.cnt = 0;
    
    // 建立起始牌組（3張基本攻擊、3張基本防禦、3張基本移動）
    for (int i = 0; i < 3; i++) {
        add_deck(&p->draw, &Attack1);
        add_deck(&p->draw, &Defense1);
        add_deck(&p->draw, &Move1);
    }
    
    // 加入起始技能牌
    add_deck(&p->draw, &crystal_fragment);    // 水晶碎片
    add_deck(&p->draw, &tainted_blessing);    // 玷污的恩惠
    add_deck(&p->draw, &broken_fantasy);      // 破碎的幻想
    
    // 洗牌
    shuffle_deck(&p->draw);
    
    printf("Snow White setup complete. Health: %d, Defense limit: %d, Epic threshold: %d\n", 
           p->health, p->fable->defense, p->fable->epic_threshold);
}