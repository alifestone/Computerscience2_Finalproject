#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<stdint.h>
#include<stdbool.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include<SDL2/SDL2_gfxPrimitives.h>
#define MAX_CARD 100
#define WCARD 100
#define HCARD 140
#define WSCREEN 1200
#define HSCREEN 800
#define HAND_SIZE 5

// 卡牌類型枚舉
typedef enum{
    BASIC_ATK,  // 基礎攻擊牌
    BASIC_DEF,  // 基礎防禦牌
    BASIC_MOV,  // 基礎移動牌
    UNIVERSAL,  // 通用牌
    SKILL_ATK,  // 技能攻擊牌
    SKILL_DEF,  // 技能防禦牌
    SKILL_MOV,  // 技能移動牌
    EPIC,       // 必殺牌
    TWIST       // 蛻變牌、反轉牌
} Type;

// 卡牌結構體
typedef struct{
    Type type;
    int32_t cst;        // 能量消耗
    int32_t dmg;        // 攻擊傷害（也代表卡牌等級）
    int32_t blk;        // 防禦值
    int32_t mov;        // 移動距離
    int32_t rng;        // 射程：0=近戰, 1=相鄰, 2=全域
    void (*effect)(void *self, void *target, Type type, int32_t dmg);
} Card;

// 牌庫結構體
typedef struct{
    Card *cards[MAX_CARD];
    int32_t cnt;        // 卡牌數量
} Deck;

// 角色基本屬性結構體
typedef struct{
    char name[50];
    int32_t health;         // 生命值
    int32_t energy;         // 能量值
    int32_t blk;           // 護盾值
    int32_t epic_threshold; // 必殺技閾值
    int32_t lane;          // 位置（0, 1, 或 2）
    int32_t qi;            // 花木蘭特有的「氣」資源
} Fable;

// 玩家結構體
typedef struct{
    Fable *fable;
    Deck draw;          // 牌庫（抽牌區）
    Deck disc;          // 棄牌區
    Deck bani;          // 出牌區
    Deck hand;          // 手牌
    Deck twist;         // 反轉牌區
    Deck epic;          // 必殺牌
} Player;

//-------------------- 遊戲機制函數 --------------------

// 向牌庫添加卡牌
void add_deck(Deck *deck, Card *card){
    if(deck->cnt < MAX_CARD) {
        deck->cards[deck->cnt++] = card;
    }
}

// 從牌庫抽取卡牌
Card *draw_deck(Deck *deck){
    if(deck->cnt == 0) return NULL;
    return deck->cards[--deck->cnt];
}

// 洗牌函數
void shuffle_deck(Deck *deck){
    for(int i = deck->cnt - 1; i > 0; i--){
        int32_t j = rand() % (i + 1);
        Card *tmp = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = tmp;
    }
}

// 抽取手牌
void draw_hand(Player *player){
    for(int i = 0; i < HAND_SIZE; i++){
        Card *card = draw_deck(&player->draw);
        if(card) add_deck(&player->hand, card);
    }
}

// 檢查對手是否在戰場邊緣
bool is_at_edge(Player *target){
    return (target->fable->lane == 0 || target->fable->lane == 2);
}

// 隨機棄掉對手手牌
void discard_random_card(Player *target){
    if(target->hand.cnt > 0){
        int random_index = rand() % target->hand.cnt;
        // 將選中的卡牌移動到棄牌區
        add_deck(&target->disc, target->hand.cards[random_index]);
        // 從手牌中移除
        for(int i = random_index; i < target->hand.cnt - 1; i++){
            target->hand.cards[i] = target->hand.cards[i + 1];
        }
        target->hand.cnt--;
        printf("對手被迫棄掉一張手牌！\n");
    }
}

// 移動角色到相鄰位置
void move_adjacent_to_target(Player *attacker, Player *target){
    int target_lane = target->fable->lane;
    // 簡化處理：移動到目標相鄰的位置
    if(target_lane == 0) attacker->fable->lane = 1;
    else if(target_lane == 2) attacker->fable->lane = 1;
    else attacker->fable->lane = (rand() % 2 == 0) ? 0 : 2;
    printf("%s移動到與對手相鄰的位置！\n", attacker->fable->name);
}

