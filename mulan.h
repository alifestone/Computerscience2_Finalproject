#ifndef MULAN_H_INCLUDED
#define MULAN_H_INCLUDED
#include "base.h"

// 前置宣告效果函式
void mulan_attack_effect(void *self, void *target);
void mulan_defense_effect(void *self, void *target);
void mulan_movement_effect(void *self, void *target);
void mulan_special_rule_effect(void *self, void *target);

// Epic 效果函式宣告
void qichongyunxiao_effect(void *self, void *target);
void zhimianhuandun_effect(void *self, void *target);
void leitingyiji_effect(void *self, void *target);

// 攻擊卡牌宣告
extern Card burongxiaoqu;        // 不容小覷 (等級一)
extern Card shibukedang;         // 勢不可擋 (等級二)
extern Card jianbukecu;          // 堅不可摧 (等級三)
extern Card qiguanquanshen;      // 氣慣全身 (蛻變1)
extern Card baofengqianxi_atk;   // 暴風前夕 (蛻變2) - 攻擊版

// 防禦卡牌宣告
extern Card yijingzhidong;       // 以靜制動 (等級一)
extern Card yiroukegang;         // 以柔克剛 (等級二)
extern Card yiruoshengqiang;     // 以弱勝強 (等級三)
extern Card zhuzaimingyun;       // 主宰命運 (蛻變1)
extern Card baofengqianxi_def;   // 暴風前夕 (蛻變2) - 防禦版

// 移動卡牌宣告
extern Card yongbutuisuo;        // 永不退縮 (等級一)
extern Card haobuliuqing;        // 毫不留情 (等級二)
extern Card jueburao恕;          // 絕不饒恕 (等級三)
extern Card changquzhiru;        // 長驅直入 (蛻變1)
extern Card baofengqianxi_mov;   // 暴風前夕 (蛻變2) - 移動版

// 必殺技宣告
extern Card qichongyunxiao;      // 氣沖雲霄
extern Card zhimianhuandun;      // 直面混沌
extern Card leitingyiji;         // 雷霆一擊

// 角色宣告
extern Fable mulan;

// 設定函式宣告
void setup_mulan(Player *p);

#endif