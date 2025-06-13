#ifndef MATCH_GIRL_H
#define MATCH_GIRL_H

#include "base.h"

// 火柴女孩角色資料
extern Fable match_girl;

// 額外狀態：火柴牌堆與狀態
typedef struct {
    Player *player;
    Deck match_pile;
    bool match_placed_this_turn;
} MGState;

extern MGState match_girl_state;

// 初始化角色與火柴牌庫
void init_match_girl_fable(void);
void init_match_pile(void);

// 每回合清除放置狀態
void reset_match_placement(Player *p);

// 檢查是否與敵人相鄰
bool is_adjacent(Player *p1, Player *p2);

// 嘗試將火柴牌放到對手牌庫頂
bool try_place_match(Player *self, Player *enemy);

// 當對手使用火柴牌時觸發 +1 能量
void on_card_played(Player *who_played, Card *card);

// -------------------------------------
// 攻擊卡
extern Card phantom_wish;
extern Card secret_desire;
extern Card endless_luxury;

// 防禦卡
extern Card devil_sacrifice;
extern Card devil_bet;
extern Card devil_contract;

// 移動卡
extern Card weightless_soul;
extern Card indebted_soul;
extern Card broken_soul;

// TWIST 蛻變卡
extern Card ritual_of_pain;
extern Card flame_trick;
extern Card torment_of_fate;
extern Card indulgent_desire;
extern Card devil_gaze;
extern Card desire_trick;

// EPIC 必殺卡
extern Card hell_flame;
extern Card doomfall;
extern Card curse_of_greed;

#endif