//-------------------- 花木蘭專用效果函數 --------------------

void mulan_effect(void *self, void *target, Type type, int32_t dmg){
    Player *attacker = (Player *)self;
    Player *defender = (Player *)target;
    
    printf("\n=== 花木蘭使用技能 ===\n");
    printf("技能類型: %d, 等級: %d\n", type, dmg);
    printf("當前氣值: %d\n", attacker->fable->qi);
    
    // 根據卡牌類型和等級執行對應效果
    switch(type){
        case SKILL_ATK: // 攻擊技能
            if(dmg >= 1 && dmg <= 3){
                // 不容小覷/勢不可擋/堅不可摧 系列
                printf("使用攻擊技能等級 %d！\n", dmg);
                
                // 基礎傷害
                int damage = dmg;
                printf("造成 %d 點基礎傷害！\n", damage);
                
                // 扣除對手生命值（考慮護盾）
                if(defender->fable->blk > 0){
                    int blocked = (defender->fable->blk >= damage) ? damage : defender->fable->blk;
                    defender->fable->blk -= blocked;
                    damage -= blocked;
                    printf("護盾吸收 %d 點傷害，剩餘護盾: %d\n", blocked, defender->fable->blk);
                }
                if(damage > 0){
                    defender->fable->health -= damage;
                    printf("對手受到 %d 點傷害，剩餘生命: %d\n", damage, defender->fable->health);
                }
                
                // 移動對手到相鄰格子
                move_adjacent_to_target(attacker, defender);
                
                // 如果對手在邊緣，棄掉其一張手牌
                if(is_at_edge(defender)){
                    printf("對手位於戰場邊緣！\n");
                    discard_random_card(defender);
                }
            }
            break;
            
        case SKILL_DEF: // 防禦技能
            if(dmg >= 1 && dmg <= 3){
                // 以靜制動/以柔克剛/以弱勝強 系列
                printf("使用防禦技能等級 %d！\n", dmg);
                printf("獲得防禦效果，回合結束時可消耗氣來額外抽牌（最多%d張）\n", dmg);
                // 這裡可以設定一個標記，在回合結束時處理額外抽牌效果
            }
            break;
            
        case SKILL_MOV: // 移動技能
            if(dmg >= 1 && dmg <= 3){
                // 永不退縮/毫不留情/絕不饒恕 系列
                printf("使用移動技能等級 %d！\n", dmg);
                
                // 造成傷害
                int damage = dmg;
                printf("造成 %d 點傷害！\n", damage);
                
                if(defender->fable->blk > 0){
                    int blocked = (defender->fable->blk >= damage) ? damage : defender->fable->blk;
                    defender->fable->blk -= blocked;
                    damage -= blocked;
                    printf("護盾吸收 %d 點傷害\n", blocked);
                }
                if(damage > 0){
                    defender->fable->health -= damage;
                    printf("對手受到 %d 點傷害\n", damage);
                }
                
                // 擊退0格（此版本簡化處理）
                printf("擊退對手 0 格！\n");
                
                // 移動到對手相鄰位置
                move_adjacent_to_target(attacker, defender);
                
                // 邊緣效果
                if(is_at_edge(defender)){
                    printf("對手位於戰場邊緣！\n");
                    discard_random_card(defender);
                }
            }
            break;
            
        case TWIST: // 蛻變牌
            switch(dmg){
                case 1: // 氣貫全身
                    printf("發動蛻變：氣貫全身！\n");
                    printf("攻擊時可花費氣來增加傷害（最多3點）！\n");
                    break;
                    
                case 2: // 主宰命運
                    printf("發動蛻變：主宰命運！\n");
                    printf("抽牌後可棄1張手牌再抽1張！\n");
                    break;
                    
                case 3: // 長驅直入
                    printf("發動蛻變：長驅直入！\n");
                    printf("移動時可花費氣來額外移動（最多3格）！\n");
                    break;
                    
                case 4: // 暴風前夕
                    printf("發動蛻變：暴風前夕！\n");
                    attacker->fable->qi += 1;
                    printf("獲得1點氣！當前氣值: %d\n", attacker->fable->qi);
                    printf("此效果可以累積！\n");
                    break;
            }
            break;
            
        case EPIC: // 必殺技
            switch(dmg){
                case 1: // 氣沖雲霄
                    printf("發動必殺技：氣沖雲霄！\n");
                    printf("回合結束時額外抽取4張牌！\n");
                    break;
                    
                case 2: // 直面混沌
                    printf("發動必殺技：直面混沌！\n");
                    move_adjacent_to_target(attacker, defender);
                    attacker->fable->qi += 3;
                    printf("立即獲得3點氣！當前氣值: %d\n", attacker->fable->qi);
                    break;
                    
                case 3: // 雷霆一擊
                    printf("發動必殺技：雷霆一擊！\n");
                    int qi_damage = attacker->fable->qi;
                    attacker->fable->qi = 0; // 消耗所有氣
                    
                    printf("消耗所有氣(%d點)造成等量傷害！\n", qi_damage);
                    
                    if(defender->fable->blk > 0){
                        int blocked = (defender->fable->blk >= qi_damage) ? qi_damage : defender->fable->blk;
                        defender->fable->blk -= blocked;
                        qi_damage -= blocked;
                        printf("護盾吸收 %d 點傷害\n", blocked);
                    }
                    if(qi_damage > 0){
                        defender->fable->health -= qi_damage;
                        printf("對手受到 %d 點雷霆傷害！\n", qi_damage);
                    }
                    break;
            }
            break;
    }
    
    printf("========================\n");
}

