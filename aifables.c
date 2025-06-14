#include"aifables.h"
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include"cards.h"

//alice
int lane_distance(Player *self, Player *enemy) {
    if (!self || !enemy) return 99;
    if (self->fable->lane != enemy->fable->lane) return 99;
    return abs(self->fable->lane - enemy->fable->lane);

}

bool alice_try_attack(Player *self, Player *enemy) {
    for (int i = 0; i < self->hand.cnt; ++i) {
        Card *card = self->hand.cards[i];
        if ((card->type == BASIC_ATK || card->type == SKILL_ATK) && card->rng >= lane_distance(self, enemy)) {
            if (card->cst <= self->fable->energy) {
                printf("[愛麗絲AI] 使用攻擊：%s\n", card->name);
                card->effect(self, enemy);
                self->fable->energy -= card->cst;
                self->hand.cards[i] = NULL;
                return true;
            }
        }
    }
    return false;
}

bool alice_try_defend(Player *self) {
    if (self->fable->health > 18) return false;
    for (int i = 0; i < self->hand.cnt; ++i) {
        Card *card = self->hand.cards[i];
        if ((card->type == BASIC_DEF || card->type == SKILL_DEF) && card->cst <= self->fable->energy) {
            printf("[愛麗絲AI] 使用防禦：%s\n", card->name);
            card->effect(self, self);
            self->fable->energy -= card->cst;
            self->hand.cards[i] = NULL;
            return true;
        }
    }
    return false;
}

bool alice_try_move(Player *self, Player *enemy) {
    int d = lane_distance(self, enemy);
    if (d <= 1) return false;
    for (int i = 0; i < self->hand.cnt; ++i) {
        Card *card = self->hand.cards[i];
        if ((card->type == BASIC_MOV || card->type == SKILL_MOV) && card->cst <= self->fable->energy) {
            printf("[愛麗絲AI] 使用移動：%s\n", card->name);
            card->effect(self, enemy);
            self->fable->energy -= card->cst;
            self->hand.cards[i] = NULL;
            return true;
        }
    }
    return false;
}

bool alice_try_buy(Player *self, FableShop *shop) {
    for (int i = 0; i < shop->Attack.cnt; ++i) {
        Card *card = shop->Attack.cards[i];
        if (card && card->cst <= self->fable->energy) {
            printf("[愛麗絲AI] 購買攻擊牌：%s\n", card->name);
            add_deck(&self->disc, card);
            self->fable->energy -= card->cst;
            return true;
        }
    }
    return false;
}

void ai_alice_turn(Player *self, Player *enemy, FableShop *shop) {
    printf("\n[愛麗絲 AI] 回合開始\n");
    if (alice_try_attack(self, enemy)) return;
    if (alice_try_defend(self)) return;
    if (alice_try_move(self, enemy)) return;
    alice_try_buy(self, shop);
    printf("[愛麗絲AI] 回合結束\n");
}
//kaguya
void ai_kaguya_turn(Player *self, Player *enemy, FableShop *shop) {
    bool acted = false;
    int dist = abs(self->fable->lane - enemy->fable->lane);

    // 如果我們有至少 3 點防禦，攻擊時會額外加傷害
    bool has_bonus = (self->fable->defense >= 3);

    // 嘗試使用攻擊牌
    for (int i = self->hand.cnt - 1; i >= 0; i--) {
        Card *c = self->hand.cards[i];
        if ((c->type == BASIC_ATK || c->type == SKILL_ATK || c->type == EPIC) &&
            self->fable->energy >= c->cst && c->rng >= dist) {
            printf("[輝夜AI] 使用攻擊牌：%s 對距離 %d 的敵人造成 %d(+%d) 傷害\n", c->name, dist, c->dmg, has_bonus ? 1 : 0);
            self->fable->energy -= c->cst;
            c->effect(self, enemy);
            acted = true;
            break;
        }
    }
    if (acted) return;

    // 嘗試使用防禦牌（偏好防禦值低時）
    if (self->fable->defense <= 3 || self->fable->health < 20) {
        for (int i = self->hand.cnt - 1; i >= 0; i--) {
            Card *c = self->hand.cards[i];
            if ((c->type == BASIC_DEF || c->type == SKILL_DEF) && self->fable->energy >= c->cst) {
                printf("[輝夜AI] 使用防禦牌：%s 增加防禦 %d\n", c->name, c->defense);
                self->fable->energy -= c->cst;
                self->fable->defense += c->defense;
                acted = true;
                break;
            }
        }
    }
    if (acted) return;

    // 嘗試移動靠近敵人（避免站太遠）
    if (dist > 1) {
        for (int i = self->hand.cnt - 1; i >= 0; i--) {
            Card *c = self->hand.cards[i];
            if ((c->type == BASIC_MOV || c->type == SKILL_MOV) && self->fable->energy >= c->cst) {
                printf("[輝夜AI] 移動靠近：%s 前進 %d 格\n", c->name, c->mov);
                self->fable->energy -= c->cst;
                if (self->fable->lane < enemy->fable->lane)
                    self->fable->lane += c->mov;
                else
                    self->fable->lane -= c->mov;
                acted = true;
                break;
            }
        }
    }
    if (acted) return;

    // 嘗試購買防禦牌（輝夜走防禦路線）
    if (shop->Defense.cnt > 0 && shop->Defense.cards[0]->cst <= self->fable->energy) {
        printf("[輝夜AI] 購買防禦牌：%s\n", shop->Defense.cards[0]->name);
        self->fable->energy -= shop->Defense.cards[0]->cst;
        add_deck(&self->disc, shop->Defense.cards[0]);
        for (int i = 0; i < shop->Defense.cnt - 1; i++)
            shop->Defense.cards[i] = shop->Defense.cards[i + 1];
        shop->Defense.cnt--;
        acted = true;
    }

    if (!acted) {
        printf("[輝夜AI] 無動作可執行，結束回合\n");
    }
}
//match girl
// int lane_distance(Player *self, Player *enemy) {
//     if (!self || !enemy) return 99;
//     if (self->fable->lane != enemy->fable->lane) return 99;
//     return abs(self->position - enemy->position);
// }

