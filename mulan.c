#include "mulan.h"
#include "cards.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//-------------------- 攻擊技能卡牌 --------------------

// 等級一：不容小覷 - 花木蘭的基礎攻擊，體現她不容被輕視的特質
Card burongxiaoqu = {
    .name = "Bu Rong\nXiao Qu",           // 不容小覷
    .type = SKILL_ATK,
    .val = 1,                             // 卡牌等級，影響獲得能量
    .cst = 0,                             // 無耗費，讓玩家容易使用
    .dmg = 1,                             // 基礎傷害1點
    .defense = 0,
    .mov = 0,
    .rng = 1,                             // 射程1，近戰攻擊
    .link = false,
    .effect = mulan_attack_effect         // 特殊效果：位置操控+棄牌
};

// 等級二：勢不可擋 - 花木蘭勢如破竹的攻勢
Card shibukedang = {
    .name = "Shi Bu\nKe Dang",           // 勢不可擋
    .type = SKILL_ATK,
    .val = 2,
    .cst = 2,                             // 需要2點能量購買
    .dmg = 2,                             // 傷害提升到2點
    .defense = 0,
    .mov = 0,
    .rng = 1,
    .link = false,
    .effect = mulan_attack_effect         // 相同的位置操控效果
};

// 等級三：堅不可摧 - 花木蘭無堅不摧的終極攻擊
Card jianbukecu = {
    .name = "Jian Bu\nKe Cui",           // 堅不可摧
    .type = SKILL_ATK,
    .val = 3,
    .cst = 4,                             // 高級卡牌需要更多資源
    .dmg = 3,                             // 最高傷害3點
    .defense = 0,
    .mov = 0,
    .rng = 1,
    .link = false,
    .effect = mulan_attack_effect
};

// 蛻變1：氣慣全身 - 花木蘭內力爆發，可消耗氣來增強攻擊
Card qiguanquanshen = {
    .name = "Qi Guan\nQuan Shen",       // 氣慣全身
    .type = TWIST,                        // 蛻變卡類型
    .val = 1,
    .cst = 0,
    .dmg = 0,                             // 蛻變卡本身不直接造成傷害
    .defense = 0,
    .mov = 0,
    .rng = 0,
    .link = false,
    .effect = NULL                        // 被動效果，在攻擊時觸發
};

// 蛻變2：暴風前夕 (攻擊版) - 每回合開始獲得氣，為戰鬥積蓄力量
Card baofengqianxi_atk = {
    .name = "Bao Feng\nQian Xi",         // 暴風前夕
    .type = TWIST,
    .val = 2,
    .cst = 0,
    .dmg = 0,
    .defense = 0,
    .mov = 0,
    .rng = 0,
    .link = false,
    .effect = NULL                        // 回合開始時觸發的被動效果
};

//-------------------- 防禦技能卡牌 --------------------

// 等級一：以靜制動 - 通過冷靜防守來獲得戰術優勢
Card yijingzhidong = {
    .name = "Yi Jing\nZhi Dong",         // 以靜制動
    .type = SKILL_DEF,
    .val = 1,
    .cst = 0,
    .dmg = 0,
    .defense = 0,                         // 防禦值為O（根據描述）
    .mov = 0,
    .rng = 0,
    .link = false,
    .effect = mulan_defense_effect       // 回合結束時額外抽牌效果
};

// 等級二：以柔克剛 - 用柔韌化解強攻
Card yiroukegang = {
    .name = "Yi Rou\nKe Gang",           // 以柔克剛
    .type = SKILL_DEF,
    .val = 2,
    .cst = 2,
    .dmg = 0,
    .defense = 0,
    .mov = 0,
    .rng = 0,
    .link = false,
    .effect = mulan_defense_effect       // 可額外抽更多牌（X≤2）
};

// 等級三：以弱勝強 - 花木蘭以弱勝強的戰術智慧
Card yiruoshengqiang = {
    .name = "Yi Ruo\nSheng Qiang",       // 以弱勝強
    .type = SKILL_DEF,
    .val = 3,
    .cst = 4,
    .dmg = 0,
    .defense = 0,
    .mov = 0,
    .rng = 0,
    .link = false,
    .effect = mulan_defense_effect       // 最多可額外抽3張牌
};

// 蛻變1：主宰命運 - 掌控自己的命運，優化手牌品質
Card zhuzaimingyun = {
    .name = "Zhu Zai\nMing Yun",         // 主宰命運
    .type = TWIST,
    .val = 1,
    .cst = 0,
    .dmg = 0,
    .defense = 0,
    .mov = 0,
    .rng = 0,
    .link = false,
    .effect = NULL                        // 手牌交換效果
};

