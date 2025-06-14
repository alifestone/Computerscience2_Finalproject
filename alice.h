#ifndef ALICE_H
#define ALICE_H

#include "base.h"

// 愛麗絲身份類型
typedef enum {
    IDENTITY_NONE = 0,
    IDENTITY_QUEEN_OF_HEARTS = 1,  // 紅心皇后
    IDENTITY_MAD_HATTER = 2,       // 瘋帽子
    IDENTITY_CHESHIRE_CAT = 3      // 柴郡貓
} AliceIdentity;

// 愛麗絲狀態追蹤
typedef struct {
    Player *player;                    // 愛麗絲玩家指標
    AliceIdentity current_identity;    // 當前身份
    AliceIdentity available_identities[3]; // 可選擇的身份
    int identity_count;                // 可選擇身份數量
    bool first_turn_identity_selected; // 是否已選擇第一回合身份
    bool identity_changed_this_turn;   // 本回合是否已變換身份
    int show_performance_stacks;       // "開始我的表演"疊加次數
    int endless_party_uses;            // "無休止的派對"使用次數
    bool wonderland_day_active;        // "精彩的奇妙日"是否生效
} AliceState;

extern AliceState alice_state;
extern Fable alice;

// 初始化函數
void init_alice_fable(void);
void init_alice_state(Player *p);

// 身份管理函數
void alice_change_identity(AliceIdentity new_identity);
void alice_reset_identity_change_flag(void);
bool alice_can_change_identity(AliceIdentity target_identity);
int alice_get_attack_modifier(void);
int alice_get_defense_modifier(void);
int alice_get_movement_modifier(void);

// 效果函數宣告
void open_game_effect(void *self, void *target);
void twist_game_effect(void *self, void *target);
void control_game_effect(void *self, void *target);
void magic_trick_effect(void *self, void *target);
void mental_magic_effect(void *self, void *target);
void hat_trick_effect(void *self, void *target);
void strange_agility_effect(void *self, void *target);
void strange_stealth_effect(void *self, void *target);
void strange_strangeness_effect(void *self, void *target);

// 蛻變效果函數
void off_with_her_head_effect(void *self, void *target);
void start_my_show_effect(void *self, void *target);
void wonderland_comes_effect(void *self, void *target);
void we_are_all_mad_effect(void *self, void *target);

// 必殺技效果函數
void endless_party_effect(void *self, void *target);
void wonderful_day_effect(void *self, void *target);
void game_control_effect(void *self, void *target);

// -------------------------------------
// 攻擊卡
extern Card open_game;
extern Card twist_game;
extern Card control_game;

// 防禦卡
extern Card magic_trick;
extern Card mental_magic;
extern Card hat_trick;

// 移動卡
extern Card strange_agility;
extern Card strange_stealth;
extern Card strange_strangeness;

// TWIST 蛻變卡
extern Card off_with_her_head;
extern Card start_my_show;
extern Card wonderland_comes;
extern Card we_are_all_mad;

// EPIC 必殺卡
extern Card endless_party;
extern Card wonderful_day;
extern Card game_control;

#endif