// 嘗試攻擊：如果手上有攻擊牌，且射程夠，就用
bool mg_try_attack(Player *self, Player *enemy) {
    for (int i = 0; i < self->hand.cnt; ++i) {
        Card *card = self->hand.cards[i];
        if ((card->type == BASIC_ATK || card->type == SKILL_ATK) && card->rng >= lane_distance(self, enemy)) {
            if (card->cst <= self->fable->energy) {
                printf("[火柴AI] 使用攻擊：%s\n", card->name);
                card->effect(self, enemy);
                self->fable->energy -= card->cst;
                self->hand.cards[i] = NULL;
                return true;
            }
        }
    }
    return false;
}

// 嘗試防禦：血量低就擋一下
bool mg_try_defend(Player *self) {
    if (self->fable->health > 20) return false;
    for (int i = 0; i < self->hand.cnt; ++i) {
        Card *card = self->hand.cards[i];
        if ((card->type == BASIC_DEF || card->type == SKILL_DEF) && card->cst <= self->fable->energy) {
            printf("[火柴AI] 使用防禦：%s\n", card->name);
            card->effect(self, self);
            self->fable->energy -= card->cst;
            self->hand.cards[i] = NULL;
            return true;
        }
    }
    return false;
}

// 嘗試移動接近對手
bool mg_try_move(Player *self, Player *enemy) {
    int d = lane_distance(self, enemy);
    if (d <= 1) return false;
    for (int i = 0; i < self->hand.cnt; ++i) {
        Card *card = self->hand.cards[i];
        if ((card->type == BASIC_MOV || card->type == SKILL_MOV) && card->cst <= self->fable->energy) {
            printf("[火柴AI] 使用移動：%s\n", card->name);
            card->effect(self, enemy);
            self->fable->energy -= card->cst;
            self->hand.cards[i] = NULL;
            return true;
        }
    }
    return false;
}

// 嘗試購買攻擊牌
bool mg_try_buy(Player *self, FableShop *shop) {
    for (int i = 0; i < shop->Attack.cnt; ++i) {
        Card *card = shop->Attack.cards[i];
        if (card && card->cst <= self->fable->energy) {
            printf("[火柴AI] 購買攻擊牌：%s\n", card->name);
            add_deck(&self->disc, card);
            self->fable->energy -= card->cst;
            return true;
        }
    }
    return false;
}