// 花木蘭的特殊防禦反應（對手回合被攻擊時觸發）
void mulan_defensive_reaction(Player *mulan_player, Player *attacker, int incoming_damage){
    printf("\n=== 花木蘭防禦反應 ===\n");
    printf("受到 %d 點傷害，可以棄掉防禦牌來減傷並獲得氣！\n", incoming_damage);
    
    // 這裡簡化處理，假設玩家選擇棄掉一張等級2的防禦牌
    if(mulan_player->hand.cnt > 0){
        printf("棄掉一張2級防禦牌，減少2點傷害並獲得2點氣！\n");
        mulan_player->fable->qi += 2;
        // 實際實現中需要讓玩家選擇要棄掉的牌
        printf("當前氣值: %d\n", mulan_player->fable->qi);
    }
    printf("=====================\n");
}

//-------------------- 角色設置函數 --------------------

void setup_mulan(Player *p){
    printf("正在設置花木蘭角色...\n");
    
    // === 定義所有技能卡牌 ===
    
    // 攻擊系列卡牌
    static Card attack_lv1 = { // 不容小覷
        .type = SKILL_ATK,
        .cst = 0,
        .dmg = 1,
        .blk = 0,
        .mov = 0,
        .rng = 1,
        .effect = mulan_effect
    };
    
    static Card attack_lv2 = { // 勢不可擋
        .type = SKILL_ATK,
        .cst = 2,
        .dmg = 2,
        .blk = 0,
        .mov = 0,
        .rng = 1,
        .effect = mulan_effect
    };
    
    static Card attack_lv3 = { // 堅不可摧
        .type = SKILL_ATK,
        .cst = 4,
        .dmg = 3,
        .blk = 0,
        .mov = 0,
        .rng = 1,
        .effect = mulan_effect
    };
    
    // 防禦系列卡牌
    static Card defense_lv1 = { // 以靜制動
        .type = SKILL_DEF,
        .cst = 0,
        .dmg = 1,
        .blk = 0, // 防禦值由特殊效果處理
        .mov = 0,
        .rng = 0,
        .effect = mulan_effect
    };
    
    static Card defense_lv2 = { // 以柔克剛
        .type = SKILL_DEF,
        .cst = 2,
        .dmg = 2,
        .blk = 0,
        .mov = 0,
        .rng = 0,
        .effect = mulan_effect
    };
    
    static Card defense_lv3 = { // 以弱勝強
        .type = SKILL_DEF,
        .cst = 4,
        .dmg = 3,
        .blk = 0,
        .mov = 0,
        .rng = 0,
        .effect = mulan_effect
    };
    
    // 移動系列卡牌
    static Card move_lv1 = { // 永不退縮
        .type = SKILL_MOV,
        .cst = 0,
        .dmg = 1,
        .blk = 0,
        .mov = 1,
        .rng = 1,
        .effect = mulan_effect
    };
    
    static Card move_lv2 = { // 毫不留情
        .type = SKILL_MOV,
        .cst = 2,
        .dmg = 2,
        .blk = 0,
        .mov = 1,
        .rng = 1,
        .effect = mulan_effect
    };
    
    static Card move_lv3 = { // 絕不饒恕
        .type = SKILL_MOV,
        .cst = 4,
        .dmg = 3,
        .blk = 0,
        .mov = 1,
        .rng = 1,
        .effect = mulan_effect
    };
    
    // 蛻變卡牌
    static Card twist_qi_power = { // 氣貫全身
        .type = TWIST,
        .cst = 0,
        .dmg = 1,
        .blk = 0,
        .mov = 0,
        .rng = 0,
        .effect = mulan_effect
    };
    
    static Card twist_fate_control = { // 主宰命運
        .type = TWIST,
        .cst = 0,
        .dmg = 2,
        .blk = 0,
        .mov = 0,
        .rng = 0,
        .effect = mulan_effect
    };
    
    static Card twist_advance = { // 長驅直入
        .type = TWIST,
        .cst = 0,
        .dmg = 3,
        .blk = 0,
        .mov = 0,
        .rng = 0,
        .effect = mulan_effect
    };
    
    static Card twist_storm = { // 暴風前夕
        .type = TWIST,
        .cst = 0,
        .dmg = 4,
        .blk = 0,
        .mov = 0,
        .rng = 0,
        .effect = mulan_effect
    };
    
    // 必殺技卡牌
    static Card epic_sky_rage = { // 氣沖雲霄
        .type = EPIC,
        .cst = 0,
        .dmg = 1,
        .blk = 0,
        .mov = 0,
        .rng = 0,
        .effect = mulan_effect
    };
    
    static Card epic_face_chaos = { // 直面混沌
        .type = EPIC,
        .cst = 0,
        .dmg = 2,
        .blk = 0,
        .mov = 0,
        .rng = 0,
        .effect = mulan_effect
    };
    
    static Card epic_thunder_strike = { // 雷霆一擊
        .type = EPIC,
        .cst = 0,
        .dmg = 3,
        .blk = 0,
        .mov = 0,
        .rng = 1,
        .effect = mulan_effect
    };
    
    // === 創建花木蘭角色 ===
    Fable *mulan = malloc(sizeof(Fable));
    
    *mulan = (Fable){
        .health = 34,           // 生命上限
        .energy = 6,            // 起始能量值（可調整）
        .blk = 0,              // 起始護盾
        .epic_threshold = 17,   // 必殺技閾值
        .lane = 1,             // 起始位置在中央
        .qi = 0                // 起始氣值為0
    };
    
    strcpy(mulan->name, "花木蘭");
    p->fable = mulan;
    
    // === 初始化牌庫 ===
    p->draw.cnt = 0;
    p->disc.cnt = 0;
    p->bani.cnt = 0;
    p->hand.cnt = 0;
    p->twist.cnt = 0;
    p->epic.cnt = 0;
    
    // === 將卡牌加入牌庫（根據描述的數量） ===
    
    // 攻擊卡牌
    add_deck(&p->draw, &attack_lv1);     // 不容小覷 x1
    add_deck(&p->draw, &attack_lv2);     // 勢不可擋 x2
    add_deck(&p->draw, &attack_lv2);
    add_deck(&p->draw, &attack_lv3);     // 堅不可摧 x3
    add_deck(&p->draw, &attack_lv3);
    add_deck(&p->draw, &attack_lv3);
    
    // 防禦卡牌
    add_deck(&p->draw, &defense_lv1);    // 以靜制動 x1
    add_deck(&p->draw, &defense_lv2);    // 以柔克剛 x2
    add_deck(&p->draw, &defense_lv2);
    add_deck(&p->draw, &defense_lv3);    // 以弱勝強 x3
    add_deck(&p->draw, &defense_lv3);
    add_deck(&p->draw, &defense_lv3);
    
    // 移動卡牌
    add_deck(&p->draw, &move_lv1);       // 永不退縮 x1
    add_deck(&p->draw, &move_lv2);       // 毫不留情 x2
    add_deck(&p->draw, &move_lv2);
    add_deck(&p->draw, &move_lv3);       // 絕不饒恕 x3
    add_deck(&p->draw, &move_lv3);
    add_deck(&p->draw, &move_lv3);
    
    // 蛻變卡牌
    add_deck(&p->twist, &twist_qi_power);    // 氣貫全身 x1
    add_deck(&p->twist, &twist_fate_control); // 主宰命運 x1  
    add_deck(&p->twist, &twist_advance);     // 長驅直入 x1
    add_deck(&p->twist, &twist_storm);       // 暴風前夕 x1 (攻擊系列)
    add_deck(&p->twist, &twist_storm);       // 暴風前夕 x1 (防禦系列)
    add_deck(&p->twist, &twist_storm);       // 暴風前夕 x1 (移動系列)
    
    // 必殺技卡牌
    add_deck(&p->epic, &epic_sky_rage);      // 氣沖雲霄
    add_deck(&p->epic, &epic_face_chaos);    // 直面混沌
    add_deck(&p->epic, &epic_thunder_strike); // 雷霆一擊
    
    // === 洗牌並抽取起始手牌 ===
    shuffle_deck(&p->draw);
    draw_hand(p);
    
    printf("花木蘭設置完成！\n");
    printf("生命值: %d\n", mulan->health);
    printf("必殺技閾值: %d\n", mulan->epic_threshold);
    printf("牌庫卡牌數: %d\n", p->draw.cnt);
    printf("手牌數: %d\n", p->hand.cnt);
    printf("蛻變卡牌數: %d\n", p->twist.cnt);
    printf("必殺技卡牌數: %d\n", p->epic.cnt);
    printf("========================\n");
}

