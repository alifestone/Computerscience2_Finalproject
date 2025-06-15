#include "aifables.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "cards.h"

extern Fable alice_fable;
extern Fable mulan;
extern Fable match_girl_enhanced_fable;
extern Fable red_hood;
extern Fable kaguya_fable;
extern Fable snow_white;

// 引用 alice.c 中的卡牌
extern Card opening_game;
extern Card twisting_game;
extern Card controliing_game;
extern Card magic_trick;
extern Card mental_illusion;
extern Card hat_trick;
extern Card strange_agility;
extern Card strange_stealth;
extern Card strange_weird;
extern Card off_with_head;
extern Card start_performance;
extern Card wonderland_descent;
extern Card ware_mad_here;
extern Card endless_party;
extern Card wonderful_day;
extern Card game_control;



// === Alice 專用購買牌庫（依照購買優先順序）===
Card *alice_deck[] = {
    &endless_party,
    &wonderful_day,
    &game_control,
    &off_with_her_head,
    &start_my_show,
    &wonderland_comes,
    &we_are_all_mad,
    &open_game,
    &twist_game,
    &control_game,
    &magic_trick,
    &mental_magic,
    &hat_trick,
    &strange_agility,
    &strange_stealth,
    &strange_strangeness
};
int alice_deck_size = sizeof(alice_deck) / sizeof(Card*);

// === 判斷雙方是否在同一條路線，並回傳距離（不同行回傳 99）===
int lane_distance(Player *self, Player *enemy) {
    if (!self || !enemy) return 99;
    if (self->fable->lane != enemy->fable->lane) return 99;
    return abs(self->fable->lane - enemy->fable->lane);
}

// === Alice 單張出牌邏輯 ===
Card* alice_choose_card(Player *self, Player *enemy) {
    for (int i = 0; i < self->hand.cnt; ++i) {
        Card *card = self->hand.cards[i];
        if (!card || card->cst > self->fable->energy) continue;

        // 攻擊牌：距離夠才能打
        if ((card->type == BASIC_ATK || card->type == SKILL_ATK) &&
            card->rng >= lane_distance(self, enemy)) {
            return card;
        }

        // 防禦牌：當血量偏低時使用
        if ((card->type == BASIC_DEF || card->type == SKILL_DEF) &&
            self->fable->health <= 18) {
            return card;
        }

        // 移動牌：若雙方距離大於 1 才考慮使用
        if ((card->type == BASIC_MOV || card->type == SKILL_MOV) &&
            lane_distance(self, enemy) > 1) {
            return card;
        }
    }
    return NULL;
}

// === Alice 購買邏輯：照 deck 中優先順序購買 ===
bool alice_try_buy(Player *self, FableShop *shop) {
    for (int i = 0; i < alice_deck_size; ++i) {
        Card *want = alice_deck[i];
        Deck *lists[] = { &shop->Attack, &shop->Defense, &shop->Move };
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < lists[j]->cnt; ++k) {
                Card *card = lists[j]->cards[k];
                if (card == want && card->cst <= self->fable->energy) {
                    printf("[愛麗絲AI] 購買卡牌：%s\n", card->name);
                    add_deck(&self->disc, card);
                    self->fable->energy -= card->cst;
                    // 將該牌從商店移除
                    for (int x = k; x < lists[j]->cnt - 1; ++x)
                        lists[j]->cards[x] = lists[j]->cards[x + 1];
                    lists[j]->cnt--;
                    return true;
                }
            }
        }
    }
    return false;
}

// === Alice AI 回合主邏輯：只出一張牌或購買 ===
void ai_alice_turn(Player *self, Player *enemy, FableShop *shop) {
    printf("\n[愛麗絲 AI] 回合開始\n");

    Card *choice = alice_choose_card(self, enemy);
    if (choice) {
        printf("[愛麗絲AI] 出牌：%s\n", choice->name);
        choice->effect(self, enemy);
        self->fable->energy -= choice->cst;

        // 從手牌中移除該牌
        for (int i = 0; i < self->hand.cnt; ++i) {
            if (self->hand.cards[i] == choice) {
                self->hand.cards[i] = NULL;
                break;
            }
        }
        return;
    }

    // 若沒打牌就購買卡牌
    alice_try_buy(self, shop);
    printf("[愛麗絲AI] 回合結束\n");
}

