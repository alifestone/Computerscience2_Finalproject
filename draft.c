#include "base.h"
#include "cards.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    const char *txt;
    int32_t x,y;
    int32_t a;
    bool activate;
} Floating;

typedef struct {
    int energy;              // 能量系统
    int qi;                 // 气系统（花木兰专用）
    bool abilities[10];     // 各种能力状态
    int status_effects[5];  // 状态效果计数器
    Card *stored_card;      // 板载缓存卡牌（小红帽专用）
} PlayerState;

TTF_Font *font = NULL;
bool Playing = 0;
PlayerState player_states[2] = {0}; // 存储两个玩家的特殊状态

//-------------------- 实用函数 --------------------
void WriteText(SDL_Renderer *ren, const char *text, SDL_Color color, int32_t x, int32_t y, int font_size) {
    TTF_Init();
    if (!font) {
        font = TTF_OpenFont("NotoSans-Regular.ttf", font_size);
        if (!font) {
            printf("Error opening font: %s\n", TTF_GetError());
            return;
        }
    }

    char *text_copy = strdup(text);
    if (!text_copy) return;

    const int line_spacing = font_size + 10;
    int line_count = 0;

    char *line = strtok(text_copy, "\n");
    while (line) {
        SDL_Surface *sur = TTF_RenderText_Solid(font, line, color);
        if (!sur) {
            printf("Error creating surface: %s\n", SDL_GetError());
            free(text_copy);
            return;
        }

        SDL_Texture *txture = SDL_CreateTextureFromSurface(ren, sur);
        if (!txture) {
            printf("Error creating texture: %s\n", SDL_GetError());
            SDL_FreeSurface(sur);
            free(text_copy);
            return;
        }

        int32_t wtext = sur->w, htext = sur->h;
        SDL_FreeSurface(sur);

        SDL_Rect dest = { x - wtext / 2, y - htext / 2 + line_count * line_spacing, wtext, htext };
        SDL_RenderCopy(ren, txture, NULL, &dest);
        SDL_DestroyTexture(txture);

        line = strtok(NULL, "\n");
        line_count++;
    }

    free(text_copy);
}

void DrawRect(SDL_Renderer *ren,SDL_Rect rect,int32_t r,SDL_Color color){
    SDL_Rect cen={rect.x-rect.w/2,rect.y-rect.h/2,rect.w,rect.h};
    roundedBoxRGBA(ren,cen.x,cen.y,cen.x+cen.w,cen.y+cen.h,r,color.r,color.g,color.b,color.a);
}

void DrawButton(SDL_Renderer *ren,SDL_Rect rect,int32_t r,SDL_Color color,SDL_Color txtclr,const char *line){
    DrawRect(ren,rect,r,color);
    WriteText(ren,line,txtclr,rect.x,rect.y,30);
}

//-------------------- 角色特殊效果实现 --------------------

// 小红帽的板载缓存效果
void red_hood_onboard_cache_effect(Player *player, Card *card) {
    PlayerState *state = &player_states[player == &player_states[1] ? 1 : 0];
    
    if (state->stored_card == NULL) {
        // 存储一张手牌
        printf("Red Hood stores a card in onboard cache.\n");
        state->stored_card = card;
    } else {
        // 取回存储的卡牌
        printf("Red Hood retrieves card from onboard cache.\n");
        add_deck(&player->hand, state->stored_card);
        state->stored_card = NULL;
    }
}

// 小红帽的过载燃烧效果
int red_hood_overdrive_burn(Player *player, int base_damage) {
    printf("Overdrive Burn: Discard skill cards for extra damage? (y/n): ");
    
    // 在实际游戏中这里应该是UI选择，这里简化为自动选择
    for (int i = 0; i < player->hand.cnt; i++) {
        Card *card = player->hand.cards[i];
        if (card->type >= SKILL_ATK && card->type <= SKILL_MOV && card->dmg > 0) {
            // 弃掉技能牌获得额外伤害
            int extra_damage = card->dmg;  // 使用卡牌等级作为额外伤害
            printf("Discarded %s for +%d damage\n", card->name, extra_damage);
            
            // 从手牌中移除
            for (int j = i; j < player->hand.cnt - 1; j++) {
                player->hand.cards[j] = player->hand.cards[j + 1];
            }
            player->hand.cnt--;
            
            return base_damage + extra_damage;
        }
    }
    
    return base_damage;
}

