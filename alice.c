#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<stdint.h>
#include<stdbool.h>

// 基本定義 (從universal.c繼承)
#define MAX_CARD 100
#define HAND_SIZE 5

// 愛麗絲專用的身份枚舉
typedef enum {
    IDENTITY_RED_QUEEN,    // 紅心皇后：攻擊+1, 移動-1
    IDENTITY_MAD_HATTER,   // 瘋帽子：防禦+1, 攻擊-1  
    IDENTITY_CHESHIRE_CAT  // 柴郡貓：移動+1, 防禦-1
} AliceIdentity;

typedef enum{
    BASIC_ATK = 0,  // 基礎攻擊牌
    BASIC_DEF,      // 基礎防禦牌
    BASIC_MOV,      // 基礎移動牌
    UNIVERSAL,      // 通用牌
    SKILL_ATK,      // 技能攻擊牌
    SKILL_DEF,      // 技能防禦牌
    SKILL_MOV,      // 技能移動牌
    EPIC,           // 必殺牌
    TWIST           // 蛻變牌、反轉牌
} Type;

typedef struct{
    Type type;
    int32_t cst;        // 能量費用
    int32_t dmg;        // 傷害值（也代表等級）
    int32_t blk;        // 防禦值
    int32_t mov;        // 移動值
    int32_t rng;        // 射程：0=近戰, 1=相鄰, 2=全域
    void (*effect)(void *self, void *target, Type type, int32_t dmg);
} Card;

typedef struct{
    Card *cards[MAX_CARD];
    int32_t cnt;        // 牌數量
} Deck;

typedef struct{
    char name[50];
    int32_t health;
    int32_t energy;
    int32_t blk;
    int32_t epic_threshold;
    int32_t lane;       // 位置：0, 1, 或 2
    AliceIdentity current_identity;  // 愛麗絲當前身份
} Fable;

typedef struct{
    Fable *fable;
    Deck draw;          // 抽牌區
    Deck disc;          // 棄牌區
    Deck bani;          // 出牌區
    Deck hand;          // 手牌
    Deck twist;         // 反轉牌區
    Deck epic;          // 必殺牌
} Player;

// 基本牌庫操作函數
void add_deck(Deck *deck, Card *card){
    if(deck->cnt < MAX_CARD) {
        deck->cards[deck->cnt++] = card;
    }
}

Card *draw_deck(Deck *deck){
    if(deck->cnt == 0) return NULL;
    return deck->cards[--deck->cnt];
}

void shuffle_deck(Deck *deck){
    for(int i = deck->cnt - 1; i > 0; i--){
        int32_t j = rand() % (i + 1);
        Card *tmp = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = tmp;
    }
}

void draw_hand(Player *player){
    for(int i = 0; i < HAND_SIZE; i++){
        Card *card = draw_deck(&player->draw);
        if(card) add_deck(&player->hand, card);
    }
}

// 愛麗絲專用身份切換函數
void switch_alice_identity(Player *player, AliceIdentity new_identity) {
    if(player->fable->current_identity != new_identity) {
        player->fable->current_identity = new_identity;
        printf("愛麗絲轉換身份為：");
        switch(new_identity) {
            case IDENTITY_RED_QUEEN:
                printf("紅心皇后 (攻擊+1, 移動-1)\n");
                break;
            case IDENTITY_MAD_HATTER:
                printf("瘋帽子 (防禦+1, 攻擊-1)\n");
                break;
            case IDENTITY_CHESHIRE_CAT:
                printf("柴郡貓 (移動+1, 防禦-1)\n");
                break;
        }
    }
}

