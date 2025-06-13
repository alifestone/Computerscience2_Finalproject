#include "twisted_fables.h"

// ===========================================
// 前向宣告區域
// 這是解決編譯錯誤的關鍵部分
// ===========================================

/*
 * 為什麼需要前向宣告？
 * 
 * 在C語言中，函數必須在使用前被宣告或定義。
 * 如果我們想要在一個函數中調用另一個在其後定義的函數，
 * 就需要先進行前向宣告。
 * 
 * 這就像是在書的目錄中先列出所有章節標題，
 * 讓讀者知道這本書包含哪些內容，即使具體內容在後面。
 */

// 花木蘭特殊機制函數的前向宣告
static int consume_qi_for_bonus(Player *mulan_player, int max_qi, const char *skill_name);
static int calculate_damage(Player *target, int base_damage);

// 花木蘭技能處理函數的前向宣告
static void handle_mulan_attack(Player *attacker, Player *defender, int32_t level);
static void handle_mulan_defense(Player *attacker, Player *defender, int32_t level);
static void handle_mulan_movement(Player *attacker, Player *defender, int32_t level);
static void handle_mulan_twist(Player *attacker, Player *defender, int32_t twist_id);
static void handle_mulan_epic(Player *attacker, Player *defender, int32_t epic_id);

// 花木蘭初始化函數的前向宣告
static void setup_mulan_cards(Player *p);

// ===========================================
// 花木蘭的防禦反應機制
// 這個函數是公開的，因為可能被其他模組調用
// ===========================================

void mulan_defensive_reaction(Player *mulan_player, Player *attacker, int incoming_damage) {
    printf("\n🛡️ === 花木蘭的防禦反應 ===\n");
    printf("受到 %d 點傷害，可以棄掉防禦牌來減傷並獲得氣！\n", incoming_damage);
    
    // 檢查是否有防禦牌可以棄掉
    if(mulan_player->hand.cnt > 0) {
        // 這裡簡化處理，假設玩家選擇棄掉一張等級2的防禦牌
        // 在完整實現中，這裡應該讓玩家選擇要棄掉的牌
        printf("💨 棄掉一張2級防禦牌，減少2點傷害並獲得2點氣！\n");
        
        // 增加花木蘭的氣值
        mulan_player->fable->special.mulan_data.qi += 2;
        
        // 實際減少傷害的邏輯會在戰鬥系統中處理
        printf("⚡ 當前氣值: %d\n", mulan_player->fable->special.mulan_data.qi);
    } else {
        printf("❌ 沒有可用的防禦牌\n");
    }
    printf("======================\n");
}

// ===========================================
// 花木蘭的核心效果函數
// 這是所有花木蘭技能的分發中心
// ===========================================

void mulan_effect(void *self, void *target, Type type, int32_t dmg) {
    Player *attacker = (Player *)self;
    Player *defender = (Player *)target;
    
    // 輸出技能使用資訊
    printf("\n⚔️ === 花木蘭使用技能 ===\n");
    printf("技能類型: ");
    switch(type) {
        case SKILL_ATK: printf("攻擊技能"); break;
        case SKILL_DEF: printf("防禦技能"); break;
        case SKILL_MOV: printf("移動技能"); break;
        case TWIST: printf("蛻變技能"); break;
        case EPIC: printf("必殺技"); break;
        default: printf("未知技能"); break;
    }
    printf(" (等級 %d)\n", dmg);
    printf("當前氣值: %d\n", attacker->fable->special.mulan_data.qi);
    
    // 根據技能類型執行相應的效果
    // 現在這些函數調用不會產生編譯錯誤，因為我們已經有了前向宣告
    switch(type) {
        case SKILL_ATK:
            handle_mulan_attack(attacker, defender, dmg);
            break;
        case SKILL_DEF:
            handle_mulan_defense(attacker, defender, dmg);
            break;
        case SKILL_MOV:
            handle_mulan_movement(attacker, defender, dmg);
            break;
        case TWIST:
            handle_mulan_twist(attacker, defender, dmg);
            break;
        case EPIC:
            handle_mulan_epic(attacker, defender, dmg);
            break;
    }
    
    printf("========================\n");
}

// ===========================================
// 花木蘭技能處理函數的實現
// 這些函數現在可以安全地被上面的代碼調用
// ===========================================

/**
 * 處理花木蘭的攻擊技能
 * 包括：不容小覷、勢不可擋、堅不可摧
 */