// 火柴女孩專屬 AI 回合控制
void ai_match_girl_turn(Player *self, Player *enemy, FableShop *shop) {
    printf("\n[火柴女孩 AI] 回合開始\n");
    if (mg_try_attack(self, enemy)) return;
    if (mg_try_defend(self)) return;
    if (mg_try_move(self, enemy)) return;
    mg_try_buy(self, shop);
    printf("[火柴AI] 回合結束\n");
}
//mulan
void ai_mulan_turn(Player *self, Player *enemy, FableShop *shop) {
    bool acted = false;
    int dist = abs(self->fable->lane - enemy->fable->lane);

    // 攻擊邏輯：血量健康、有氣時優先攻擊
    for (int i = self->hand.cnt - 1; i >= 0; i--) {
        Card *c = self->hand.cards[i];
        if ((c->type == BASIC_ATK || c->type == SKILL_ATK || c->type == EPIC) &&
            self->fable->energy >= c->cst && c->rng >= dist) {
            printf("[花木蘭AI] 使用攻擊牌：%s 對距離 %d 的敵人造成 %d 傷害\n", c->name, dist, c->dmg);
            c->effect(self, enemy);
            self->fable->energy -= c->cst;
            acted = true;
            break;
        }
    }
    if (acted) return;

    // 防禦邏輯：血量偏低或距離遠時
    if (self->fable->health <= 15 || dist > 2) {
        for (int i = self->hand.cnt - 1; i >= 0; i--) {
            Card *c = self->hand.cards[i];
            if ((c->type == BASIC_DEF || c->type == SKILL_DEF) && self->fable->energy >= c->cst) {
                printf("[花木蘭AI] 使用防禦牌：%s 增加防禦 %d\n", c->name, c->defense);
                self->fable->energy -= c->cst;
                self->fable->defense += c->defense;
                acted = true;
                break;
            }
        }
    }
    if (acted) return;

    // 移動靠近敵人
    if (dist > 1) {
        for (int i = 0; i < self->hand.cnt; i++) {
            Card *c = self->hand.cards[i];
            if ((c->type == BASIC_MOV || c->type == SKILL_MOV) && self->fable->energy >= c->cst) {
                printf("[花木蘭AI] 移動靠近敵人：使用 %s 前進 %d 格\n", c->name, c->mov);
                self->fable->energy -= c->cst;
                if (self->fable->lane < enemy->fable->lane)
                    self->fable->lane += c->mov;
                else
                    self->fable->lane -= c->mov;
                acted = true;
                break;
            }
        }
    }
    if (acted) return;

    // 購買攻擊牌
    if (shop->Attack.cnt > 0 && shop->Attack.cards[0]->cst <= self->fable->energy) {
        printf("[花木蘭AI] 購買攻擊牌：%s\n", shop->Attack.cards[0]->name);
        self->fable->energy -= shop->Attack.cards[0]->cst;
        add_deck(&self->disc, shop->Attack.cards[0]);
        for (int i = 0; i < shop->Attack.cnt - 1; i++)
            shop->Attack.cards[i] = shop->Attack.cards[i + 1];
        shop->Attack.cnt--;
        acted = true;
    }

    if (!acted) {
        printf("[花木蘭AI] 無可執行動作，結束回合\n");
    }
}
//redhood
void ai_redhood_turn(Player *self, Player *enemy, FableShop *shop) {
    bool acted = false;
    int dist = abs(self->fable->lane - enemy->fable->lane);

    // 嘗試使用攻擊牌（從高等級向低搜尋）
    for (int i = self->hand.cnt - 1; i >= 0; i--) {
        Card *c = self->hand.cards[i];
        if ((c->type == BASIC_ATK || c->type == SKILL_ATK || c->type == EPIC) &&
            self->fable->energy >= c->cst && c->rng >= dist) {
            printf("[紅帽AI] 使用攻擊牌：%s 對距離 %d 的敵人造成 %d 傷害\n", c->name, dist, c->dmg);
            c->effect(self, enemy);
            self->fable->energy -= c->cst;
            acted = true;
            break;
        }
    }

    if (acted) return;

    // 嘗試防禦：當血量低於 15 或無法攻擊時再考慮
    if (self->fable->health <= 15 || dist > 2) {
        for (int i = self->hand.cnt - 1; i >= 0; i--) {
            Card *c = self->hand.cards[i];
            if ((c->type == BASIC_DEF || c->type == SKILL_DEF) &&
                self->fable->energy >= c->cst) {
                printf("[紅帽AI] 使用防禦牌：%s 增加防禦 %d\n", c->name, c->defense);
                self->fable->energy -= c->cst;
                self->fable->defense += c->defense;
                acted = true;
                break;
            }
        }
    }

    if (acted) return;

    // 嘗試移動靠近（若敵人距離大於 1）
    if (dist > 1) {
        for (int i = 0; i < self->hand.cnt; i++) {
            Card *c = self->hand.cards[i];
            if ((c->type == BASIC_MOV || c->type == SKILL_MOV) && self->fable->energy >= c->cst) {
                printf("[紅帽AI] 移動靠近敵人：使用 %s 前進 %d 格\n", c->name, c->mov);
                self->fable->energy -= c->cst;
                if (self->fable->lane < enemy->fable->lane)
                    self->fable->lane += c->mov;
                else
                    self->fable->lane -= c->mov;
                acted = true;
                break;
            }
        }
    }

    if (acted) return;

    // 嘗試購買攻擊牌
    if (shop->Attack.cnt > 0 && shop->Attack.cards[0]->cst <= self->fable->energy) {
        printf("[紅帽AI] 購買攻擊牌：%s\n", shop->Attack.cards[0]->name);
        self->fable->energy -= shop->Attack.cards[0]->cst;
        add_deck(&self->disc, shop->Attack.cards[0]);
        // 將牌庫頂卡片移除
        for (int i = 0; i < shop->Attack.cnt - 1; i++)
            shop->Attack.cards[i] = shop->Attack.cards[i + 1];
        shop->Attack.cnt--;
        acted = true;
    }

    if (!acted) {
        printf("[紅帽AI] 沒有可執行動作，結束回合\n");
    }
}
//snow
void ai_snowwhite_turn(Player *self, Player *enemy, FableShop *shop) {
    bool acted = false;
    int dist = abs(self->fable->lane - enemy->fable->lane);

    // 若對方棄牌堆中毒牌較多，偏好使用攻擊（白雪特色）
    int poison_count = 0;
    for (int i = 0; i < enemy->disc.cnt; i++) {
        if (strstr(enemy->disc.cards[i]->name, "中毒")) {
            poison_count++;
        }
    }

    // 嘗試使用攻擊牌
    for (int i = self->hand.cnt - 1; i >= 0; i--) {
        Card *c = self->hand.cards[i];
        if ((c->type == BASIC_ATK || c->type == SKILL_ATK || c->type == EPIC) &&
            self->fable->energy >= c->cst && c->rng >= dist) {
            printf("[白雪AI] 使用攻擊牌：%s (毒牌 %d 張)\n", c->name, poison_count);
            c->effect(self, enemy);
            self->fable->energy -= c->cst;
            acted = true;
            break;
        }
    }

    if (acted) return;

    // 嘗試使用防禦牌：血量偏低或有中毒增益時
    if (self->fable->health <= 18 || poison_count >= 2) {
        for (int i = self->hand.cnt - 1; i >= 0; i--) {
            Card *c = self->hand.cards[i];
            if ((c->type == BASIC_DEF || c->type == SKILL_DEF) && self->fable->energy >= c->cst) {
                printf("[白雪AI] 使用防禦牌：%s\n", c->name);
                self->fable->defense += c->defense;
                self->fable->energy -= c->cst;
                acted = true;
                break;
            }
        }
    }

    if (acted) return;

    // 嘗試移動接近目標（白雪射程短）
    if (dist > 1) {
        for (int i = 0; i < self->hand.cnt; i++) {
            Card *c = self->hand.cards[i];
            if ((c->type == BASIC_MOV || c->type == SKILL_MOV) && self->fable->energy >= c->cst) {
                printf("[白雪AI] 使用移動牌靠近敵人：%s\n", c->name);
                if (self->fable->lane < enemy->fable->lane)
                    self->fable->lane += c->mov;
                else
                    self->fable->lane -= c->mov;
                self->fable->energy -= c->cst;
                acted = true;
                break;
            }
        }
    }

    if (acted) return;

    // 優先購買攻擊牌
    if (shop->Attack.cnt > 0 && shop->Attack.cards[0]->cst <= self->fable->energy) {
        printf("[白雪AI] 購買攻擊牌：%s\n", shop->Attack.cards[0]->name);
        self->fable->energy -= shop->Attack.cards[0]->cst;
        add_deck(&self->disc, shop->Attack.cards[0]);
        for (int i = 0; i < shop->Attack.cnt - 1; i++)
            shop->Attack.cards[i] = shop->Attack.cards[i + 1];
        shop->Attack.cnt--;
        acted = true;
    }

    if (!acted) {
        printf("[白雪AI] 無可執行動作，結束回合\n");
    }
}