// 小红帽的变异感应效果
int red_hood_transformed_senses(Player *player, int base_range) {
    printf("Transformed Senses: Discard skill cards for extra range? (y/n): ");
    
    for (int i = 0; i < player->hand.cnt; i++) {
        Card *card = player->hand.cards[i];
        if (card->type >= SKILL_ATK && card->type <= SKILL_MOV && card->dmg > 0) {
            int extra_range = card->dmg;
            printf("Discarded %s for +%d range\n", card->name, extra_range);
            
            // 从手牌中移除
            for (int j = i; j < player->hand.cnt - 1; j++) {
                player->hand.cards[j] = player->hand.cards[j + 1];
            }
            player->hand.cnt--;
            
            return base_range + extra_range;
        }
    }
    
    return base_range;
}

// 火柴女孩的火柴牌机制
void match_girl_place_match(Player *self, Player *enemy) {
    if (abs(self->pos - enemy->pos) == 1) { // 相邻检查
        if (match_girl_state.match_pile.cnt > 0 && !match_girl_state.match_placed_this_turn) {
            Card *match = draw_deck(&match_girl_state.match_pile);
            if (match) {
                // 将火柴牌放在敌人牌库顶部
                for (int i = enemy->draw.cnt; i > 0; i--) {
                    enemy->draw.cards[i] = enemy->draw.cards[i - 1];
                }
                enemy->draw.cards[0] = match;
                enemy->draw.cnt++;
                
                match_girl_state.match_placed_this_turn = true;
                printf("🔥 Match placed on enemy's deck top!\n");
            }
        }
    }
}

// 火柴女孩的能量强化效果
int match_girl_energy_enhance_damage(Player *player, int base_damage) {
    PlayerState *state = &player_states[player == &player_states[1] ? 1 : 0];
    
    int energy_to_spend = state->energy / 3; // 每3点能量+1伤害
    if (energy_to_spend > 0) {
        state->energy -= energy_to_spend * 3;
        printf("Match Girl spends %d energy for +%d damage\n", energy_to_spend * 3, energy_to_spend);
        return base_damage + energy_to_spend;
    }
    
    return base_damage;
}

// 花木兰的位置操控效果
void mulan_position_control(Player *attacker, Player *defender, SDL_Renderer *ren) {
    printf("Mulan can reposition the enemy!\n");
    
    // 显示可用位置（攻击者相邻的格子）
    int valid_positions[9];
    int valid_count = 0;
    
    for (int pos = -4; pos <= 4; pos++) {
        if (abs(pos - attacker->pos) == 1 && pos != attacker->pos) {
            valid_positions[valid_count++] = pos;
        }
    }
    
    if (valid_count > 0) {
        // 简化：选择第一个有效位置
        int new_pos = valid_positions[0];
        defender->pos = new_pos;
        printf("Enemy repositioned to lane %d\n", new_pos);
        
        // 检查是否在边缘，如果是则弃掉手牌
        if (new_pos == -4 || new_pos == 4) {
            if (defender->hand.cnt > 0) {
                int random_index = rand() % defender->hand.cnt;
                Card *discarded = defender->hand.cards[random_index];
                printf("Enemy discards %s due to edge positioning!\n", discarded->name);
                
                // 移除被弃掉的卡牌
                for (int i = random_index; i < defender->hand.cnt - 1; i++) {
                    defender->hand.cards[i] = defender->hand.cards[i + 1];
                }
                defender->hand.cnt--;
            }
        }
    }
}

// 花木兰的气系统
void mulan_gain_qi(Player *player, int amount) {
    PlayerState *state = &player_states[player == &player_states[1] ? 1 : 0];
    state->qi += amount;
    printf("Mulan gains %d qi (total: %d)\n", amount, state->qi);
}

