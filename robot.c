#include "robot.h"

// 計算玩家距離
static inline int get_distance(Player *a, Player *b) {
    return abs(a->fable->lane - b->fable->lane);
}

// 嘗試出牌並執行效果
bool try_play_card(Player *self, Card *card, Player *enemy) {
    if (self->power < card->cst) return false;
    if (card->type == BASIC_ATK && get_distance(self, enemy) > card->rng) return false;

    self->power -= card->cst;
    add_deck(&self->bani, card);

    switch (card->type) {
        case BASIC_ATK:
            if (enemy->defense >= card->dmg)
                enemy->defense -= card->dmg;
            else {
                int remaining = card->dmg - enemy->defense;
                enemy->defense = 0;
                enemy->health -= remaining;
            }
            break;
        case BASIC_DEF:
            self->defense += card->defense;
            break;
        case BASIC_MOV:
            if (card->mov != 0) {
                if (enemy->fable->lane > self->fable->lane)
                    self->fable->lane += card->mov;
                else if (enemy->fable->lane < self->fable->lane)
                    self->fable->lane -= card->mov;
            }
            break;
        default:
            break;
    }

    return true;
}

// 優先順序策略選牌
Card *choose_card(Player *self, Player *enemy) {
    int dist = get_distance(self, enemy);
    int hp = self->health;

    // 優先擊殺機會
    for (int i = 0; i < self->hand.cnt; ++i) {
        Card *c = self->hand.cards[i];
        if (c && c->type == BASIC_ATK && self->power >= c->cst && dist <= c->rng) {
            int potential = c->dmg - enemy->defense;
            if (enemy->health <= potential) return c;
        }
    }

    // 若自己快死則優先防禦
    if (hp < 8) {
        for (int i = 0; i < self->hand.cnt; ++i) {
            Card *c = self->hand.cards[i];
            if (c && c->type == BASIC_DEF && self->power >= c->cst)
                return c;
        }
    }

    // 若敵人太遠則優先移動靠近
    if (dist > 0) {
        for (int i = 0; i < self->hand.cnt; ++i) {
            Card *c = self->hand.cards[i];
            if (c && c->type == BASIC_MOV && self->power >= c->cst)
                return c;
        }
    }

    // 其次優先攻擊
    for (int i = 0; i < self->hand.cnt; ++i) {
        Card *c = self->hand.cards[i];
        if (c && c->type == BASIC_ATK && self->power >= c->cst && dist <= c->rng)
            return c;
    }

    // 其次再選防禦
    for (int i = 0; i < self->hand.cnt; ++i) {
        Card *c = self->hand.cards[i];
        if (c && c->type == BASIC_DEF && self->power >= c->cst)
            return c;
    }

    // 若沒有任何能打的牌，則移動或結束
    for (int i = 0; i < self->hand.cnt; ++i) {
        Card *c = self->hand.cards[i];
        if (c && self->power >= c->cst)
            return c;
    }

    return NULL;
}

// 機器人 AI 執行回合
void robot_turn(Player *self, Player *enemy) {
    while (true) {
        Card *choice = choose_card(self, enemy);
        if (!choice) break;

        // 找到該卡在手牌中的 index 並移除
        bool played = false;
        for (int i = 0; i < self->hand.cnt; ++i) {
            if (self->hand.cards[i] == choice) {
                if (try_play_card(self, choice, enemy)) {
                    for (int j = i; j < self->hand.cnt - 1; ++j)
                        self->hand.cards[j] = self->hand.cards[j + 1];
                    self->hand.cards[--self->hand.cnt] = NULL;
                    played = true;
                }
                break;
            }
        }
        if (!played) break;
    }
}
