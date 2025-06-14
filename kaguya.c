#include "kaguya.h"
#include "cards.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// 輝夜姬的狀態管理全域變數
KaguyaState kaguya_state = {0};

// ===============================
// 核心機制函式實作 
// ===============================

/**
 * 獲取當前的防禦上限
 * 基礎上限6 + 蛻變牌加成
 */
int get_current_defense_limit(Player *p) {
    int base_limit = 6;  // 輝夜姬基礎防禦上限
    return base_limit + kaguya_state.defense_limit_bonus;
}

/**
 * 檢查是否有防禦加成（防禦≥3時攻擊+1）
 * 這是輝夜姬的核心機制之一
 */
bool has_defense_bonus(Player *p) {
    return (p && p->defense >= 3);
}

/**
 * 觸發反擊機制（輝夜姬的特殊規則）
 * 當受到傷害時，可以棄攻擊牌進行反擊
 */
void trigger_counter_attack(Player *defender, Player *attacker, int damage) {
    if (!defender || !attacker || damage <= 0) return;
    
    // 搜尋手牌中的攻擊牌或通用牌
    for (int i = 0; i < defender->hand.cnt; i++) {
        Card *card = defender->hand.cards[i];
        if (card && (card->type == BASIC_ATK || card->type == SKILL_ATK || card->type == UNIVERSAL)) {
            printf("💫 輝夜姬反擊！棄掉 %s 造成 %d 點傷害並抽1張牌\n", 
                   card->name, card->dmg);
            
            // 對攻擊者造成傷害
            attacker->health -= card->dmg;
            
            // 移除該牌並抽牌
            for (int j = i; j < defender->hand.cnt - 1; j++) {
                defender->hand.cards[j] = defender->hand.cards[j + 1];
            }
            defender->hand.cnt--;
            
            // 從抽牌堆抽一張牌
            Card *drawn = draw_deck(&defender->draw);
            if (drawn) {
                add_deck(&defender->hand, drawn);
            }
            break;  // 每次只能反擊一次
        }
    }
}

/**
 * 月下沉思效果：回合結束時獲得防禦2
 * 這個效果可以累積（有多張月下沉思蛻變牌時）
 */
void moonlight_meditation_effect(Player *p) {
    if (!p) return;
    
    int defense_gain = 2 * kaguya_state.moonlight_meditation_count;
    int current_limit = get_current_defense_limit(p);
    
    if (p->defense + defense_gain > current_limit) {
        p->defense = current_limit;
        printf("🌙 月下沉思：防禦已達上限 %d\n", current_limit);
    } else {
        p->defense += defense_gain;
        printf("🌙 月下沉思：獲得 %d 點防禦\n", defense_gain);
    }
}

/**
 * 重置回合能力（每回合開始時調用）
 */
void reset_turn_abilities(Player *p) {
    if (p == kaguya_state.player) {
        kaguya_state.can_use_defense_as_attack = false;
    }
}

// ===============================
// 攻擊技能卡牌效果
// ===============================

void enlightened_glow_effect(void *self, void *target) {
    Player *attacker = (Player *)self;
    Player *defender = (Player *)target;
    
    int base_damage = 1;
    int final_damage = base_damage;
    
    // 檢查防禦加成
    if (has_defense_bonus(attacker)) {
        final_damage += 1;
        printf("✨ 領悟的光芒：防禦≥3，傷害+1！\n");
    }
    
    printf("⚔️ 領悟的光芒造成 %d 點傷害\n", final_damage);
    defender->health -= final_damage;
}

void enlightened_glory_effect(void *self, void *target) {
    Player *attacker = (Player *)self;
    Player *defender = (Player *)target;
    
    int base_damage = 2;
    int final_damage = base_damage;
    
    if (has_defense_bonus(attacker)) {
        final_damage += 1;
        printf("✨ 領悟的榮耀：防禦≥3，傷害+1！\n");
    }
    
    printf("⚔️ 領悟的榮耀造成 %d 點傷害\n", final_damage);
    defender->health -= final_damage;
}

