#include "aifables.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "cards.h"

extern Fable alice_fable;
extern Fable mulan_fable;
extern Fable match_girl_enhanced_fable;
extern Fable red_hood_fable;
extern Fable kaguya_fable;
extern Fable snow_white_fable;

// 引用 alice.c 中的卡牌
extern Card deal_the_cards;
extern Card cut_the_cards;
extern Card rig_the_cards;

// 防禦卡
extern Card magic_trick;
extern Card mind_trick;
extern Card hat_trick;

// 移動卡
extern Card curiously_agile;
extern Card curiously_covert;
extern Card curiously_curious;

// TWIST 蛻變卡
extern Card off_with_her_head;
extern Card its_showtime;
extern Card wonderland;
extern Card were_all_mad_here;

// EPIC 必殺卡
extern Card important_date;
extern Card frabjous_day;
extern Card the_games_afoot;


// === Alice 專用購買牌庫（依照購買優先順序）===
Card *alice_deck[] = {
    &important_date,
    &frabjous_day,
    &the_games_afoot,
    &off_with_her_head,
    &its_showtime,
    &wonderland,
    &were_all_mad_here,
    &curiously_agile,
    &curiously_covert,
    &deal_the_cards,
    &cut_the_cards,
    &rig_the_cards,
    &curiously_curious,
    &magic_trick,
    &mind_trick,
    &hat_trick

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
extern Card pot_shot;
extern Card crack_shot;
extern Card overdrive_burn;
extern Card sniper_shot;
extern Card onboard_cache;
extern Card unleashed_firepower;
extern Card transformed_senses;
extern Card unleashed_fury;
extern Card energy_shield;
extern Card electrified_shield;
extern Card omega_shield;
extern Card unleashed_ballistics;
extern Card hooded_system;

Card *redhood_deck[] = {
    &hooded_system,
    &unleashed_ballistics,
    &omega_shield,
    &electrified_shield,
    &energy_shield,
    &unleashed_fury,
    &transformed_senses,
    &unleashed_firepower,
    &pot_shot,
    &crack_shot,
    &overdrive_burn,
    &sniper_shot,
    &onboard_cache
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

Card *mulan_deck[] = {
    &legends_call,        // 不容小覷 (等級一)
    &confronting_chaos ,         // 勢不可擋 (等級二)
    &honors_thunder,          // 堅不可摧 (等級三)
    &no_retreat,      // 氣慣全身 (蛻變1)
    &no_quarter,   // 暴風前夕 (蛻變2) - 攻擊版
    &no_mercy,       // 以靜制動 (等級一)
    &enlightened_assault,         // 以柔克剛 (等級二)
    &underestimated,     // 以弱勝強 (等級三)
    &undeniable,       // 主宰命運 (蛻變1)
    &undiminished,   // 暴風前夕 (蛻變2) - 防禦版
    &channeled_qi,        // 永不退縮 (等級一)
    &gathering_storm,        // 毫不留情 (等級二)
    &diverted_fire,          // 絕不饒恕 (等級三)
    &diverted_might,
    &diverted_wrath,
    &evading_fate     
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
// extern Card poison1;        // 1級中毒牌
// extern Card poison2;        // 2級中毒牌  
// extern Card poison3;        // 3級中毒牌

/**
 * 中毒牌庫
 * 存放所有可用的中毒牌，是白雪公主機制的核心資源
 */
extern Deck poison_deck;

// 攻擊技能卡聲明區域
extern Card shard_storm;
extern Card shard_tempest;
extern Card shard_vortex;
// 攻擊系統蛻變牌
extern Card crystal_coffin;
extern Card poisoned_purity;

// 防禦技能卡聲明區域
extern Card tainted_offering;
extern Card tainted_feast;
extern Card tainted_maelstrom;
// 防禦系統蛻變牌
extern Card corrupted_destiny;

// 移動技能卡聲明區域

extern Card shattered_passage;
extern Card shattered_truth;
extern Card shattered_destiny;

// 移動系統蛻變牌
extern Card venomous_turn;

// 必殺技聲明區域
extern Card seven_serpents;
extern Card rain_of_mirrors;
extern Card brewing_destruction;

Card *snow_deck[] = {
    // &poison1,        // 1級中毒牌
    // &poison2,        // 2級中毒牌  
    // &poison3,        // 3級中毒牌
    &brewing_destruction,
    &rain_of_mirrors,
    &seven_serpents,
    &venomous_turn,
    &shattered_passage,
    &shattered_destiny,
    &shattered_truth,
    &corrupted_destiny,
    &shard_storm,
    &shard_tempest,
    &shard_vortex,
    &crystal_coffin,
    &poisoned_purity,
    &tainted_feast,
    &tainted_maelstrom,
    &tainted_offering
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
extern Card ascendant_light;       // 領悟的光芒
extern Card ascendant_glory;      // 領悟的榮耀  
extern Card ascendant_avatar;     // 領悟的化身
extern Card swift_reprisal;    // 懲戒時刻 (蛻變1)

// 防禦卡
extern Card addled_echoes;          // 困惑的回聲
extern Card bygone_echoes;           // 久遠的回響
extern Card divine_echoes;          // 神性的召換
extern Card blood_moon;        // 血色月光 (蛻變1)

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


Card *kaguya_deck[] = {
    &impulsive_onslaught,       // 
    &judgement_arising,      //  
    &blazing_shinai,     // 
    &reflective_repose,    // 懲戒時刻 (蛻變1
    &focused_intent,          // 
    &focused_resolve,           // 久遠的回響
    &focused_ascent,          // 
    &uncanny_instinct,        // 
    &ascendant_light,  // 
    &ascendant_glory,    // 
    &ascendant_avatar,   // 
    &swift_reprisal,   //
    &addled_echoes,   // 
    &bygone_echoes,      // 
    &divine_echoes,     // 
    &blood_moon
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
extern Card make_a_wish;    // Phantom Wish
extern Card regret_a_wish;   // Secret Desire
extern Card requite_a_wish;  // Endless Luxury
extern Card diabolical_offer; // Devil's Sacrifice
extern Card diabolical_gamble;       // Devil's Bet
extern Card diabolical_deal;  // Devil's Contract
extern Card your_soul_weighed; // Weightless Soul
extern Card your_soul_wanting;   // Indebted Soul
extern Card your_soul_ravaged;     // Broken Soul
extern Card ritual_of_pain;           // Ritual of Pain
extern Card inflamed_craving;              // Flame Trick
extern Card after_all_ive_given;          // Torment of Fate
extern Card playing_with_fire;         // Indulgent Desire
extern Card inferno_overload;      // Hell Flame
extern Card the_demons_due;        // Doomfall
extern Card a_surfeit_of_wishes;  // Curse of Greed

Card *mg_deck[] = {
    &a_surfeit_of_wishes,    // Phantom Wish
    &the_demons_due,   // Secret Desire
    &inferno_overload,  // Endless Luxury
    &playing_with_fire, // Devil's Sacrifice
    &after_all_ive_given,       // Devil's Bet
    &inflamed_craving,  // Devil's Contract
    &ritual_of_pain, // Weightless Soul
    &your_soul_ravaged,   // Indebted Soul
    &your_soul_wanting,     // Broken Soul
    &your_soul_weighed,
    &make_a_wish,      // Hell Flame
    &regret_a_wish,        // Doomfall
    &requite_a_wish,
    &diabolical_deal,
    &diabolical_gamble,
    &diabolical_offer
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
