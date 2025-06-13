//-------------------- 輝夜角色實作 --------------------

// 輝夜的卡牌效果實作
void kaguya_effect(void *self, void *target, Type type, int32_t dmg) {
    Player *attacker = (Player *)self;   // 施法者
    Player *defender = (Player *)target; // 目標
    
    // 根據卡牌類型和等級來判斷具體效果
    switch(type) {
        case SKILL_ATK: // 攻擊技能牌
            switch(dmg) {
                case 1: // 領悟的光芒 - 等級1攻擊
                case 2: // 領悟的榮耀 - 等級2攻擊  
                case 3: // 領悟的化身 - 等級3攻擊
                    // 核心效果：當前擁有3點防禦即獲得傷害+1
                    if(attacker->fable->blk >= 3) {
                        // 造成額外1點傷害
                        defender->fable->health -= 1;
                        printf("輝夜的防禦力增強了攻擊！額外造成1點傷害\n");
                    }
                    break;
            }
            break;
            
        case SKILL_DEF: // 防禦技能牌
            switch(dmg) {
                case 1: // 困惑的回聲
                    // 展示牌庫頂部1張牌，如果是防禦牌則加入手牌
                    if(attacker->draw.cnt > 0) {
                        Card *revealed = attacker->draw.cards[attacker->draw.cnt - 1];
                        printf("輝夜展示了牌庫頂部的卡牌\n");
                        
                        if(revealed->type == BASIC_DEF || revealed->type == SKILL_DEF) {
                            // 是防禦牌，加入手牌
                            Card *drawn = draw_deck(&attacker->draw);
                            if(drawn) add_deck(&attacker->hand, drawn);
                            printf("找到了防禦牌，加入手牌\n");
                        } else {
                            // 不是防禦牌，選擇棄掉或放回（這裡簡化為棄掉）
                            draw_deck(&attacker->draw); // 移除這張牌
                            printf("不是防禦牌，將其棄掉\n");
                        }
                    }
                    break;
                    
                case 2: // 久遠的回響
                    // 展示牌庫頂部2張牌
                    printf("輝夜展示牌庫頂部2張牌\n");
                    for(int i = 0; i < 2 && attacker->draw.cnt > 0; i++) {
                        Card *revealed = attacker->draw.cards[attacker->draw.cnt - 1];
                        if(revealed->type == BASIC_DEF || revealed->type == SKILL_DEF) {
                            Card *drawn = draw_deck(&attacker->draw);
                            if(drawn) add_deck(&attacker->hand, drawn);
                            printf("找到防禦牌，加入手牌\n");
                        } else {
                            draw_deck(&attacker->draw);
                            printf("棄掉非防禦牌\n");
                        }
                    }
                    break;
                    
                case 3: // 神性的召換
                    // 展示牌庫頂部3張牌
                    printf("輝夜展示牌庫頂部3張牌\n");
                    for(int i = 0; i < 3 && attacker->draw.cnt > 0; i++) {
                        Card *revealed = attacker->draw.cards[attacker->draw.cnt - 1];
                        if(revealed->type == BASIC_DEF || revealed->type == SKILL_DEF) {
                            Card *drawn = draw_deck(&attacker->draw);
                            if(drawn) add_deck(&attacker->hand, drawn);
                            printf("找到防禦牌，加入手牌\n");
                        } else {
                            draw_deck(&attacker->draw);
                            printf("棄掉非防禦牌\n");
                        }
                    }
                    break;
            }
            break;
            
        case SKILL_MOV: // 移動技能牌
            switch(dmg) {
                case 1: // 專注的自省
                case 2: // 頓悟的決心
                case 3: // 痛徹的淨化
                    // 可以失去1點生命來移除手牌或棄牌堆中的一張牌
                    printf("輝夜可以選擇失去1點生命來淨化卡牌\n");
                    // 這裡需要玩家選擇，簡化實作
                    if(attacker->fable->health > 1) {
                        attacker->fable->health -= 1;
                        // 移除一張手牌（簡化為移除第一張）
                        if(attacker->hand.cnt > 0) {
                            attacker->hand.cnt--;
                            printf("輝夜失去1點生命，移除了一張手牌\n");
                        }
                    }
                    
                    // 等級2和3有持續效果
                    if(dmg >= 2) {
                        printf("輝夜設置了持續效果：下回合開始時的遠程制裁\n");
                        // 持續效果標記（實際遊戲中需要更複雜的狀態管理）
                    }
                    break;
            }
            break;
            
        case TWIST: // 蛻變牌效果
            // 根據具體的蛻變牌來實作不同效果
            // 這裡需要額外的識別機制來區分不同的蛻變牌
            printf("輝夜使用了蛻變牌，防禦能力得到提升\n");
            // 大部分蛻變牌都有防禦上限+1的效果
            break;
            
        case EPIC: // 必殺技
            printf("輝夜使用了必殺技！\n");
            // 必殺技效果根據具體技能實作
            break;
            
        default:
            break;
    }
}