void enlightened_avatar_effect(void *self, void *target) {
    Player *attacker = (Player *)self;
    Player *defender = (Player *)target;
    
    int base_damage = 3;
    int final_damage = base_damage;
    
    if (has_defense_bonus(attacker)) {
        final_damage += 1;
        printf("✨ 領悟的化身：防禦≥3，傷害+1！\n");
    }
    
    printf("⚔️ 領悟的化身造成 %d 點傷害\n", final_damage);
    defender->health -= final_damage;
}

void disciplinary_moment_effect(void *self, void *target) {
    kaguya_state.defense_limit_bonus += 1;
    kaguya_state.can_use_defense_as_attack = true;
    
    printf("⚖️ 懲戒時刻：防禦上限+1，本回合可將防禦牌當攻擊牌使用！\n");
}

// ===============================
// 防禦技能卡牌效果
// ===============================

/**
 * 困惑的回聲效果：展示牌庫頂1張牌
 * 如果是防禦牌則加入手牌，否則選擇棄掉或放回
 */
void confused_echo_effect(void *self, void *target) {
    Player *p = (Player *)self;
    
    // 基礎防禦效果
    int defense_gain = 1;
    int current_limit = get_current_defense_limit(p);
    
    if (p->defense + defense_gain > current_limit) {
        p->defense = current_limit;
    } else {
        p->defense += defense_gain;
    }
    
    // 展示牌庫頂部的牌
    if (p->draw.cnt > 0) {
        Card *top_card = p->draw.cards[p->draw.cnt - 1];
        printf("🔮 困惑的回聲：展示牌庫頂的 %s\n", top_card->name);
        
        if (top_card->type == BASIC_DEF || top_card->type == SKILL_DEF) {
            // 是防禦牌，加入手牌
            Card *card = draw_deck(&p->draw);
            add_deck(&p->hand, card);
            printf("🛡️ 防禦牌加入手牌！\n");
        } else {
            // 不是防禦牌，模擬玩家選擇（這裡簡化為棄掉）
            Card *card = draw_deck(&p->draw);
            add_deck(&p->disc, card);
            printf("🗑️ 非防禦牌已棄掉\n");
        }
    }
}

void distant_echo_effect(void *self, void *target) {
    Player *p = (Player *)self;
    
    int defense_gain = 2;
    int current_limit = get_current_defense_limit(p);
    
    if (p->defense + defense_gain > current_limit) {
        p->defense = current_limit;
    } else {
        p->defense += defense_gain;
    }
    
    // 展示牌庫頂部2張牌
    int cards_to_show = (p->draw.cnt < 2) ? p->draw.cnt : 2;
    
    for (int i = 0; i < cards_to_show; i++) {
        if (p->draw.cnt > 0) {
            Card *top_card = p->draw.cards[p->draw.cnt - 1];
            printf("🔮 久遠的回響：展示 %s\n", top_card->name);
            
            if (top_card->type == BASIC_DEF || top_card->type == SKILL_DEF) {
                Card *card = draw_deck(&p->draw);
                add_deck(&p->hand, card);
                printf("🛡️ 防禦牌加入手牌！\n");
            } else {
                Card *card = draw_deck(&p->draw);
                add_deck(&p->disc, card);
                printf("🗑️ 非防禦牌已棄掉\n");
            }
        }
    }
}

void divine_summon_effect(void *self, void *target) {
    Player *p = (Player *)self;
    
    int defense_gain = 3;
    int current_limit = get_current_defense_limit(p);
    
    if (p->defense + defense_gain > current_limit) {
        p->defense = current_limit;
    } else {
        p->defense += defense_gain;
    }
    
    // 展示牌庫頂部3張牌
    int cards_to_show = (p->draw.cnt < 3) ? p->draw.cnt : 3;
    
    for (int i = 0; i < cards_to_show; i++) {
        if (p->draw.cnt > 0) {
            Card *top_card = p->draw.cards[p->draw.cnt - 1];
            printf("🔮 神性的召換：展示 %s\n", top_card->name);
            
            if (top_card->type == BASIC_DEF || top_card->type == SKILL_DEF) {
                Card *card = draw_deck(&p->draw);
                add_deck(&p->hand, card);
                printf("🛡️ 防禦牌加入手牌！\n");
            } else {
                Card *card = draw_deck(&p->draw);
                add_deck(&p->disc, card);
                printf("🗑️ 非防禦牌已棄掉\n");
            }
        }
    }
}