//red hood
extern Card pot_shot_enhanced;
extern Card crack_shot_enhanced;
extern Card overdrive_burn_enhanced;
extern Card sniper_shot_enhanced;
extern Card onboard_cache_enhanced;
extern Card unleashed_firepower_enhanced;
// extern Card transformed_senses;
extern Card unleashed_fury_enhanced;
extern Card energy_shield_enhanced;
extern Card electrified_shield_enhanced;
extern Card omega_shield_enhanced;
extern Card unleashed_ballistics_enhanced;
extern Card hungry_wolf_devour;
extern Card system_intrusion;
extern Card revenge_rain;

Card *redhood_deck[] = {
    &pot_shot_enhanced,
    &crack_shot_enhanced,
    &overdrive_burn_enhanced,
    &sniper_shot_enhanced,
    &onboard_cache_enhanced,
    &unleashed_firepower_enhanced,
    &unleashed_fury_enhanced,
    &energy_shield_enhanced,
    &electrified_shield_enhanced,
    &omega_shield_enhanced,
    &unleashed_ballistics_enhanced,
    &hungry_wolf_devour,
    &system_intrusion,
    &revenge_rain
};
int redhood_deck_size = sizeof(redhood_deck) / sizeof(Card*);
Card* redhood_choose_card(Player *self, Player *enemy) {
    int dist = abs(self->fable->lane - enemy->fable->lane);
    for (int i = 0; i < self->hand.cnt; ++i) {
        Card *card = self->hand.cards[i];
        if (!card || card->cst > self->fable->energy) continue;

        if ((card->type == BASIC_ATK || card->type == SKILL_ATK || card->type == EPIC) &&
            card->rng >= dist) {
            return card;
        }

        if ((card->type == BASIC_DEF || card->type == SKILL_DEF) &&
            self->fable->health <= 15) {
            return card;
        }

        if ((card->type == BASIC_MOV || card->type == SKILL_MOV) &&
            dist > 1) {
            return card;
        }
    }
    return NULL;
}


bool redhood_try_buy(Player *self, FableShop *shop) {
    for (int i = 0; i < redhood_deck_size; ++i) {
        Card *want = redhood_deck[i];
        Deck *lists[] = { &shop->Attack, &shop->Defense, &shop->Move };
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < lists[j]->cnt; ++k) {
                Card *card = lists[j]->cards[k];
                if (card == want && card->cst <= self->fable->energy) {
                    printf("[紅帽AI] 購買卡牌：%s\n", card->name);
                    add_deck(&self->disc, card);
                    self->fable->energy -= card->cst;
                    for (int x = k; x < lists[j]->cnt - 1; ++x)
                        lists[j]->cards[x] = lists[j]->cards[x + 1];
                    lists[j]->cnt--;
                    return true;
                }
            }
        }
    }
    return false;
}

void ai_redhood_turn(Player *self, Player *enemy, FableShop *shop) {
    printf("\n[紅帽 AI] 回合開始\n");

    Card *choice = redhood_choose_card(self, enemy);
    if (choice) {
        printf("[紅帽AI] 出牌：%s\n", choice->name);
        choice->effect(self, enemy);
        self->fable->energy -= choice->cst;
        for (int i = 0; i < self->hand.cnt; ++i) {
            if (self->hand.cards[i] == choice) {
                self->hand.cards[i] = NULL;
                break;
            }
        }
        return;
    }

    redhood_try_buy(self, shop);
    printf("[紅帽AI] 回合結束\n");
}