// 輝夜角色設置函數
void setup_kaguya(Player *p) {
    // 攻擊牌定義
    Card attack_lv1 = { // 領悟的光芒
        .type = SKILL_ATK,
        .cst = 0,        // 等級1無消耗
        .dmg = 1,        // 基礎傷害1，等級標識
        .blk = 0,
        .mov = 0,
        .rng = 1,        // 射程1
        .effect = kaguya_effect
    };
    
    Card attack_lv2 = { // 領悟的榮耀
        .type = SKILL_ATK,
        .cst = 2,        // 購買費用2
        .dmg = 2,        // 基礎傷害2，等級標識
        .blk = 0,
        .mov = 0,
        .rng = 1,        // 射程1
        .effect = kaguya_effect
    };
    
    Card attack_lv3 = { // 領悟的化身
        .type = SKILL_ATK,
        .cst = 4,        // 購買費用4
        .dmg = 3,        // 基礎傷害3，等級標識
        .blk = 0,
        .mov = 0,
        .rng = 1,        // 射程1
        .effect = kaguya_effect
    };
    
    // 防禦牌定義
    Card defense_lv1 = { // 困惑的回聲
        .type = SKILL_DEF,
        .cst = 0,
        .dmg = 1,        // 等級標識
        .blk = 1,        // 基礎防禦1
        .mov = 0,
        .rng = 0,
        .effect = kaguya_effect
    };
    
    Card defense_lv2 = { // 久遠的回響
        .type = SKILL_DEF,
        .cst = 2,
        .dmg = 2,        // 等級標識
        .blk = 2,        // 基礎防禦2
        .mov = 0,
        .rng = 0,
        .effect = kaguya_effect
    };
    
    Card defense_lv3 = { // 神性的召換
        .type = SKILL_DEF,
        .cst = 4,
        .dmg = 3,        // 等級標識
        .blk = 3,        // 基礎防禦3
        .mov = 0,
        .rng = 0,
        .effect = kaguya_effect
    };
    
    // 移動牌定義
    Card movement_lv1 = { // 專注的自省
        .type = SKILL_MOV,
        .cst = 0,
        .dmg = 1,        // 基礎傷害1，等級標識
        .blk = 0,
        .mov = 0,        // 射程0的特殊移動
        .rng = 0,
        .effect = kaguya_effect
    };
    
    Card movement_lv2 = { // 頓悟的決心
        .type = SKILL_MOV,
        .cst = 2,
        .dmg = 2,        // 基礎傷害2，等級標識
        .blk = 0,
        .mov = 0,
        .rng = 0,
        .effect = kaguya_effect
    };
    
    Card movement_lv3 = { // 痛徹的淨化
        .type = SKILL_MOV,
        .cst = 4,
        .dmg = 3,        // 基礎傷害3，等級標識
        .blk = 0,
        .mov = 0,
        .rng = 0,
        .effect = kaguya_effect
    };
    
    // 蛻變牌定義（簡化實作，實際需要更詳細的區分）
    Card twist1 = { // 懲戒時刻
        .type = TWIST,
        .cst = 0,
        .dmg = 1,        // 蛻變牌標識
        .blk = 0,
        .mov = 0,
        .rng = 0,
        .effect = kaguya_effect
    };
    
    Card twist2 = { // 血色月光
        .type = TWIST,
        .cst = 0,
        .dmg = 2,        // 蛻變牌標識
        .blk = 0,
        .mov = 0,
        .rng = 0,
        .effect = kaguya_effect
    };
    
    Card twist3 = { // 靈性本能
        .type = TWIST,
        .cst = 0,
        .dmg = 3,        // 蛻變牌標識
        .blk = 0,
        .mov = 0,
        .rng = 0,
        .effect = kaguya_effect
    };
    
    Card twist4 = { // 月下沉思
        .type = TWIST,
        .cst = 0,
        .dmg = 4,        // 蛻變牌標識
        .blk = 0,
        .mov = 0,
        .rng = 0,
        .effect = kaguya_effect
    };
    
    // 必殺牌定義（簡化實作）
    Card epic1 = { // 炙熱的竹刀
        .type = EPIC,
        .cst = 0,
        .dmg = 1,        // 必殺牌標識
        .blk = 0,
        .mov = 0,
        .rng = 0,
        .effect = kaguya_effect
    };
    
    Card epic2 = { // 注定的審判
        .type = EPIC,
        .cst = 0,
        .dmg = 2,        // 必殺牌標識
        .blk = 6,        // 防禦6
        .mov = 0,
        .rng = 0,
        .effect = kaguya_effect
    };
    
    Card epic3 = { // 躁動的血性
        .type = EPIC,
        .cst = 0,
        .dmg = 3,        // 必殺牌標識，傷害3
        .blk = 0,
        .mov = 0,
        .rng = 1,        // 需要將對手移到相鄰位置
        .effect = kaguya_effect
    };
    
    // 創建輝夜角色
    Fable *kaguya = malloc(sizeof(Fable));
    
    *kaguya = (Fable){
        .health = 32,        // 生命上限32
        .energy = 6,         // 起始能量（推測值）
        .blk = 0,           // 起始防禦0
        .epic_threshold = 16, // 必殺閥值16
        .lane = 4           // 起始位置（中央偏後）
    };
    
    strcpy(kaguya->name, "輝夜姬");
    p->fable = kaguya;
    
    // 將卡牌加入牌庫 - 按照輝夜的牌組配置
    // 攻擊牌：等級1x1, 等級2x2, 等級3x3
    add_deck(&p->draw, &attack_lv1);
    add_deck(&p->draw, &attack_lv2);
    add_deck(&p->draw, &attack_lv2);
    add_deck(&p->draw, &attack_lv3);
    add_deck(&p->draw, &attack_lv3);
    add_deck(&p->draw, &attack_lv3);
    
    // 防禦牌：等級1x1, 等級2x2, 等級3x3
    add_deck(&p->draw, &defense_lv1);
    add_deck(&p->draw, &defense_lv2);
    add_deck(&p->draw, &defense_lv2);
    add_deck(&p->draw, &defense_lv3);
    add_deck(&p->draw, &defense_lv3);
    add_deck(&p->draw, &defense_lv3);
    
    // 移動牌：等級1x1, 等級2x2, 等級3x3
    add_deck(&p->draw, &movement_lv1);
    add_deck(&p->draw, &movement_lv2);
    add_deck(&p->draw, &movement_lv2);
    add_deck(&p->draw, &movement_lv3);
    add_deck(&p->draw, &movement_lv3);
    add_deck(&p->draw, &movement_lv3);
    
    // 蛻變牌：各1張
    add_deck(&p->twist, &twist1);
    add_deck(&p->twist, &twist2);
    add_deck(&p->twist, &twist3);
    add_deck(&p->twist, &twist4);
    
    // 必殺牌：各1張
    add_deck(&p->epic, &epic1);
    add_deck(&p->epic, &epic2);
    add_deck(&p->epic, &epic3);
    
    // 洗牌並抽取起始手牌
    shuffle_deck(&p->draw);
    draw_hand(p);
    
    printf("輝夜姬已準備就緒！\n");
    printf("特色：防禦強化型角色，擅長透過高防禦值增強戰鬥力\n");
    printf("生命值：%d，必殺閥值：%d\n", kaguya->health, kaguya->epic_threshold);
}