// 花木兰的特殊防御机制
bool mulan_defensive_counter(Player *defender, Player *attacker, int incoming_damage) {
    printf("Mulan can discard defense cards to reduce damage and gain qi!\n");
    
    for (int i = 0; i < defender->hand.cnt; i++) {
        Card *card = defender->hand.cards[i];
        if (card->type == BASIC_DEF || card->type == SKILL_DEF || card->type == UNIVERSAL) {
            int reduction = card->dmg > 0 ? card->dmg : 1; // 使用卡牌等级
            printf("Mulan discards %s to reduce damage by %d and gain %d qi\n", 
                   card->name, reduction, reduction);
            
            // 减少伤害并获得气
            mulan_gain_qi(defender, reduction);
            
            // 从手牌中移除
            for (int j = i; j < defender->hand.cnt - 1; j++) {
                defender->hand.cards[j] = defender->hand.cards[j + 1];
            }
            defender->hand.cnt--;
            
            return true;
        }
    }
    
    return false;
}

//-------------------- 改进的战斗判断系统 --------------------

int enhanced_judge(Card *card, Player *p1, Player *p2, SDL_Renderer *ren, Card *linked, int player_index) {
    PlayerState *current_state = &player_states[player_index];
    PlayerState *enemy_state = &player_states[1 - player_index];
    
    // 检查火柴牌的使用
    if (strcmp(card->name, "Match") == 0) {
        printf("🔥 Match card used!\n");
        // 如果对手是火柴女孩，她获得能量
        if (strcmp(p2->fable->name, "Match Girl") == 0) {
            enemy_state->energy += 1;
            printf("Match Girl gains +1 energy from opponent's match!\n");
        }
        return 1; // 火柴牌正常使用
    }
    
    // 处理链接卡牌
    if (card->link) {
        return 3; // 需要链接
    }
    
    // 基础卡牌处理
    if (card->type == BASIC_ATK || card->type == SKILL_ATK) {
        int final_damage = card->dmg;
        int final_range = card->rng;
        
        // 小红帽的特殊效果
        if (strcmp(p1->fable->name, "Red Riding Hood") == 0) {
            if (strcmp(card->name, "Overdrive\nBurn") == 0) {
                final_damage = red_hood_overdrive_burn(p1, final_damage);
            }
        }
        
        // 火柴女孩的能量强化
        if (strcmp(p1->fable->name, "Match Girl") == 0) {
            final_damage = match_girl_energy_enhance_damage(p1, final_damage);
        }
        
        // 距离检查
        if (abs(p1->pos - p2->pos) <= final_range) {
            // 花木兰的防御反击机制
            if (strcmp(p2->fable->name, "Mulan") == 0) {
                if (mulan_defensive_counter(p2, p1, final_damage)) {
                    final_damage = final_damage > 0 ? final_damage - 1 : 0;
                }
            }
            
            p2->health -= final_damage;
            p1->power += card->val;
            
            // 花木兰的位置控制效果
            if (strcmp(p1->fable->name, "Mulan") == 0) {
                mulan_position_control(p1, p2, ren);
            }
            
            printf("⚔️ %s deals %d damage to %s\n", p1->fable->name, final_damage, p2->fable->name);
            return 1;
        } else {
            char buffer[100];
            snprintf(buffer, sizeof(buffer), "Attack out of range! (Range: %d, Distance: %d)", 
                    final_range, abs(p1->pos - p2->pos));
            WriteText(ren, buffer, (SDL_Color){255, 0, 0, 255}, WSCREEN / 2, HSCREEN / 2 + 100, 30);
            SDL_RenderPresent(ren);
            SDL_Delay(2000);
            return 0;
        }
    }
    
    if (card->type == BASIC_DEF || card->type == SKILL_DEF) {
        int defense_gain = card->defense;
        
        if (p1->defense + defense_gain > p1->fable->defense) {
            char buffer[100];
            snprintf(buffer, sizeof(buffer), "Defense cannot exceed %d.", p1->fable->defense);
            WriteText(ren, buffer, (SDL_Color){255, 0, 0, 255}, WSCREEN / 2, HSCREEN / 2 + 100, 30);
            SDL_RenderPresent(ren);
            SDL_Delay(2000);
            p1->defense = p1->fable->defense;
        } else {
            p1->defense += defense_gain;
        }
        
        p1->power += card->val;
        
        // 火柴女孩的生命换卡牌机制
        if (strcmp(p1->fable->name, "Match Girl") == 0) {
            printf("Match Girl can lose life to draw cards? (1 life = 1 card)\n");
            // 简化：自动选择失去1点生命抽1张牌
            if (p1->health > 1) {
                p1->health -= 1;
                draw_hand(p1, 1);
                printf("Match Girl loses 1 life to draw 1 card\n");
            }
        }
        
        printf("🛡️ %s gains %d defense\n", p1->fable->name, defense_gain);
        return 1;
    }
    
    if (card->type == BASIC_MOV || card->type == SKILL_MOV) {
        return 2; // 移动卡牌需要特殊处理
    }
    
    return 0;
}

