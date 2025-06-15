#pragma once

#include "base.h"
#include "cards.h"

void ai_alice_turn(Player *self, Player *enemy, FableShop *shop);
void ai_kaguya_turn(Player *self, Player *enemy, FableShop *shop);
void ai_match_girl_turn(Player *self, Player *enemy, FableShop *shop);
void ai_mulan_turn(Player *self, Player *enemy, FableShop *shop);
void ai_redhood_turn(Player *self, Player *enemy, FableShop *shop);
void ai_snowwhite_turn(Player *self, Player *enemy, FableShop *shop);
void robot_ai_turn(Player *self, Player *enemy, FableShop *shop);

bool alice_try_buy(Player *self, FableShop *shop);
bool redhood_try_buy(Player *self, FableShop *shop);
bool mulan_try_buy(Player *self, FableShop *shop);
bool snow_try_buy(Player *self, FableShop *shop);
bool kaguya_try_buy(Player *self, FableShop *shop);
bool mg_try_buy(Player *self, FableShop *shop);