//mulan
extern Card underestimate;        // 不容小覷 (等級一)
extern Card unstoppable ;         // 勢不可擋 (等級二)
extern Card indestructible;          // 堅不可摧 (等級三)
extern Card calm_control;      // 氣慣全身 (蛻變1)
extern Card soft_overcome_hard;   // 暴風前夕 (蛻變2) - 攻擊版
extern Card weak_defeat_strong;       // 以靜制動 (等級一)
extern Card never_retreat;         // 以柔克剛 (等級二)
extern Card no_mercy;     // 以弱勝強 (等級三)
extern Card never_forgive;       // 主宰命運 (蛻變1)
extern Card energy_throughout;   // 暴風前夕 (蛻變2) - 防禦版
extern Card storm_gathering;        // 永不退縮 (等級一)
extern Card face_chaos;        // 毫不留情 (等級二)
extern Card thunder_strike;          // 絕不饒恕 (等級三)
extern Card soaring_spirit;        // 長驅直入 (蛻變1)
// extern Card baofengqianxi_mov;   // 暴風前夕 (蛻變2) - 移動版
// extern Card qichongyunxiao;      // 氣沖雲霄
// extern Card zhimianhuandun;      // 直面混沌
// extern Card leitingyiji;         // 雷霆一擊

Card *mulan_deck[] = {
    &burongxiaoqu,        // 不容小覷 (等級一)
    &shibukedang,         // 勢不可擋 (等級二)
    &jianbukecu,          // 堅不可摧 (等級三)
    &qiguanquanshen,      // 氣慣全身 (蛻變1)
    &baofengqianxi_atk,   // 暴風前夕 (蛻變2) - 攻擊版
    &yijingzhidong,       // 以靜制動 (等級一)
    &yiroukegang,         // 以柔克剛 (等級二)
    &yiruoshengqiang,     // 以弱勝強 (等級三)
    &zhuzaimingyun,       // 主宰命運 (蛻變1)
    &baofengqianxi_def,   // 暴風前夕 (蛻變2) - 防禦版
    &yongbutuisuo,        // 永不退縮 (等級一)
    &haobuliuqing,        // 毫不留情 (等級二)
    &jueburao恕,          // 絕不饒恕 (等級三)
    &changquzhiru,        // 長驅直入 (蛻變1)
    &baofengqianxi_mov,   // 暴風前夕 (蛻變2) - 移動版
    &qichongyunxiao,      // 氣沖雲霄
    &zhimianhuandun,      // 直面混沌
    &leitingyiji      
};
int mulan_deck_size = sizeof(mulan_deck) / sizeof(Card*);

Card* mulan_choose_card(Player *self, Player *enemy) {
    int dist = abs(self->fable->lane - enemy->fable->lane);
    for (int i = 0; i < self->hand.cnt; ++i) {
        Card *c = self->hand.cards[i];
        if (!c || c->cst > self->fable->energy) continue;

        if ((c->type == BASIC_ATK || c->type == SKILL_ATK || c->type == EPIC) &&
            c->rng >= dist) {
            return c;
        }

        if ((c->type == BASIC_DEF || c->type == SKILL_DEF) &&
            (self->fable->health <= 15 || dist > 2)) {
            return c;
        }

        if ((c->type == BASIC_MOV || c->type == SKILL_MOV) &&
            dist > 1) {
            return c;
        }
    }
    return NULL;
}

bool mulan_try_buy(Player *self, FableShop *shop) {
    for (int i = 0; i < mulan_deck_size; ++i) {
        Card *want = mulan_deck[i];
        Deck *lists[] = { &shop->Attack, &shop->Defense, &shop->Move };
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < lists[j]->cnt; ++k) {
                Card *card = lists[j]->cards[k];
                if (card == want && card->cst <= self->fable->energy) {
                    printf("[花木蘭AI] 購買卡牌：%s\n", card->name);
                    add_deck(&self->disc, card);
                    self->fable->energy -= card->cst;
                    for (int x = k; x < lists[j]->cnt - 1; ++x)
                        lists[j]->cards[x] = lists[j]->cards[x + 1];
                    lists[j]->cnt--;
                    return true;
                }
            }
        }
    }
    return false;
}
void ai_mulan_turn(Player *self, Player *enemy, FableShop *shop) {
    printf("\n[花木蘭 AI] 回合開始\n");

    Card *choice = mulan_choose_card(self, enemy);
    if (choice) {
        printf("[花木蘭AI] 出牌：%s\n", choice->name);
        choice->effect(self, enemy);
        self->fable->energy -= choice->cst;
        for (int i = 0; i < self->hand.cnt; ++i) {
            if (self->hand.cards[i] == choice) {
                self->hand.cards[i] = NULL;
                break;
            }
        }
        return;
    }

    mulan_try_buy(self, shop);
    printf("[花木蘭AI] 回合結束\n");
}