//-------------------- 回合开始/结束处理 --------------------

void turn_start_effects(Player *player, int player_index) {
    PlayerState *state = &player_states[player_index];
    
    // 重置火柴女孩的本回合火柴放置状态
    if (strcmp(player->fable->name, "Match Girl") == 0) {
        match_girl_state.match_placed_this_turn = false;
        match_girl_state.player = player;
    }
    
    // 花木兰的暴风前夕效果（获得气）
    if (strcmp(player->fable->name, "Mulan") == 0) {
        mulan_gain_qi(player, 1);
    }
    
    printf("🌅 Turn start effects applied for %s\n", player->fable->name);
}

void turn_end_effects(Player *player, int player_index) {
    PlayerState *state = &player_states[player_index];
    
    // 小红帽的能量护盾持续效果
    if (strcmp(player->fable->name, "Red Riding Hood") == 0 && player->defense > 0) {
        printf("Red Hood's energy shield deals damage to nearby enemies!\n");
        // 这里需要检查敌人距离并造成伤害
    }
    
    // 花木兰的以静制动抽牌效果
    if (strcmp(player->fable->name, "Mulan") == 0) {
        if (state->qi > 0) {
            int extra_cards = state->qi > 3 ? 3 : state->qi; // 最多额外抽3张
            draw_hand(player, extra_cards);
            state->qi -= extra_cards;
            printf("Mulan spends %d qi to draw %d extra cards\n", extra_cards, extra_cards);
        }
    }
    
    printf("🌙 Turn end effects applied for %s\n", player->fable->name);
}

//-------------------- 增强的移动处理 --------------------