//-------------------- 測試函數 --------------------

void test_mulan_abilities(){
    printf("\n=== 花木蘭能力測試 ===\n");
    
    // 創建花木蘭玩家
    Player mulan_player;
    setup_mulan(&mulan_player);
    
    // 創建假想敵人
    Player enemy;
    Fable enemy_fable = {
        .health = 30,
        .energy = 6,
        .blk = 2,
        .epic_threshold = 15,
        .lane = 0,
        .qi = 0
    };
    strcpy(enemy_fable.name, "敵人");
    enemy.fable = &enemy_fable;
    
    printf("\n--- 測試攻擊技能 ---\n");
    // 測試等級1攻擊
    mulan_effect(&mulan_player, &enemy, SKILL_ATK, 1);
    
    printf("\n--- 測試必殺技 ---\n");
    mulan_player.fable->qi = 5; // 設置一些氣值
    // 測試雷霆一擊
    mulan_effect(&mulan_player, &enemy, EPIC, 3);
    
    printf("\n--- 測試蛻變技能 ---\n");
    // 測試暴風前夕
    mulan_effect(&mulan_player, &enemy, TWIST, 4);
    
    printf("\n=== 測試完成 ===\n");
}

// 主函數示例
int main(){
    srand(time(NULL));
    
    printf("歡迎來到《反轉寓言》- 花木蘭角色系統\n");
    printf("=====================================\n");
    
    test_mulan_abilities();
    
    return 0;
}