// 根據身份調整牌的數值
int32_t get_modified_value(Player *player, Type card_type, int32_t base_value) {
    AliceIdentity identity = player->fable->current_identity;
    
    switch(identity) {
        case IDENTITY_RED_QUEEN:
            if(card_type == SKILL_ATK) return base_value + 1;
            if(card_type == SKILL_MOV) return base_value - 1;
            break;
        case IDENTITY_MAD_HATTER:
            if(card_type == SKILL_DEF) return base_value + 1;
            if(card_type == SKILL_ATK) return base_value - 1;
            break;
        case IDENTITY_CHESHIRE_CAT:
            if(card_type == SKILL_MOV) return base_value + 1;
            if(card_type == SKILL_DEF) return base_value - 1;
            break;
    }
    return base_value;
}

// 愛麗絲的卡牌效果實作
void alice_effect(void *self, void *target, Type type, int32_t dmg) {
    Player *atker = (Player *)self;
    Player *defer = (Player *)target;
    
    // 根據卡牌類型和等級決定效果
    if(type == SKILL_ATK) {
        switch(dmg) {
            case 1: // 開啟牌局
                printf("愛麗絲使用「開啟牌局」\n");
                printf("對手需展示3張基本牌，每有一張不能展示，愛麗絲獲得+1傷害\n");
                // 這裡可以轉換為紅心皇后
                switch_alice_identity(atker, IDENTITY_RED_QUEEN);
                break;
            case 2: // 扭轉牌局
                printf("愛麗絲使用「扭轉牌局」\n");
                printf("射程2，對手需展示4張基本牌\n");
                switch_alice_identity(atker, IDENTITY_RED_QUEEN);
                break;
            case 3: // 操控牌局
                printf("愛麗絲使用「操控牌局」\n");
                printf("射程3，對手需展示5張基本牌\n");
                switch_alice_identity(atker, IDENTITY_RED_QUEEN);
                break;
        }
    }
    else if(type == SKILL_DEF) {
        switch(dmg) {
            case 1: // 魔力技巧
                printf("愛麗絲使用「魔力技巧」\n");
                printf("防禦1，可從手牌或棄牌堆移除1張基本牌\n");
                switch_alice_identity(atker, IDENTITY_MAD_HATTER);
                break;
            case 2: // 精神幻術
                printf("愛麗絲使用「精神幻術」\n");
                printf("防禦2，可移除基本牌並獲得新牌\n");
                switch_alice_identity(atker, IDENTITY_MAD_HATTER);
                break;
            case 3: // 帽子戲法
                printf("愛麗絲使用「帽子戲法」\n");
                printf("防禦3，強化版魔力技巧\n");
                switch_alice_identity(atker, IDENTITY_MAD_HATTER);
                break;
        }
    }
    else if(type == SKILL_MOV) {
        switch(dmg) {
            case 1: // 詭異的敏捷
                printf("愛麗絲使用「詭異的敏捷」\n");
                printf("移動1格，穿過對手時抽1張牌\n");
                switch_alice_identity(atker, IDENTITY_CHESHIRE_CAT);
                break;
            case 2: // 詭異的隱蔽
                printf("愛麗絲使用「詭異的隱蔽」\n");
                printf("移動2格，穿過對手時抽2張牌\n");
                switch_alice_identity(atker, IDENTITY_CHESHIRE_CAT);
                break;
            case 3: // 詭異的詭異
                printf("愛麗絲使用「詭異的詭異」\n");
                printf("移動3格，穿過對手時抽3張牌\n");
                switch_alice_identity(atker, IDENTITY_CHESHIRE_CAT);
                break;
        }
    }
    else if(type == TWIST) {
        switch(dmg) {
            case 1: // 砍掉她的頭 (紅心皇后蛻變)
                printf("愛麗絲使用蛻變「砍掉她的頭」\n");
                printf("身份為紅心皇后時，攻擊技能獲得的牌可加入手牌\n");
                break;
            case 2: // 仙境降臨 (瘋帽子蛻變)
                printf("愛麗絲使用蛻變「仙境降臨」\n");
                printf("身份為瘋帽子時，防禦技能獲得的牌可選擇加入手牌\n");
                break;
            case 3: // 我們全是瘋子 (柴郡貓蛻變)
                printf("愛麗絲使用蛻變「我們全是瘋子」\n");
                printf("身份為柴郡貓時，穿越效果增強\n");
                break;
            case 4: // 開始我的表演 (通用蛻變)
                printf("愛麗絲使用蛻變「開始我的表演」\n");
                printf("回合結束時額外抽1張牌，可疊加\n");
                break;
        }
    }
    else if(type == EPIC) {
        switch(dmg) {
            case 1: // 無休止的派對
                printf("愛麗絲使用必殺「無休止的派對」\n");
                printf("抽牌後棄至4張手牌，立即開始新回合\n");
                break;
            case 2: // 精彩的奇妙日
                printf("愛麗絲使用必殺「精彩的奇妙日」\n");
                printf("本回合所有牌的數值+1\n");
                break;
            case 3: // 遊戲盡在掌控
                printf("愛麗絲使用必殺「遊戲盡在掌控」\n");
                printf("射程3，傷害3，將基本牌洗入對手牌庫\n");
                break;
        }
    }
}