// 蛻變2：暴風前夕 (防禦版) - 同攻擊版，累積氣息
Card baofengqianxi_def = {
    .name = "Bao Feng\nQian Xi",         // 暴風前夕
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

//-------------------- 移動技能卡牌 --------------------

// 等級一：永不退縮 - 體現花木蘭勇往直前的精神
Card yongbutuisuo = {
    .name = "Yong Bu\nTui Suo",          // 永不退縮
    .type = SKILL_MOV,
    .val = 1,
    .cst = 0,
    .dmg = 1,                             // 移動中攻擊，造成1點傷害
    .defense = 0,
    .mov = 0,                             // 特殊移動方式，不是固定移動格數
    .rng = 1,
    .link = false,
    .effect = mulan_movement_effect       // 擊退+重新定位效果
};

// 等級二：毫不留情 - 攻勢如潮，不給敵人喘息機會
Card haobuliuqing = {
    .name = "Hao Bu\nLiu Qing",          // 毫不留情
    .type = SKILL_MOV,
    .val = 2,
    .cst = 2,
    .dmg = 2,                             // 更強的攻擊力
    .defense = 0,
    .mov = 0,
    .rng = 1,
    .link = false,
    .effect = mulan_movement_effect
};

// 等級三：絕不饒恕 - 花木蘭最強的移動攻擊技能
Card jueburao恕 = {
    .name = "Jue Bu\nRao Shu",           // 絕不饒恕
    .type = SKILL_MOV,
    .val = 3,
    .cst = 4,
    .dmg = 3,                             // 最高移動攻擊傷害
    .defense = 0,
    .mov = 0,
    .rng = 1,
    .link = false,
    .effect = mulan_movement_effect
};

// 蛻變1：長驅直入 - 利用氣息加強機動性
Card changquzhiru = {
    .name = "Chang Qu\nZhi Ru",          // 長驅直入
    .type = TWIST,
    .val = 1,
    .cst = 0,
    .dmg = 0,
    .defense = 0,
    .mov = 0,
    .rng = 0,
    .link = false,
    .effect = NULL                        // 移動行動時的增強效果
};

// 蛻變2：暴風前夕 (移動版)
Card baofengqianxi_mov = {
    .name = "Bao Feng\nQian Xi",         // 暴風前夕
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

//-------------------- 必殺技卡牌 --------------------

// 氣沖雲霄 - 爆發式抽牌，獲得巨大手牌優勢
Card qichongyunxiao = {
    .name = "Qi Chong\nYun Xiao",        // 氣沖雲霄
    .type = EPIC,
    .val = 0,
    .cst = 0,                             // 必殺技不需要額外費用
    .dmg = 0,
    .defense = 0,
    .mov = 0,
    .rng = 0,
    .link = false,
    .effect = qichongyunxiao_effect      // 額外抽4張牌
};

// 直面混沌 - 直接面對敵人，獲得戰術位置和氣息
Card zhimianhuandun = {
    .name = "Zhi Mian\nHuan Dun",        // 直面混沌
    .type = EPIC,
    .val = 0,
    .cst = 0,
    .dmg = 0,
    .defense = 0,
    .mov = 0,
    .rng = 0,
    .link = false,
    .effect = zhimianhuandun_effect      // 移動到相鄰位置+獲得3氣
};

// 雷霆一擊 - 花木蘭的終極攻擊，消耗所有氣息造成巨大傷害
Card leitingyiji = {
    .name = "Lei Ting\nYi Ji",           // 雷霆一擊
    .type = EPIC,
    .val = 0,
    .cst = 0,
    .dmg = 0,                             // 傷害取決於消耗的氣息
    .defense = 0,
    .mov = 0,
    .rng = 1,
    .link = false,
    .effect = leitingyiji_effect         // 消耗所有氣轉化為傷害
};

//-------------------- 效果函式實作 --------------------

// 攻擊效果：位置操控+邊緣棄牌
void mulan_attack_effect(void *self, void *target) {
    Player *attacker = (Player *)self;
    Player *defender = (Player *)target;
    
    printf("花木蘭使用攻擊技能！\n");
    // 這裡需要實作：
    // 1. 將對手重新定位到相鄰格子
    // 2. 如果對手在邊緣，從手牌中隨機棄掉一張牌
    // 具體實作需要配合遊戲的位置系統
}

// 防禦效果：回合結束時額外抽牌
void mulan_defense_effect(void *self, void *target) {
    Player *player = (Player *)self;
    
    printf("花木蘭使用防禦技能，可在回合結束時額外抽牌！\n");
    // 這裡需要實作：
    // 根據卡牌等級，允許花費氣息來額外抽牌
}

// 移動效果：擊退+重新定位+邊緣棄牌
void mulan_movement_effect(void *self, void *target) {
    Player *attacker = (Player *)self;
    Player *defender = (Player *)target;
    
    printf("花木蘭使用移動攻擊！\n");
    // 這裡需要實作：
    // 1. 對敵人造成傷害
    // 2. 將敵人擊退O格（根據卡牌描述）
    // 3. 將自己移動到與敵人相鄰的位置
    // 4. 如果敵人在邊緣，可棄掉手牌
}

// 必殺技效果實作
void qichongyunxiao_effect(void *self, void *target) {
    Player *player = (Player *)self;
    
    printf("氣沖雲霄！花木蘭額外抽取4張牌！\n");
    // 實作：在回合結束階段額外抽4張牌
}

void zhimianhuandun_effect(void *self, void *target) {
    Player *attacker = (Player *)self;
    Player *defender = (Player *)target;
    
    printf("直面混沌！花木蘭移動到敵人身邊並獲得3點氣！\n");
    // 實作：移動到與對手相鄰的格子，立即獲得3點氣
}

void leitingyiji_effect(void *self, void *target) {
    Player *attacker = (Player *)self;
    Player *defender = (Player *)target;
    
    printf("雷霆一擊！花木蘭消耗所有氣息發動致命攻擊！\n");
    // 實作：
    // 1. 獲取玩家當前所有氣息
    // 2. 造成等量傷害
    // 3. 清空氣息
}

// 花木蘭特殊規則效果
void mulan_special_rule_effect(void *self, void *target) {
    Player *defender = (Player *)self;
    
    printf("花木蘭的特殊能力觸發：可棄防禦牌減傷並獲得氣息！\n");
    // 實作：
    // 1. 檢查手牌中的防禦牌或通用牌
    // 2. 允許玩家選擇棄掉一張
    // 3. 減少等量傷害並獲得等於卡牌等級的氣息
}

//-------------------- 角色設定 --------------------

// 花木蘭角色定義
Fable mulan = {
    .name = "Mulan",                      // 角色名稱
    .Piece = {218, 165, 32, 255},        // 金黃色，象徵花木蘭的英勇
    .health = 34,                         // 生命上限：34點，體現她的堅韌
    .energy = 0,                          // 初始能量（遊戲開始時設定）
    .defense = 3,                         // 護盾上限：3點，相對較低但有特殊能力補償
    .epic_threshold = 17,                 // 必殺閥值：17點，生命值一半時可使用
    .lane = 1,                            // 初始位置設為中央
    
    // 技能牌庫設定：每個類型包含5張卡牌
    .skill[0] = {                         // 攻擊技能牌庫
        .cards = {&burongxiaoqu, &shibukedang, &shibukedang, 
                  &jianbukecu, &jianbukecu},
        .cnt = 5
    },
    .skill[1] = {                         // 防禦技能牌庫
        .cards = {&yijingzhidong, &yiroukegang, &yiroukegang,
                  &yiruoshengqiang, &yiruoshengqiang},
        .cnt = 5
    },
    .skill[2] = {                         // 移動技能牌庫
        .cards = {&yongbutuisuo, &haobuliuqing, &haobuliuqing,
                  &jueburao恕, &jueburao恕},
        .cnt = 5
    },
    
    // 必殺技設定
    .epic[0] = qichongyunxiao,            // 氣沖雲霄
    .epic[1] = zhimianhuandun,            // 直面混沌  
    .epic[2] = leitingyiji                // 雷霆一擊
};

//-------------------- 設定函式 --------------------

// 花木蘭角色設定函式 - 依照universal.c的模式實作
void setup_mulan(Player *p) {
    // 設定玩家使用花木蘭角色
    p->fable = &mulan;
    p->health = mulan.health;             // 設定初始生命值
    p->power = 0;                         // 初始能量為0
    p->defense = 0;                       // 初始防禦為0
    p->pos = 0;                           // 初始位置設為中央
    
    // 建構起始牌組：
    // 根據遊戲規則，每個角色開始時擁有基本牌組
    for(int i = 0; i < 3; i++) {
        add_deck(&p->draw, &Attack1);     // 3張基礎攻擊牌
        add_deck(&p->draw, &Defense1);    // 3張基礎防禦牌
        add_deck(&p->draw, &Move1);       // 3張基礎移動牌
    }
    
    // 加入花木蘭的特色技能牌（各一張等級1技能）
    add_deck(&p->draw, &burongxiaoqu);    // 不容小覷
    add_deck(&p->draw, &yijingzhidong);   // 以靜制動
    add_deck(&p->draw, &yongbutuisuo);    // 永不退縮
    
    // 洗牌以確保隨機性
    shuffle_deck(&p->draw);
    
    printf("花木蘭準備就緒！生命：%d，防禦上限：%d，必殺閥值：%d\n", 
           mulan.health, mulan.defense, mulan.epic_threshold);
}