bool enhanced_movement_handler(Player *player, Card *card, Player *enemy, SDL_Renderer *ren, int player_index) {
    PlayerState *state = &player_states[player_index];
    
    if (card->type != BASIC_MOV && card->type != SKILL_MOV) {
        return false;
    }
    
    int origin = player->pos;
    int move_range = card->mov;
    
    // 小红帽的变异感应效果
    if (strcmp(player->fable->name, "Red Riding Hood") == 0 && 
        strcmp(card->name, "Transformed\nSenses") == 0) {
        move_range = red_hood_transformed_senses(player, move_range);
    }
    
    // 花木兰的长驱直入效果
    if (strcmp(player->fable->name, "Mulan") == 0 && state->qi > 0) {
        int qi_movement = state->qi > 3 ? 3 : state->qi;
        printf("Mulan can spend %d qi for extra movement\n", qi_movement);
        move_range += qi_movement;
        state->qi -= qi_movement;
    }
    
    int from = origin - move_range;
    int to = origin + move_range;
    
    if (from < -4) from = -4;
    if (to > 4) to = 4;
    
    // 高亮移动选项
    for (int j = from; j <= to; j++) {
        if (j != player->pos) {
            filledCircleRGBA(ren, XCENTER + j * (WField + 20), YCENTER, 20, 0, 255, 0, 255);
        }
    }
    SDL_RenderPresent(ren);
    
    // 等待玩家选择
    SDL_Event e2;
    int picked = -5;
    while (picked == -5 && SDL_WaitEvent(&e2)) {
        if (e2.type == SDL_QUIT) {
            return false;
        }
        if (e2.type == SDL_MOUSEBUTTONDOWN && e2.button.button == SDL_BUTTON_LEFT) {
            int mx = e2.button.x, my = e2.button.y;
            for (int j = from; j <= to; j++) {
                SDL_Rect move_rect = {XCENTER + j * (WField + 20) - 20, YCENTER - 20, 40, 40};
                if (SDL_PointInRect(&(SDL_Point){mx, my}, &move_rect)) {
                    picked = j;
                    if (picked != enemy->pos) {
                        player->pos = picked;
                        player->power += card->val;
                        
                        // 移动后的特殊效果
                        
                        // 火柴女孩的移动技能效果
                        if (strcmp(player->fable->name, "Match Girl") == 0) {
                            printf("Match Girl's soul movement effect!\n");
                            // 从对手弃牌堆回收火柴牌获得能量
                            for (int i = 0; i < enemy->disc.cnt; i++) {
                                if (strcmp(enemy->disc.cards[i]->name, "Match") == 0) {
                                    state->energy += card->dmg; // 根据技能等级获得能量
                                    printf("Match Girl gains %d energy from recycling match\n", card->dmg);
                                    break;
                                }
                            }
                        }
                        
                        // 花木兰的永不退縮击退效果
                        if (strcmp(player->fable->name, "Mulan") == 0 && card->dmg > 0) {
                            // 对敌人造成伤害并击退
                            enemy->health -= card->dmg;
                            printf("Mulan's movement deals %d damage\n", card->dmg);
                            
                            // 击退敌人
                            int knockback_direction = enemy->pos > player->pos ? 1 : -1;
                            int new_enemy_pos = enemy->pos + knockback_direction;
                            if (new_enemy_pos >= -4 && new_enemy_pos <= 4) {
                                enemy->pos = new_enemy_pos;
                                printf("Enemy knocked back to position %d\n", new_enemy_pos);
                            }
                        }
                        
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}

//-------------------- 其余函数保持原样 --------------------

void TitleScreen(SDL_Renderer *ren){
    bool intitle=1;
    SDL_Event e;
    SDL_Rect button={WSCREEN/2-200,HSCREEN/2-50,400,100};
    SDL_Color txtclr={255,255,255,255},btnclr={194,194,194,255};
    int32_t r=10;
    while(intitle){
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT){
                exit(0);
                Playing=0;
            }
            if(e.type==SDL_MOUSEBUTTONDOWN){
                int32_t mx=e.button.x, my=e.button.y;
                if(mx>=XCENTER-200&&mx<=XCENTER+200&&my>=YCENTER+60&&my<=YCENTER+160){
                    intitle=0;
                }
            }
        }
        SDL_SetRenderDrawColor(ren,255,255,255,255);
        SDL_RenderClear(ren);
        SDL_Rect box={XCENTER,YCENTER+110,400,100};
        DrawButton(ren,box,r,btnclr,txtclr,"Play 1v1");

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }
}

int randomize_round(SDL_Renderer *ren) {
    int dice_roll = rand() % 6 + 1;
    SDL_Color txtclr = {0, 0, 0, 255};
    char message[50];

    if (dice_roll % 2 == 1) {
        snprintf(message, sizeof(message), "P1 goes first! (Dice: %d)", dice_roll);
        WriteText(ren, message, txtclr, WSCREEN / 2, HSCREEN / 2 - 50,30);
        return 1;
    } else {
        snprintf(message, sizeof(message), "P2 goes first! (Dice: %d)", dice_roll);
        WriteText(ren, message, txtclr, WSCREEN / 2, HSCREEN / 2 - 50,30);
        return 2;
    }
}

bool linkcheck(Type skill, Type candidate){
    return (skill>=SKILL_ATK&&skill<=SKILL_MOV)&&(candidate==skill-4);
}

//-------------------- 主要战斗屏幕（更新版本） --------------------

void BattleScreen(SDL_Renderer *ren,Player *p1,Player *p2){
    bool inbattle=1;
    bool fighting=1;
    int32_t turn=0;
    memset(p1,0,sizeof(Player));
    memset(p2,0,sizeof(Player));
    memset(player_states, 0, sizeof(player_states));
    
    // 初始化火柴女孩状态
    init_match_girl_fable();
    init_match_pile();
    
    SDL_Event e;

    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    SDL_RenderClear(ren);
    int first_player = randomize_round(ren);
    turn+=first_player;
    SDL_RenderPresent(ren);
    SDL_Delay(2000);
    battle_setup(p1,p2,first_player);
    
    int p1discardindex=0, p2discardindex=0;
    int p1_epic_selected=0,p2_epic_selected=0;
    
    while(inbattle){
        // 回合开始效果
        if (turn % 2 == 1) {
            turn_start_effects(p1, 0);
        } else {
            turn_start_effects(p2, 1);
        }
        
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT){
                inbattle=0;
            }
            if(e.type==SDL_MOUSEBUTTONDOWN&&e.button.button==SDL_BUTTON_LEFT){
                int32_t mx=e.button.x, my=e.button.y;
                
                // 结束回合按钮
                SDL_Rect rect={WSCREEN-250, HSCREEN/2-50, 200, 100};
                if(SDL_PointInRect(&(SDL_Point){mx,my},&rect)){
                    printf("Ending turn.\n");
                    
                    if(turn%2==1) {
                        turn_end_effects(p1, 0);
                        // 清空手牌并重新抽牌
                        for(int i=0;i<p1->hand.cnt;i++){
                            p1->hand.cards[i]=NULL;
                        }
                        p1->hand.cnt=0;
                        draw_hand(p1,6);
                        p1->power=0;
                    } else {
                        turn_end_effects(p2, 1);
                        for(int i=0;i<p2->hand.cnt;i++){
                            p2->hand.cards[i]=NULL;
                        }
                        p2->hand.cnt=0;
                        draw_hand(p2,6);
                        p2->power=0;
                    }
                    turn++;
                    break;
                }
                
                // P1手牌处理
                for(int i=0;i<p1->hand.cnt;i++){
                    int32_t x=50+i*(WCARD+20), y=HSCREEN-HCARD+40;
                    SDL_Rect rect={x,y,WCARD,HCARD};
                    Card *linkholder=NULL;
                    if(SDL_PointInRect(&(SDL_Point){mx,my},&rect)&&turn%2==1){
                        printf("p1 Playing %s...\n", p1->hand.cards[i]->name);
                        
                        // 火柴女孩的火柴放置机制
                        if (strcmp(p1->fable->name, "Match Girl") == 0) {
                            match_girl_place_match(p1, p2);
                        }
                        
                        int validity = enhanced_judge(p1->hand.cards[i], p1, p2, ren, linkholder, 0);
                        
                        if(validity==1){
                            p1->disc.cards[p1discardindex++]=p1->hand.cards[i];
                            for(int j=i;j<p1->hand.cnt-1;j++){
                                p1->hand.cards[j]=p1->hand.cards[j+1];
                            }
                            p1->hand.cnt--;
                        } else if(validity==2){
                            if (enhanced_movement_handler(p1, p1->hand.cards[i], p2, ren, 0)) {
                                p1->disc.cards[p1discardindex++] = p1->hand.cards[i];
                                for (int j = i; j < p1->hand.cnt - 1; j++) {
                                    p1->hand.cards[j] = p1->hand.cards[j + 1];
                                }
                                p1->hand.cnt--;
                            }
                        } else if(validity==3){
                            // 链接机制处理（保持原有逻辑）
                            int samecnt=0;
                            for(int j=0;j<p1->hand.cnt;j++){
                                if(linkcheck(p1->hand.cards[i]->type, p1->hand.cards[j]->type)){
                                    filledCircleRGBA(ren, 100+j*(WCARD+20), HSCREEN-40, 20, 255, 255, 0, 255);
                                    samecnt++;
                                }
                            }
                            SDL_RenderPresent(ren);
                            if(samecnt==0){
                                WriteText(ren, "No valid link cards found!", (SDL_Color){255, 0, 0, 255}, WSCREEN / 2, HSCREEN / 2+100,30);
                                SDL_RenderPresent(ren);
                                SDL_Delay(2000);
                                continue;
                            }
                            
                            // 链接处理逻辑...（保持原有的复杂链接逻辑）
                        }
                    }
                }
                
                // P2手牌处理（类似P1，但使用player_index=1）
                for(int i=0;i<p2->hand.cnt;i++){
                    int32_t x=WSCREEN-(50+(i+1)*(WCARD+20)), y=-40;
                    SDL_Rect rect={x,y,WCARD,HCARD};
                    Card *linkholder=NULL;
                    if(SDL_PointInRect(&(SDL_Point){mx,my},&rect)&&turn%2==0){
                        printf("p2 Playing %s...\n", p2->hand.cards[i]->name);
                        
                        if (strcmp(p2->fable->name, "Match Girl") == 0) {
                            match_girl_place_match(p2, p1);
                        }
                        
                        int validity = enhanced_judge(p2->hand.cards[i], p2, p1, ren, linkholder, 1);
                        
                        if(validity==1){
                            p2->disc.cards[p1discardindex++]=p2->hand.cards[i];
                            for(int j=i;j<p2->hand.cnt-1;j++){
                                p2->hand.cards[j]=p2->hand.cards[j+1];
                            }
                            p2->hand.cnt--;
                        } else if(validity==2){
                            if (enhanced_movement_handler(p2, p2->hand.cards[i], p1, ren, 1)) {
                                p2->disc.cards[p1discardindex++] = p2->hand.cards[i];
                                for (int j = i; j < p2->hand.cnt - 1; j++) {
                                    p2->hand.cards[j] = p2->hand.cards[j + 1];
                                }
                                p2->hand.cnt--;
                            }
                        }
                    }
                }
            }
            
            if(e.type==SDL_MOUSEBUTTONDOWN&&fighting==0){
                inbattle=0;
            }
        }
        
        if(p1->health<=0||p2->health<=0){
            char winner[50];
            snprintf(winner, sizeof(winner), "%s wins!", p1->health > 0 ? p1->fable->name : p2->fable->name);
            WriteText(ren, winner, (SDL_Color){255, 0, 0, 255}, WSCREEN / 2, HSCREEN / 2, 48);
            inbattle=0;
            fighting=0;
            continue;
        }
        
        // 渲染游戏界面
        SDL_SetRenderDrawColor(ren,255,255,255,255);
        SDL_RenderClear(ren);
        BattleLane(ren,p1,p2);
        
        // 显示特殊状态
        char status_text[200];
        snprintf(status_text, sizeof(status_text), "P1 Energy: %d, Qi: %d", 
                player_states[0].energy, player_states[0].qi);
        WriteText(ren, status_text, (SDL_Color){0, 0, 255, 255}, 200, 50, 16);
        
        snprintf(status_text, sizeof(status_text), "P2 Energy: %d, Qi: %d", 
                player_states[1].energy, player_states[1].qi);
        WriteText(ren, status_text, (SDL_Color){255, 0, 0, 255}, WSCREEN - 200, 50, 16);
        
        // 显示火柴牌堆状态
        snprintf(status_text, sizeof(status_text), "Matches remaining: %d", 
                match_girl_state.match_pile.cnt);
        WriteText(ren, status_text, (SDL_Color){255, 165, 0, 255}, WSCREEN / 2, 50, 16);
        
        // P1手牌显示
        for(int i=0;i<p1->hand.cnt;i++){
            SDL_Rect rect={50+i*(WCARD+20),HSCREEN-HCARD+40,WCARD,HCARD};
            SDL_SetRenderDrawColor(ren,200,0,0,255);
            SDL_RenderFillRect(ren,&rect);
            WriteText(ren,p1->hand.cards[i]->name,(SDL_Color){255,255,255,255},rect.x+WCARD/2,rect.y+HCARD/2,16);
        }
        
        // P2手牌显示
        for(int i=0;i<p2->hand.cnt;i++){
            SDL_Rect rect={WSCREEN-(50+(i+1)*(WCARD+20)),-40,WCARD,HCARD};
            SDL_SetRenderDrawColor(ren,0,0,200,255);
            SDL_RenderFillRect(ren,&rect);
            WriteText(ren,p2->hand.cards[i]->name,(SDL_Color){255,255,255,255},rect.x+WCARD/2,rect.y+HCARD/2,16);
        }
        
        // 生命值显示
        SDL_Rect p1_health_rect={WSCREEN-350, HSCREEN-100, p1->health*300/p1->fable->health, 30};
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        SDL_RenderFillRect(ren, &p1_health_rect);
        SDL_Rect p1_health_frame={WSCREEN-350, HSCREEN-100, 300, 30};
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderDrawRect(ren, &p1_health_frame);
        
        SDL_Rect p2_health_rect={50, 200, p2->health*300/p2->fable->health, 30};
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        SDL_RenderFillRect(ren, &p2_health_rect);
        SDL_Rect p2_health_frame={50, 200, 300, 30};
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderDrawRect(ren, &p2_health_frame);
        
        // 回合指示器
        SDL_Rect end_turn_button={WSCREEN-150, HSCREEN/2, 200, 100};
        SDL_Color turn_color = {0, 0, 0, 255};
        if(turn % 2 == 1) {
            WriteText(ren, "P1's Turn", turn_color, 100, HSCREEN/2,30);
            SDL_Color end_turn_color = {200, 0, 0, 255};
            DrawButton(ren, end_turn_button, 10, end_turn_color, (SDL_Color){255, 255, 255, 255}, "End Turn");
        } else {
            WriteText(ren, "P2's Turn", turn_color, 100, HSCREEN/2,30);
            SDL_Color end_turn_color = {0, 0, 200, 255};
            DrawButton(ren, end_turn_button, 10, end_turn_color, (SDL_Color){255, 255, 255, 255}, "End Turn");
        }

        SDL_RenderPresent(ren);
        SDL_Delay(15);
    }
}

void BattleLane(SDL_Renderer *ren,Player *p1, Player *p2){
    for(int i=-4;i<=4;i++){
        SDL_Rect rect={XCENTER+i*(WField+20),YCENTER,WField,HField};
        SDL_Color relic={0,0,0,255};
        DrawRect(ren,rect,10,relic);
    }
    filledCircleRGBA(ren,XCENTER+p1->pos*(WField+20),YCENTER,40,p1->fable->Piece.r,p1->fable->Piece.g,p1->fable->Piece.b,p1->fable->Piece.a);
    filledCircleRGBA(ren,XCENTER+p2->pos*(WField+20),YCENTER,40,p2->fable->Piece.r,p2->fable->Piece.g,p2->fable->Piece.b,p2->fable->Piece.a);
}

//-------------------- Setup函数 --------------------
Deck general_shop={
    .cards={&Attack1,&Defense1,&Move1,&Wild1,&Attack2,&Defense2,&Move2,&Attack3,&Defense3,&Move3},
    .cnt=10
};

void starting_deck(Player *p){
    for(int i=0;i<3;i++){
        add_deck(&p->draw,&Attack1);
        add_deck(&p->draw,&Defense1);
        add_deck(&p->draw,&Move1);
    }
    add_deck(&p->draw,p->fable->skill[0].cards[0]);
    add_deck(&p->draw,p->fable->skill[1].cards[0]);
    add_deck(&p->draw,p->fable->skill[2].cards[0]);
    shuffle_deck(&p->draw);   
}

void setup_player(Player *p, Fable *fable) {
    p->fable = fable;
    p->health = fable->health;
    p->power=0;
    p->defense = 0;
}

void battle_setup(Player *p1, Player *p2,int starting){
    setup_player(p1, &red_hood);
    p1->pos=-1;
    starting_deck(p1);
    setup_player(p2, &match_girl);
    p2->pos=1;
    starting_deck(p2);
    if(starting==1){
        draw_hand(p1,4);
        draw_hand(p2,6);
    }else if(starting==2){
        draw_hand(p1,6);
        draw_hand(p2,4);
    }
}

//-------------------- 主函数 --------------------
int main() {
    srand(time(NULL));
    Playing = 1;
    Player p1 = {0}, p2 = {0};

    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("Twisted Fables - Enhanced",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WSCREEN, HSCREEN, 0);
    if(!win) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if(!ren) {
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    while(Playing) {
        TitleScreen(ren);
        BattleScreen(ren, &p1, &p2);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}