//snow
extern Card poison1;        // 1級中毒牌
extern Card poison2;        // 2級中毒牌  
extern Card poison3;        // 3級中毒牌
extern Deck poison_deck;
extern Card crystal_fragment;//
extern Card crystal_vortex;//
extern Card crystal_storm;//
extern Card crystal_coffin;//
extern Card pure_poison;//
extern Card tainted_blessing;//
extern Card tainted_feast;//
extern Card tainted_revelry;//
// extern Card fallen_calamity;
extern Card broken_fantasy;//
extern Card broken_reality;//
extern Card broken_destiny;//
// extern Card toxic_erosion;
extern Card seven_serpent_rage;//
extern Card mirror_rain;//
extern Card brewing_disaster;//
extern Card corrupted_banquet

Card *snow_deck[] = {
    &poison1,        // 1級中毒牌
    &poison2,        // 2級中毒牌  
    &poison3,        // 3級中毒牌
    &crystal_fragment,
    &crystal_vortex,
    &crystal_storm,
    &crystal_coffin,
    &pure_poison,
    &tainted_blessing,
    &tainted_feast,
    &tainted_carnival,
    &fallen_calamity,
    &broken_fantasy,
    &broken_reality,
    &broken_destiny,
    &toxic_erosion,
    &seven_serpent_rage,
    &mirror_rain,
    &brewing_disaster
};
int snow_deck_size = sizeof(snow_deck) / sizeof(Card*);

Card* snow_choose_card(Player *self, Player *enemy) {
    int dist = abs(self->fable->lane - enemy->fable->lane);
    int poison_count = 0;
    for (int i = 0; i < enemy->disc.cnt; i++) {
        if (enemy->disc.cards[i] && strstr(enemy->disc.cards[i]->name, "毒")) {
            poison_count++;
        }
    }

    for (int i = 0; i < self->hand.cnt; ++i) {
        Card *c = self->hand.cards[i];
        if (!c || c->cst > self->fable->energy) continue;

        if ((c->type == BASIC_ATK || c->type == SKILL_ATK || c->type == EPIC) &&
            c->rng >= dist) {
            return c;
        }

        if ((c->type == BASIC_DEF || c->type == SKILL_DEF) &&
            (self->fable->health <= 18 || poison_count >= 2)) {
            return c;
        }

        if ((c->type == BASIC_MOV || c->type == SKILL_MOV) && dist > 1) {
            return c;
        }
    }
    return NULL;
}

bool snow_try_buy(Player *self, FableShop *shop) {
    for (int i = 0; i < snow_deck_size; ++i) {
        Card *want = snow_deck[i];
        Deck *lists[] = { &shop->Attack, &shop->Defense, &shop->Move };
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < lists[j]->cnt; ++k) {
                Card *card = lists[j]->cards[k];
                if (card == want && card->cst <= self->fable->energy) {
                    printf("[白雪AI] 購買卡牌：%s\n", card->name);
                    add_deck(&self->disc, card);
                    self->fable->energy -= card->cst;
                    for (int x = k; x < lists[j]->cnt - 1; ++x)
                        lists[j]->cards[x] = lists[j]->cards[x + 1];
                    lists[j]->cnt--;
                    return true;
                }
            }
        }
    }
    return false;
}

void ai_snowwhite_turn(Player *self, Player *enemy, FableShop *shop) {
    printf("\n[白雪 AI] 回合開始\n");

    Card *choice = snow_choose_card(self, enemy);
    if (choice) {
        printf("[白雪AI] 出牌：%s\n", choice->name);
        choice->effect(self, enemy);
        self->fable->energy -= choice->cst;
        for (int i = 0; i < self->hand.cnt; ++i) {
            if (self->hand.cards[i] == choice) {
                self->hand.cards[i] = NULL;
                break;
            }
        }
        return;
    }

    snow_try_buy(self, shop);
    printf("[白雪AI] 回合結束\n");
}