static void handle_mulan_attack(Player *attacker, Player *defender, int32_t level) {
    printf("🗡️ 使用攻擊技能等級 %d！\n", level);
    
    // 基礎傷害等於技能等級
    int damage = level;
    
    // 檢查是否可以使用氣值增強攻擊（蛻變效果）
    if(attacker->fable->special.mulan_data.qi > 0) {
        printf("💨 可以消耗氣值來增強攻擊力！\n");
        int qi_boost = consume_qi_for_bonus(attacker, 3, "攻擊增強");
        damage += qi_boost;
        
        if(qi_boost > 0) {
            printf("🔥 攻擊力增強 +%d！總傷害: %d\n", qi_boost, damage);
        }
    }
    
    // 計算實際傷害（考慮護盾）
    int actual_damage = calculate_damage(defender, damage);
    printf("💥 造成 %d 點傷害！\n", actual_damage);
    
    // 花木蘭攻擊技能的特殊效果：移動對手並可能棄牌
    printf("🌪️ 將對手移動到相鄰位置\n");
    move_adjacent_to_target(attacker, defender);
    
    // 如果對手在邊緣，觸發棄牌效果
    if(is_at_edge(defender)) {
        printf("🏁 對手位於戰場邊緣，觸發棄牌效果！\n");
        discard_random_card(defender);
    }
}

/**
 * 處理花木蘭的防禦技能
 * 包括：以靜制動、以柔克剛、以弱勝強
 */
static void handle_mulan_defense(Player *attacker, Player *defender, int32_t level) {
    printf("🛡️ 使用防禦技能等級 %d！\n", level);
    
    // 花木蘭的防禦技能特殊效果：回合結束時可消耗氣來額外抽牌
    printf("🎴 設置回合結束效果：可消耗氣來額外抽牌（最多%d張）\n", level);
    
    // 這裡我們模擬回合結束時的抽牌效果
    if(attacker->fable->special.mulan_data.qi > 0) {
        int qi_to_use = consume_qi_for_bonus(attacker, level, "額外抽牌");
        
        if(qi_to_use > 0) {
            printf("📚 額外抽取 %d 張牌！\n", qi_to_use);
            
            // 模擬抽牌過程
            for(int i = 0; i < qi_to_use; i++) {
                Card *drawn_card = draw_deck(&attacker->draw);
                if(drawn_card) {
                    add_deck(&attacker->hand, drawn_card);
                } else {
                    printf("⚠️ 牌庫不足，只額外抽到 %d 張牌\n", i);
                    break;
                }
            }
        }
    }
}

/**
 * 處理花木蘭的移動技能
 * 包括：永不退縮、毫不留情、絕不饒恕
 */
static void handle_mulan_movement(Player *attacker, Player *defender, int32_t level) {
    printf("🏃 使用移動技能等級 %d！\n", level);
    
    // 移動技能會造成傷害
    int damage = level;
    int actual_damage = calculate_damage(defender, damage);
    printf("💥 造成 %d 點傷害！\n", actual_damage);
    
    // 擊退效果（在這個版本中簡化為0格）
    printf("🌊 擊退對手 0 格\n");
    
    // 移動到對手相鄰位置
    move_adjacent_to_target(attacker, defender);
    
    // 檢查是否可以使用氣值增強移動（蛻變效果）
    if(attacker->fable->special.mulan_data.qi > 0) {
        printf("💨 可以消耗氣值來額外移動！\n");
        int qi_boost = consume_qi_for_bonus(attacker, 3, "額外移動");
        
        if(qi_boost > 0) {
            printf("🏃‍♀️ 向相同方向額外移動 %d 格！\n", qi_boost);
            // 這裡會有實際的移動邏輯
        }
    }
    
    // 邊緣效果檢查
    if(is_at_edge(defender)) {
        printf("🏁 對手位於戰場邊緣，觸發棄牌效果！\n");
        discard_random_card(defender);
    }
}

/**
 * 處理花木蘭的蛻變技能
 * 包括：氣貫全身、主宰命運、長驅直入、暴風前夕
 */
static void handle_mulan_twist(Player *attacker, Player *defender, int32_t twist_id) {
    printf("🔄 發動蛻變技能！\n");
    
    switch(twist_id) {
        case 1: // 氣貫全身
            printf("💨 氣貫全身：攻擊時可花費氣來增加傷害（最多3點）\n");
            printf("🔥 此效果為被動效果，攻擊時自動觸發\n");
            break;
            
        case 2: // 主宰命運
            printf("🎯 主宰命運：抽牌後可棄1張手牌再抽1張\n");
            printf("🎴 此效果為被動效果，回合結束抽牌時觸發\n");
            break;
            
        case 3: // 長驅直入
            printf("🏃‍♀️ 長驅直入：移動時可花費氣來額外移動（最多3格）\n");
            printf("⚡ 此效果為被動效果，使用移動技能時觸發\n");
            break;
            
        case 4: // 暴風前夕
            printf("⛈️ 暴風前夕：獲得1點氣，此效果可累積\n");
            attacker->fable->special.mulan_data.qi += 1;
            printf("⚡ 當前氣值: %d\n", attacker->fable->special.mulan_data.qi);
            break;
    }
}