void blood_moonlight_effect(void *self, void *target) {
    kaguya_state.defense_limit_bonus += 1;
    printf("🩸 血色月光：防禦上限+1，每重置3點防禦抽1張牌的效果已啟動\n");
}

// ===============================
// 移動技能卡牌效果
// ===============================

void focused_introspection_effect(void *self, void *target) {
    Player *p = (Player *)self;
    
    printf("🧘 專注的自省：可以失去1點生命來移除手牌或棄牌堆中的一張牌\n");
    
    // 這裡簡化實作，實際遊戲中需要讓玩家選擇
    if (p->health > 1 && (p->hand.cnt > 0 || p->disc.cnt > 0)) {
        p->health -= 1;
        
        // 簡化：移除手牌中的第一張牌
        if (p->hand.cnt > 0) {
            printf("🗑️ 移除手牌中的 %s\n", p->hand.cards[0]->name);
            for (int i = 0; i < p->hand.cnt - 1; i++) {
                p->hand.cards[i] = p->hand.cards[i + 1];
            }
            p->hand.cnt--;
        }
    }
}

void enlightened_resolve_effect(void *self, void *target) {
    Player *p = (Player *)self;
    
    focused_introspection_effect(self, target);  // 包含基礎效果
    
    printf("💡 頓悟的決心：持續效果已設置，下回合開始時將檢查對手位置\n");
    // 實際遊戲中需要設置持續效果標記
}

void painful_purification_effect(void *self, void *target) {
    Player *p = (Player *)self;
    
    focused_introspection_effect(self, target);  // 包含基礎效果
    
    printf("💔 痛徹的淨化：持續效果已設置，下回合開始時將造成更多傷害\n");
    // 實際遊戲中需要設置持續效果標記
}

void spiritual_instinct_effect(void *self, void *target) {
    kaguya_state.defense_limit_bonus += 1;
    printf("👁️ 靈性本能：防禦上限+1，可以移動對手的能力已啟動\n");
}

void moonlight_meditation_passive_effect(void *self, void *target) {
    kaguya_state.moonlight_meditation_count++;
    printf("🌙 月下沉思：蛻變牌數量+1（現在：%d張）\n", 
           kaguya_state.moonlight_meditation_count);
}

// ===============================
// 必殺技效果
// ===============================

void blazing_bamboo_sword_effect(void *self, void *target) {
    Player *p = (Player *)self;
    printf("🗡️ 炙熱的竹刀：直到下回合開始，你不會承受傷害也不會失去生命！\n");
    // 實際遊戲中需要設置無敵狀態
}

void destined_judgment_effect(void *self, void *target) {
    Player *attacker = (Player *)self;
    Player *defender = (Player *)target;
    
    int current_limit = get_current_defense_limit(attacker);
    
    // 設置防禦為6
    attacker->defense = (current_limit < 6) ? current_limit : 6;
    
    printf("⚖️ 注定的審判：防禦設為6，下回合將根據防禦差值造成傷害\n");
    // 實際遊戲中需要設置持續效果
}

void restless_bloodlust_effect(void *self, void *target) {
    Player *attacker = (Player *)self;
    Player *defender = (Player *)target;
    
    printf("🩸 躁動的血性：將對手拉到相鄰位置並造成3點傷害！\n");
    defender->health -= 3;
    
    // 實際遊戲中需要設置持續效果，下回合再次觸發
}

// ===============================
// 卡牌定義
// ===============================

// 攻擊卡
Card enlightened_glow = {
    .name = "Enlightened\nGlow", .type = SKILL_ATK, .val = 1, .cst = 0, .dmg = 1,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = enlightened_glow_effect
};

Card enlightened_glory = {
    .name = "Enlightened\nGlory", .type = SKILL_ATK, .val = 2, .cst = 2, .dmg = 2,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = enlightened_glory_effect
};

Card enlightened_avatar = {
    .name = "Enlightened\nAvatar", .type = SKILL_ATK, .val = 3, .cst = 4, .dmg = 3,
    .defense = 0, .mov = 0, .rng = 1, .link = false, .effect = enlightened_avatar_effect
};

Card disciplinary_moment = {
    .name = "Disciplinary\nMoment", .type = TWIST, .val = 1, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = disciplinary_moment_effect
};