//kaguya
extern Card enlightenment_light;       // 
extern Card enlightenment_glory;      //  
extern Card enlightenment_avatar;     // 
extern Card punishment_moment;    // 懲戒時刻 (蛻變1
extern Card confusing_echo;          // 
extern Card ancient_resonance;           // 久遠的回響
extern Card divine_summon;          // 
extern Card blood_moon;        // 
extern Card focused_introspection;  // 
extern Card enlightened_resolve;    // 
extern Card painful_purification;   // 
extern Card spiritual_instinct;     // 靈性本能 (蛻變1)
extern Card moonlight_contemplation;   //
extern Card scorching_bamboo;   // 
extern Card destined_judgment;      // 
extern Card restless_blood;     // 
extern Card master_destiny;

Card *kaguya_deck[] = {
    &enlightened_glow,      // 領悟的光芒
    &enlightened_glory,      // 領悟的榮耀  
    &enlightened_avatar,     // 領悟的化身
    &disciplinary_moment,    // 懲戒時刻 (蛻變1
    &confused_echo,          // 困惑的回聲
    &distant_echo,           // 久遠的回響
    &divine_summon,          // 神性的召換
    &blood_moonlight,        // 血色月光 (蛻變1)
    &focused_introspection,  // 專注的自省
    &enlightened_resolve,    // 頓悟的決心
    &painful_purification,   // 痛徹的淨化
    &spiritual_instinct,     // 靈性本能 (蛻變1)
    &moonlight_meditation,   // 月下沉思 (蛻變2)
    &blazing_bamboo_sword,   // 炙熱的竹刀
    &destined_judgment,      // 注定的審判
    &restless_bloodlust
};
int kaguya_deck_size = sizeof(kaguya_deck) / sizeof(Card*);

Card* kaguya_choose_card(Player *self, Player *enemy) {
    int dist = abs(self->fable->lane - enemy->fable->lane);
    bool has_bonus = (self->fable->defense >= 3);

    for (int i = self->hand.cnt - 1; i >= 0; i--) {
        Card *c = self->hand.cards[i];
        if (!c || c->cst > self->fable->energy) continue;

        if ((c->type == BASIC_ATK || c->type == SKILL_ATK || c->type == EPIC) &&
            c->rng >= dist) {
            return c;
        }

        if ((c->type == BASIC_DEF || c->type == SKILL_DEF) &&
            (self->fable->defense <= 3 || self->fable->health < 20)) {
            return c;
        }

        if ((c->type == BASIC_MOV || c->type == SKILL_MOV) &&
            dist > 1) {
            return c;
        }
    }
    return NULL;
}

bool kaguya_try_buy(Player *self, FableShop *shop) {
    for (int i = 0; i < kaguya_deck_size; ++i) {
        Card *want = kaguya_deck[i];
        Deck *lists[] = { &shop->Attack, &shop->Defense, &shop->Move };
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < lists[j]->cnt; ++k) {
                Card *card = lists[j]->cards[k];
                if (card == want && card->cst <= self->fable->energy) {
                    printf("[輝夜AI] 購買卡牌：%s\n", card->name);
                    add_deck(&self->disc, card);
                    self->fable->energy -= card->cst;
                    for (int x = k; x < lists[j]->cnt - 1; ++x)
                        lists[j]->cards[x] = lists[j]->cards[x + 1];
                    lists[j]->cnt--;
                    return true;
                }
            }
        }
    }
    return false;
}

void ai_kaguya_turn(Player *self, Player *enemy, FableShop *shop) {
    printf("\n[輝夜 AI] 回合開始\n");

    Card *choice = kaguya_choose_card(self, enemy);
    if (choice) {
        printf("[輝夜AI] 出牌：%s\n", choice->name);
        choice->effect(self, enemy);
        self->fable->energy -= choice->cst;
        for (int i = 0; i < self->hand.cnt; ++i) {
            if (self->hand.cards[i] == choice) {
                self->hand.cards[i] = NULL;
                break;
            }
        }
        return;
    }

    kaguya_try_buy(self, shop);
    printf("[輝夜AI] 回合結束\n");
}
// Match Girl
extern Card phantom_wish_enhanced;    // Phantom Wish
extern Card secret_desire_enhanced;   // Secret Desire
extern Card endless_luxury_enhanced;  // Endless Luxury
extern Card devil_sacrifice_enhanced; // Devil's Sacrifice
extern Card devil_bet_enhanced;       // Devil's Bet
extern Card devil_contract_enhanced;  // Devil's Contract
extern Card weightless_soul_enhanced; // Weightless Soul
extern Card indebted_soul_enhanced;   // Indebted Soul
extern Card broken_soul_enhanced;     // Broken Soul
extern Card ritual_of_pain;           // Ritual of Pain
extern Card flame_trick;              // Flame Trick
extern Card torment_of_fate;          // Torment of Fate
extern Card indulgent_desire;         // Indulgent Desire
extern Card devil_gaze;               // Devil Gaze
extern Card desire_trick;             // Desire Trick
extern Card hell_flame_enhanced;      // Hell Flame
extern Card doomfall_enhanced;        // Doomfall
extern Card curse_of_greed_enhanced;  // Curse of Greed

