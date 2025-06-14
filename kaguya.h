#ifndef KAGUYA_H
#define KAGUYA_H

#include "base.h"

// 輝夜姬狀態管理結構 - Use different struct name to avoid conflicts
/*typedef struct KaguyaGameState {
    Player *player;                  // 指向使用輝夜姬的玩家
    int defense_limit_bonus;         // 防禦上限加成（透過蛻變牌獲得）
    bool can_use_defense_as_attack;  // 本回合是否可以將防禦牌當攻擊牌使用
    bool moonlight_meditation_count; // 月下沉思蛻變牌的數量（可累積）
} KaguyaGameState;

extern KaguyaGameState kaguya_state;*/
extern Fable kaguya_fable;

// 初始化函式
void init_kaguya_fable(void);
void init_kaguya_state(Player *p);

// 輝夜姬特殊機制函式
int get_current_defense_limit(Player *p);
bool has_defense_bonus(Player *p);
void trigger_counter_attack(Player *defender, Player *attacker, int damage);
void moonlight_meditation_effect(Player *p);
void reset_turn_abilities(Player *p);

// 卡牌效果函式宣告
void enlightened_glow_effect(void *self, void *target);
void enlightened_glory_effect(void *self, void *target);
void enlightened_avatar_effect(void *self, void *target);
void disciplinary_moment_effect(void *self, void *target);
void moonlight_meditation_passive_effect(void *self, void *target);

void confused_echo_effect(void *self, void *target);
void distant_echo_effect(void *self, void *target);
void divine_summon_effect(void *self, void *target);
void blood_moonlight_effect(void *self, void *target);

void focused_introspection_effect(void *self, void *target);
void enlightened_resolve_effect(void *self, void *target);
void painful_purification_effect(void *self, void *target);
void spiritual_instinct_effect(void *self, void *target);

// 必殺技效果函式
void blazing_bamboo_sword_effect(void *self, void *target);
void destined_judgment_effect(void *self, void *target);
void restless_bloodlust_effect(void *self, void *target);

// -------------------------------------
// 攻擊卡
extern Card enlightened_glow;       // 領悟的光芒
extern Card enlightened_glory;      // 領悟的榮耀  
extern Card enlightened_avatar;     // 領悟的化身
extern Card disciplinary_moment;    // 懲戒時刻 (蛻變1)

// 防禦卡
extern Card confused_echo;          // 困惑的回聲
extern Card distant_echo;           // 久遠的回響
extern Card divine_summon;          // 神性的召換
extern Card blood_moonlight;        // 血色月光 (蛻變1)

// 移動卡
extern Card focused_introspection;  // 專注的自省
extern Card enlightened_resolve;    // 頓悟的決心
extern Card painful_purification;   // 痛徹的淨化
extern Card spiritual_instinct;     // 靈性本能 (蛻變1)

// 共同蛻變卡
extern Card moonlight_meditation;   // 月下沉思 (蛻變2)

// EPIC 必殺卡
extern Card blazing_bamboo_sword;   // 炙熱的竹刀
extern Card destined_judgment;      // 注定的審判
extern Card restless_bloodlust;     // 躁動的血性

#endif