// 防禦卡
Card confused_echo = {
    .name = "Confused\nEcho", .type = SKILL_DEF, .val = 1, .cst = 0, .dmg = 0,
    .defense = 1, .mov = 0, .rng = 0, .link = false, .effect = confused_echo_effect
};

Card distant_echo = {
    .name = "Distant\nEcho", .type = SKILL_DEF, .val = 2, .cst = 2, .dmg = 0,
    .defense = 2, .mov = 0, .rng = 0, .link = false, .effect = distant_echo_effect
};

Card divine_summon = {
    .name = "Divine\nSummon", .type = SKILL_DEF, .val = 3, .cst = 4, .dmg = 0,
    .defense = 3, .mov = 0, .rng = 0, .link = false, .effect = divine_summon_effect
};

Card blood_moonlight = {
    .name = "Blood\nMoonlight", .type = TWIST, .val = 1, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = blood_moonlight_effect
};

// 移動卡
Card focused_introspection = {
    .name = "Focused\nIntrospection", .type = SKILL_MOV, .val = 1, .cst = 0, .dmg = 1,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = focused_introspection_effect
};

Card enlightened_resolve = {
    .name = "Enlightened\nResolve", .type = SKILL_MOV, .val = 2, .cst = 2, .dmg = 2,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = enlightened_resolve_effect
};

Card painful_purification = {
    .name = "Painful\nPurification", .type = SKILL_MOV, .val = 3, .cst = 4, .dmg = 3,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = painful_purification_effect
};

Card spiritual_instinct = {
    .name = "Spiritual\nInstinct", .type = TWIST, .val = 1, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = spiritual_instinct_effect
};

// 共同蛻變卡
Card moonlight_meditation = {
    .name = "Moonlight\nMeditation", .type = TWIST, .val = 2, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = moonlight_meditation_passive_effect
};

// 必殺卡
Card blazing_bamboo_sword = {
    .name = "Blazing\nBamboo Sword", .type = EPIC, .val = 0, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = blazing_bamboo_sword_effect
};

Card destined_judgment = {
    .name = "Destined\nJudgment", .type = EPIC, .val = 0, .cst = 0, .dmg = 0,
    .defense = 6, .mov = 0, .rng = 0, .link = false, .effect = destined_judgment_effect
};

Card restless_bloodlust = {
    .name = "Restless\nBloodlust", .type = EPIC, .val = 0, .cst = 0, .dmg = 3,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = restless_bloodlust_effect
};

// ===============================
// 角色初始化
// ===============================

Fable kaguya_fable;

void init_kaguya_fable(void) {
    kaguya_fable = (Fable){
        .name = "Kaguya-hime",
        .Piece = {255, 192, 203, 255},  // 粉紅色代表輝夜姬
        .health = 32,
        .energy = 0,
        .defense = 0,
        .epic_threshold = 16,
        .lane = 1  // 預設位置
    };
    
    // 攻擊技能牌庫
    kaguya_fable.skill[0] = (Deck){
        .cards = { &enlightened_glow, &enlightened_glory, &enlightened_glory, 
                   &enlightened_avatar, &enlightened_avatar },
        .cnt = 5
    };
    
    // 防禦技能牌庫
    kaguya_fable.skill[1] = (Deck){
        .cards = { &confused_echo, &distant_echo, &distant_echo,
                   &divine_summon, &divine_summon },
        .cnt = 5
    };
    
    // 移動技能牌庫
    kaguya_fable.skill[2] = (Deck){
        .cards = { &focused_introspection, &enlightened_resolve, &enlightened_resolve,
                   &painful_purification, &painful_purification },
        .cnt = 5
    };
    
    // 必殺技
    kaguya_fable.epic[0] = blazing_bamboo_sword;
    kaguya_fable.epic[1] = destined_judgment;
    kaguya_fable.epic[2] = restless_bloodlust;
    
    printf("🌸 輝夜姬已初始化完成！\n");
}

void init_kaguya_state(Player *p) {
    kaguya_state.player = p;
    kaguya_state.defense_limit_bonus = 0;
    kaguya_state.can_use_defense_as_attack = false;
    kaguya_state.moonlight_meditation_count = 0;
    
    printf("🌙 輝夜姬狀態已重置\n");
}