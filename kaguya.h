#ifndef KAGUYA_H
#define KAGUYA_H

#include "base.h"

// 輝夜姬狀態管理結構 - Use different struct name to avoid conflicts
/*typedef struct KaguyaGameState {
    Player *player;                  // 指向使用輝夜姬的玩家
    int defense_limit_bonus;         // 防禦上限加成（透過蛻變牌獲得）
    bool can_use_defense_as_attack;  // 本回合是否可以將防禦牌當攻擊牌使用
    bool reflective_repose_count; // 月下沉思蛻變牌的數量（可累積）
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
void reflective_repose_effect(Player *p);
void reset_turn_abilities(Player *p);

// 卡牌效果函式宣告
void ascendant_light_effect(void *self, void *target);
void ascendant_glory_effect(void *self, void *target);
void ascendant_avatar_effect(void *self, void *target);
void swift_reprisal_effect(void *self, void *target);

void addled_echoes_effect(void *self, void *target);
void bygone_echoes_effect(void *self, void *target);
void divine_echoes_effect(void *self, void *target);
void blood_moon_effect(void *self, void *target);

void focused_intent_effect(void *self, void *target);
void focused_resolve_effect(void *self, void *target);
void focused_ascent_effect(void *self, void *target);
void uncanny_instinct_effect(void *self, void *target);

// 必殺技效果函式
void blazing_shinai_effect(void *self, void *target);
void judgement_arising_effect(void *self, void *target);
void impulsive_onslaught_effect(void *self, void *target);

// -------------------------------------
// 攻擊卡
extern Card ascendant_light;       // 領悟的光芒
extern Card ascendant_glory;      // 領悟的榮耀  
extern Card ascendant_avatar;     // 領悟的化身
extern Card swift_reprisal;    // 懲戒時刻 (蛻變1)

// 防禦卡
extern Card addled_echoes;          // 困惑的回聲
extern Card bygone_echoes;           // 久遠的回響
extern Card divine_echoes;          // 神性的召換
extern Card blood_moonlight;        // 血色月光 (蛻變1)

// 移動卡
extern Card focused_intent;  // 專注的自省
extern Card focused_resolve;    // 頓悟的決心
extern Card focused_ascent;   // 痛徹的淨化
extern Card uncanny_instinct;     // 靈性本能 (蛻變1)

// 共同蛻變卡
extern Card reflective_repose;   // 月下沉思 (蛻變2)

// EPIC 必殺卡
extern Card blazing_shinai;   // 炙熱的竹刀
extern Card judgement_arising;      // 注定的審判
extern Card impulsive_onslaught;     // 躁動的血性

#endif
