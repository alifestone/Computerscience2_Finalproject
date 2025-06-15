#ifndef MULAN_H_INCLUDED
#define MULAN_H_INCLUDED
#include "base.h"

// 前置宣告效果函式
void mulan_attack_effect(void *self, void *target);
void mulan_defense_effect(void *self, void *target);
void mulan_movement_effect(void *self, void *target);
void mulan_special_rule_effect(void *self, void *target);

// Epic 效果函式宣告
void legends_call_effect(void *self, void *target);
void confronting_chaos_effect(void *self, void *target);
void honors_thunder_effect(void *self, void *target);

// 攻擊卡牌宣告
extern Card underestimated;        // 不容小覷 (等級一)
extern Card undeniable;         // 勢不可擋 (等級二)
extern Card undiminished;          // 堅不可摧 (等級三)
extern Card channeled_qi;      // 氣慣全身 (蛻變1)
extern Card gathering_storm;   // 暴風前夕 (蛻變2) - 攻擊版

// 防禦卡牌宣告
extern Card diverted_fire;       // 以靜制動 (等級一)
extern Card diverted_might;         // 以柔克剛 (等級二)
extern Card diverted_wrath;     // 以弱勝強 (等級三)
extern Card evading_fate;       // 主宰命運 (蛻變1)

// 移動卡牌宣告
extern Card no_retreat;        // 永不退縮 (等級一)
extern Card no_quarter;        // 毫不留情 (等級二)
extern Card no_mercy;          // 絕不饒恕 (等級三)
extern Card enlightened_assault;        // 長驅直入 (蛻變1)

// 必殺技宣告
extern Card legends_call;      // 氣沖雲霄
extern Card confronting_chaos;      // 直面混沌
extern Card honors_thunder;         // 雷霆一擊

// 角色宣告
extern Fable mulan_fable;
#endif