Card *mg_deck[] = {
    &phantom_wish_enhanced,    // Phantom Wish
    &secret_desire_enhanced,   // Secret Desire
    &endless_luxury_enhanced,  // Endless Luxury
    &devil_sacrifice_enhanced, // Devil's Sacrifice
    &devil_bet_enhanced,       // Devil's Bet
    &devil_contract_enhanced,  // Devil's Contract
    &weightless_soul_enhanced, // Weightless Soul
    &indebted_soul_enhanced,   // Indebted Soul
    &broken_soul_enhanced,     // Broken Soul
    &ritual_of_pain,           // Ritual of Pain
    &flame_trick,              // Flame Trick
    &torment_of_fate,          // Torment of Fate
    &indulgent_desire,         // Indulgent Desire
    &devil_gaze,               // Devil Gaze
    &desire_trick,             // Desire Trick
    &hell_flame_enhanced,      // Hell Flame
    &doomfall_enhanced,        // Doomfall
    &curse_of_greed_enhanced
};
int mg_deck_size = sizeof(mg_deck) / sizeof(Card*);


Card* mg_choose_card(Player *self, Player *enemy) {
    int dist = abs(self->fable->lane - enemy->fable->lane);
    for (int i = 0; i < self->hand.cnt; ++i) {
        Card *c = self->hand.cards[i];
        if (!c || c->cst > self->fable->energy) continue;

        if ((c->type == BASIC_ATK || c->type == SKILL_ATK || c->type == EPIC) &&
            c->rng >= dist) {
            return c;
        }

        if ((c->type == BASIC_DEF || c->type == SKILL_DEF) &&
            self->fable->health <= 20) {
            return c;
        }

        if ((c->type == BASIC_MOV || c->type == SKILL_MOV) && dist > 1) {
            return c;
        }
    }
    return NULL;
}

bool mg_try_buy(Player *self, FableShop *shop) {
    for (int i = 0; i < mg_deck_size; ++i) {
        Card *want = mg_deck[i];
        Deck *lists[] = { &shop->Attack, &shop->Defense, &shop->Move };
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < lists[j]->cnt; ++k) {
                Card *card = lists[j]->cards[k];
                if (card == want && card->cst <= self->fable->energy) {
                    printf("[火柴AI] 購買卡牌：%s\n", card->name);
                    add_deck(&self->disc, card);
                    self->fable->energy -= card->cst;
                    for (int x = k; x < lists[j]->cnt - 1; ++x)
                        lists[j]->cards[x] = lists[j]->cards[x + 1];
                    lists[j]->cnt--;
                    return true;
                }
            }
        }
    }
    return false;
}

void ai_match_girl_turn(Player *self, Player *enemy, FableShop *shop) {
    printf("\n[火柴女孩 AI] 回合開始\n");

    Card *choice = mg_choose_card(self, enemy);
    if (choice) {
        printf("[火柴AI] 出牌：%s\n", choice->name);
        choice->effect(self, enemy);
        self->fable->energy -= choice->cst;
        for (int i = 0; i < self->hand.cnt; ++i) {
            if (self->hand.cards[i] == choice) {
                self->hand.cards[i] = NULL;
                break;
            }
        }
        return;
    }

    mg_try_buy(self, shop);
    printf("[火柴AI] 回合結束\n");
}