/**
 * 處理花木蘭的必殺技
 * 包括：氣沖雲霄、直面混沌、雷霆一擊
 */
static void handle_mulan_epic(Player *attacker, Player *defender, int32_t epic_id) {
    printf("💫 發動必殺技！\n");
    
    switch(epic_id) {
        case 1: // 氣沖雲霄
            printf("☁️ 氣沖雲霄：回合結束時額外抽取4張牌\n");
            printf("🎴 模擬額外抽牌效果...\n");
            
            for(int i = 0; i < 4; i++) {
                Card *drawn_card = draw_deck(&attacker->draw);
                if(drawn_card) {
                    add_deck(&attacker->hand, drawn_card);
                } else {
                    printf("⚠️ 牌庫不足，只額外抽到 %d 張牌\n", i);
                    break;
                }
            }
            break;
            
        case 2: // 直面混沌
            printf("🌪️ 直面混沌：移動到對手相鄰位置並獲得3點氣\n");
            move_adjacent_to_target(attacker, defender);
            attacker->fable->special.mulan_data.qi += 3;
            printf("⚡ 當前氣值: %d\n", attacker->fable->special.mulan_data.qi);
            break;
            
        case 3: // 雷霆一擊
            printf("⚡ 雷霆一擊：消耗所有氣來造成等量傷害\n");
            int qi_damage = attacker->fable->special.mulan_data.qi;
            attacker->fable->special.mulan_data.qi = 0;
            
            if(qi_damage > 0) {
                int actual_damage = calculate_damage(defender, qi_damage);
                printf("💥 消耗 %d 點氣，造成 %d 點雷霆傷害！\n", qi_damage, actual_damage);
            } else {
                printf("💨 沒有氣值可以消耗\n");
            }
            break;
    }
}

// ===========================================
// 花木蘭角色設置函數
// 這個函數負責初始化花木蘭的所有數據
// ===========================================

void setup_mulan(Player *p) {
    printf("🌸 正在設置花木蘭角色...\n");
    
    // 創建花木蘭角色資料
    // 注意：這裡使用了統一的 Fable 結構體
    Fable *mulan = malloc(sizeof(Fable));
    if(mulan == NULL) {
        printf("❌ 記憶體分配失敗！\n");
        return;
    }
    
    // 初始化花木蘭的基本屬性
    *mulan = (Fable){
        .health = 34,           // 生命上限
        .energy = 6,            // 起始能量值
        .blk = 0,              // 起始護盾值
        .epic_threshold = 17,   // 必殺技閾值
        .lane = 1,             // 起始位置（中央）
        .special.mulan_data.qi = 0  // 起始氣值為0
    };
    
    // 設置角色名稱
    strcpy(mulan->name, "花木蘭");
    p->fable = mulan;
    
    // 初始化所有牌庫
    memset(&p->draw, 0, sizeof(Deck));
    memset(&p->disc, 0, sizeof(Deck));
    memset(&p->bani, 0, sizeof(Deck));
    memset(&p->hand, 0, sizeof(Deck));
    memset(&p->twist, 0, sizeof(Deck));
    memset(&p->epic, 0, sizeof(Deck));
    
    // 創建花木蘭的技能卡牌
    // 現在這個函數調用不會產生編譯錯誤
    setup_mulan_cards(p);
    
    // 洗牌並抽取起始手牌
    shuffle_deck(&p->draw);
    draw_hand(p);
    
    printf("✅ 花木蘭設置完成！\n");
    printf("❤️ 生命值: %d\n", mulan->health);
    printf("🎯 必殺技閾值: %d\n", mulan->epic_threshold);
    printf("🎴 牌庫: %d 張\n", p->draw.cnt);
    printf("🖐️ 手牌: %d 張\n", p->hand.cnt);
    printf("🔄 蛻變牌: %d 張\n", p->twist.cnt);
    printf("💫 必殺牌: %d 張\n", p->epic.cnt);
    printf("========================\n");
}

// ===========================================
// 輔助函數的實現
// 這些函數支援花木蘭的特殊機制
// ===========================================

/**
 * 花木蘭的氣值消耗機制
 * 這個函數處理花木蘭使用氣值來增強技能的邏輯
 */
static int consume_qi_for_bonus(Player *mulan_player, int max_qi, const char *skill_name) {
    int available_qi = mulan_player->fable->special.mulan_data.qi;
    
    if(available_qi == 0) {
        printf("💨 沒有氣值可以消耗\n");
        return 0;
    }
    
    // 計算實際可以消耗的氣值（不超過最大值和可用值）
    int qi_to_consume = (available_qi > max_qi) ? max_qi : available_qi;
    
    // 消耗氣值
    mulan_player->fable->special.mulan_data.qi -= qi_to_consume;
    
    printf("⚡ %s 消耗 %d 點氣，剩餘氣值: %d\n", 
           skill_name, qi_to_consume, mulan_player->fable->special.mulan_data.qi);
    
    return qi_to_consume;
}