// 愛麗絲角色設置函數
void setup_alice(Player *p) {
    // 創建攻擊技能牌
    static Card attack_lv1 = {
        .type = SKILL_ATK,
        .cst = 0,      // 等級1免費
        .dmg = 1,      // 等級1
        .blk = 0,
        .mov = 0,
        .rng = 1,      // 射程1
        .effect = alice_effect
    };
    
    static Card attack_lv2 = {
        .type = SKILL_ATK,
        .cst = 2,      // 購買費用2
        .dmg = 2,      // 等級2
        .blk = 0,
        .mov = 0,
        .rng = 2,      // 射程2
        .effect = alice_effect
    };
    
    static Card attack_lv3 = {
        .type = SKILL_ATK,
        .cst = 4,      // 購買費用4
        .dmg = 3,      // 等級3
        .blk = 0,
        .mov = 0,
        .rng = 3,      // 射程3
        .effect = alice_effect
    };
    
    // 創建防禦技能牌
    static Card defense_lv1 = {
        .type = SKILL_DEF,
        .cst = 0,
        .dmg = 1,
        .blk = 1,
        .mov = 0,
        .rng = 0,
        .effect = alice_effect
    };
    
    static Card defense_lv2 = {
        .type = SKILL_DEF,
        .cst = 2,
        .dmg = 2,
        .blk = 2,
        .mov = 0,
        .rng = 0,
        .effect = alice_effect
    };
    
    static Card defense_lv3 = {
        .type = SKILL_DEF,
        .cst = 4,
        .dmg = 3,
        .blk = 3,
        .mov = 0,
        .rng = 0,
        .effect = alice_effect
    };
    
    // 創建移動技能牌
    static Card movement_lv1 = {
        .type = SKILL_MOV,
        .cst = 0,
        .dmg = 1,
        .blk = 0,
        .mov = 1,
        .rng = 0,
        .effect = alice_effect
    };
    
    static Card movement_lv2 = {
        .type = SKILL_MOV,
        .cst = 2,
        .dmg = 2,
        .blk = 0,
        .mov = 2,
        .rng = 0,
        .effect = alice_effect
    };
    
    static Card movement_lv3 = {
        .type = SKILL_MOV,
        .cst = 4,
        .dmg = 3,
        .blk = 0,
        .mov = 3,
        .rng = 0,
        .effect = alice_effect
    };
    
    // 創建蛻變牌
    static Card twist_cards[4] = {
        {TWIST, 0, 1, 0, 0, 0, alice_effect}, // 砍掉她的頭
        {TWIST, 0, 2, 0, 0, 0, alice_effect}, // 仙境降臨
        {TWIST, 0, 3, 0, 0, 0, alice_effect}, // 我們全是瘋子
        {TWIST, 0, 4, 0, 0, 0, alice_effect}  // 開始我的表演
    };
    
    // 創建必殺牌
    static Card epic_cards[3] = {
        {EPIC, 0, 1, 0, 0, 0, alice_effect}, // 無休止的派對
        {EPIC, 0, 2, 0, 0, 0, alice_effect}, // 精彩的奇妙日
        {EPIC, 0, 3, 0, 0, 3, alice_effect}  // 遊戲盡在掌控
    };
    
    // 創建愛麗絲角色
    Fable *alice = malloc(sizeof(Fable));
    *alice = (Fable){
        .health = 32,
        .energy = 0,
        .blk = 0,
        .epic_threshold = 16,
        .lane = 1,  // 初始位置
        .current_identity = IDENTITY_RED_QUEEN  // 預設身份為紅心皇后
    };
    strcpy(alice->name, "愛麗絲");
    p->fable = alice;
    
    // 將牌加入牌庫（按照遊戲規則的數量）
    // 攻擊牌：等級1有1張，等級2有2張，等級3有3張
    add_deck(&p->draw, &attack_lv1);
    add_deck(&p->draw, &attack_lv2);
    add_deck(&p->draw, &attack_lv2);
    add_deck(&p->draw, &attack_lv3);
    add_deck(&p->draw, &attack_lv3);
    add_deck(&p->draw, &attack_lv3);
    
    // 防禦牌：等級1有1張，等級2有2張，等級3有3張
    add_deck(&p->draw, &defense_lv1);
    add_deck(&p->draw, &defense_lv2);
    add_deck(&p->draw, &defense_lv2);
    add_deck(&p->draw, &defense_lv3);
    add_deck(&p->draw, &defense_lv3);
    add_deck(&p->draw, &defense_lv3);
    
    // 移動牌：等級1有1張，等級2有2張，等級3有2張（注意：等級3只有2張）
    add_deck(&p->draw, &movement_lv1);
    add_deck(&p->draw, &movement_lv2);
    add_deck(&p->draw, &movement_lv2);
    add_deck(&p->draw, &movement_lv3);
    add_deck(&p->draw, &movement_lv3);
    
    // 蛻變牌：每種各1張
    for(int i = 0; i < 4; i++) {
        add_deck(&p->draw, &twist_cards[i]);
    }
    
    // 必殺牌：每種各1張
    for(int i = 0; i < 3; i++) {
        add_deck(&p->epic, &epic_cards[i]);
    }
    
    // 洗牌並抽取起始手牌
    shuffle_deck(&p->draw);
    draw_hand(p);
    
    printf("愛麗絲已準備就緒！\n");
    printf("- 生命值：%d\n", alice->health);
    printf("- 護盾上限：6\n");
    printf("- 必殺閾值：%d\n", alice->epic_threshold);
    printf("- 當前身份：紅心皇后\n");
    printf("- 牌庫共有：%d張牌\n", p->draw.cnt);
    printf("- 手牌：%d張\n", p->hand.cnt);
}

// 示範用的主函數
int main() {
    srand(time(NULL));
    
    Player alice_player = {0};
    
    printf("=== 愛麗絲角色初始化 ===\n");
    setup_alice(&alice_player);
    
    printf("\n=== 模擬使用技能 ===\n");
    
    // 模擬使用不同的技能
    printf("\n1. 使用攻擊技能：\n");
    alice_effect(&alice_player, NULL, SKILL_ATK, 1);
    
    printf("\n2. 使用防禦技能：\n");
    alice_effect(&alice_player, NULL, SKILL_DEF, 2);
    
    printf("\n3. 使用移動技能：\n");
    alice_effect(&alice_player, NULL, SKILL_MOV, 3);
    
    printf("\n4. 使用蛻變牌：\n");
    alice_effect(&alice_player, NULL, TWIST, 4);
    
    printf("\n5. 使用必殺技：\n");
    alice_effect(&alice_player, NULL, EPIC, 1);
    
    // 清理記憶體
    free(alice_player.fable);
    
    return 0;
}