/**
 * 計算實際傷害（考慮護盾）
 * 這個函數處理傷害計算的核心邏輯
 */
static int calculate_damage(Player *target, int base_damage) {
    int blocked = 0;
    
    if(target->fable->blk > 0) {
        blocked = (target->fable->blk >= base_damage) ? base_damage : target->fable->blk;
        target->fable->blk -= blocked;
        printf("🛡️ 護盾阻擋 %d 點傷害，剩餘護盾: %d\n", blocked, target->fable->blk);
    }
    
    int actual_damage = base_damage - blocked;
    if(actual_damage > 0) {
        target->fable->health -= actual_damage;
        printf("💔 %s 受到 %d 點傷害，剩餘生命: %d\n", 
               target->fable->name, actual_damage, target->fable->health);
    }
    
    return actual_damage;
}

/**
 * 設置花木蘭的卡牌
 * 這個函數負責創建所有花木蘭的技能卡牌
 */
static void setup_mulan_cards(Player *p) {
    // 攻擊系列卡牌
    static Card attack_cards[] = {
        // 不容小覷 (Lv1)
        {SKILL_ATK, 0, 1, 0, 0, 1, mulan_effect},
        // 勢不可擋 (Lv2) x2
        {SKILL_ATK, 2, 2, 0, 0, 1, mulan_effect},
        {SKILL_ATK, 2, 2, 0, 0, 1, mulan_effect},
        // 堅不可摧 (Lv3) x3
        {SKILL_ATK, 4, 3, 0, 0, 1, mulan_effect},
        {SKILL_ATK, 4, 3, 0, 0, 1, mulan_effect},
        {SKILL_ATK, 4, 3, 0, 0, 1, mulan_effect}
    };
    
    // 防禦系列卡牌
    static Card defense_cards[] = {
        {SKILL_DEF, 0, 1, 0, 0, 0, mulan_effect},
        {SKILL_DEF, 2, 2, 0, 0, 0, mulan_effect},
        {SKILL_DEF, 2, 2, 0, 0, 0, mulan_effect},
        {SKILL_DEF, 4, 3, 0, 0, 0, mulan_effect},
        {SKILL_DEF, 4, 3, 0, 0, 0, mulan_effect},
        {SKILL_DEF, 4, 3, 0, 0, 0, mulan_effect}
    };
    
    // 移動系列卡牌
    static Card movement_cards[] = {
        {SKILL_MOV, 0, 1, 0, 1, 1, mulan_effect},
        {SKILL_MOV, 2, 2, 0, 1, 1, mulan_effect},
        {SKILL_MOV, 2, 2, 0, 1, 1, mulan_effect},
        {SKILL_MOV, 4, 3, 0, 1, 1, mulan_effect},
        {SKILL_MOV, 4, 3, 0, 1, 1, mulan_effect},
        {SKILL_MOV, 4, 3, 0, 1, 1, mulan_effect}
    };
    
    // 蛻變卡牌
    static Card twist_cards[] = {
        {TWIST, 0, 1, 0, 0, 0, mulan_effect}, // 氣貫全身
        {TWIST, 0, 2, 0, 0, 0, mulan_effect}, // 主宰命運
        {TWIST, 0, 3, 0, 0, 0, mulan_effect}, // 長驅直入
        {TWIST, 0, 4, 0, 0, 0, mulan_effect}, // 暴風前夕 x3
        {TWIST, 0, 4, 0, 0, 0, mulan_effect},
        {TWIST, 0, 4, 0, 0, 0, mulan_effect}
    };
    
    // 必殺技卡牌
    static Card epic_cards[] = {
        {EPIC, 0, 1, 0, 0, 0, mulan_effect}, // 氣沖雲霄
        {EPIC, 0, 2, 0, 0, 0, mulan_effect}, // 直面混沌
        {EPIC, 0, 3, 0, 0, 1, mulan_effect}  // 雷霆一擊
    };
    
    // 將卡牌加入相應的牌庫
    for(int i = 0; i < 6; i++) {
        add_deck(&p->draw, &attack_cards[i]);
        add_deck(&p->draw, &defense_cards[i]);
        add_deck(&p->draw, &movement_cards[i]);
    }
    
    for(int i = 0; i < 6; i++) {
        add_deck(&p->twist, &twist_cards[i]);
    }
    
    for(int i = 0; i < 3; i++) {
        add_deck(&p->epic, &epic_cards[i]);
    }
}