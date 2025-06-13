/*
 * 輝夜姬角色實作 - 修正版本
 * 
 * 主要修正：
 * 1. 修正標頭檔引用錯誤
 * 2. 確保與統一架構的兼容性
 * 3. 使用正確的結構體定義
 */

// 修正重點：使用正確的統一標頭檔
#include "twisted_fables.h"

// ===========================================
// 前向宣告區域
// ===========================================

// 輝夜姬技能處理函數的前向宣告
static void handle_kaguya_attack(Player *attacker, Player *defender, int32_t level);
static void handle_kaguya_defense(Player *attacker, Player *defender, int32_t level);
static void handle_kaguya_movement(Player *attacker, Player *defender, int32_t level);
static void handle_kaguya_twist(Player *attacker, Player *defender, int32_t twist_id);
static void handle_kaguya_epic(Player *attacker, Player *defender, int32_t epic_id);

// 輔助函數的前向宣告
static void setup_kaguya_cards(Player *p);
static int calculate_damage(Player *target, int base_damage);

// ===========================================
// 輝夜姬的卡牌效果實作
// ===========================================

void kaguya_effect(void *self, void *target, Type type, int32_t dmg) {
    Player *attacker = (Player *)self;   // 施法者
    Player *defender = (Player *)target; // 目標
    
    printf("\n🌙 === 輝夜姬使用技能 ===\n");
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
    printf("當前防禦值: %d\n", attacker->fable->blk);
    
    // 根據卡牌類型和等級來判斷具體效果
    switch(type) {
        case SKILL_ATK:
            handle_kaguya_attack(attacker, defender, dmg);
            break;
        case SKILL_DEF:
            handle_kaguya_defense(attacker, defender, dmg);
            break;
        case SKILL_MOV:
            handle_kaguya_movement(attacker, defender, dmg);
            break;
        case TWIST:
            handle_kaguya_twist(attacker, defender, dmg);
            break;
        case EPIC:
            handle_kaguya_epic(attacker, defender, dmg);
            break;
    }
    
    printf("========================\n");
}

// ===========================================
// 輝夜姬技能處理函數的實現
// ===========================================

/**
 * 處理輝夜姬的攻擊技能
 * 包括：領悟的光芒、領悟的榮耀、領悟的化身
 */
static void handle_kaguya_attack(Player *attacker, Player *defender, int32_t level) {
    const char *skill_names[] = {"", "領悟的光芒", "領悟的榮耀", "領悟的化身"};
    
    printf("⚔️ 使用攻擊技能：%s\n", skill_names[level]);
    
    // 基礎傷害等於等級
    int base_damage = level;
    
    // 核心效果：當前擁有3點防禦即獲得傷害+1
    if(attacker->fable->blk >= 3) {
        base_damage += 1;
        printf("🛡️ 防禦力增強了攻擊！額外造成1點傷害\n");
        printf("💫 防禦轉攻擊：%d → %d\n", level, base_damage);
    }
    
    // 造成實際傷害
    int actual_damage = calculate_damage(defender, base_damage);
    printf("💥 總傷害：%d 點\n", actual_damage);
}

/**
 * 處理輝夜姬的防禦技能
 * 包括：困惑的回聲、久遠的回響、神性的召換
 */
static void handle_kaguya_defense(Player *attacker, Player *defender, int32_t level) {
    const char *skill_names[] = {"", "困惑的回聲", "久遠的回響", "神性的召換"};
    
    printf("🛡️ 使用防禦技能：%s\n", skill_names[level]);
    
    // 獲得基礎防禦值
    int defense_gained = level;
    attacker->fable->blk += defense_gained;
    printf("🛡️ 獲得 %d 點防禦，總防禦：%d\n", defense_gained, attacker->fable->blk);
    
    // 展示牌庫頂部的牌，如果是防禦牌則加入手牌
    printf("🔍 展示牌庫頂部 %d 張牌\n", level);
    
    for(int i = 0; i < level && attacker->draw.cnt > 0; i++) {
        Card *revealed = attacker->draw.cards[attacker->draw.cnt - 1];
        printf("👁️ 展示了一張卡牌\n");
        
        if(revealed->type == BASIC_DEF || revealed->type == SKILL_DEF) {
            // 是防禦牌，加入手牌
            Card *drawn = draw_deck(&attacker->draw);
            if(drawn) {
                add_deck(&attacker->hand, drawn);
                printf("🎁 找到了防禦牌，加入手牌\n");
            }
        } else {
            // 不是防禦牌，棄掉
            Card *discarded = draw_deck(&attacker->draw);
            if(discarded) {
                add_deck(&attacker->disc, discarded);
                printf("🗑️ 不是防禦牌，將其棄掉\n");
            }
        }
    }
}

/**
 * 處理輝夜姬的移動技能
 * 包括：專注的自省、頓悟的決心、痛徹的淨化
 */
static void handle_kaguya_movement(Player *attacker, Player *defender, int32_t level) {
    const char *skill_names[] = {"", "專注的自省", "頓悟的決心", "痛徹的淨化"};
    
    printf("🧘 使用移動技能：%s\n", skill_names[level]);
    
    // 造成等級相等的傷害
    int damage = level;
    int actual_damage = calculate_damage(defender, damage);
    printf("💥 造成 %d 點傷害\n", actual_damage);
    
    // 可以失去1點生命來移除手牌或棄牌堆中的一張牌
    printf("🤔 可以選擇失去1點生命來淨化卡牌\n");
    
    // 模擬玩家選擇（這裡簡化為有50%機率選擇淨化）
    if(attacker->fable->health > 1 && (rand() % 2 == 0)) {
        attacker->fable->health -= 1;
        printf("💔 輝夜選擇失去1點生命進行淨化\n");
        printf("❤️ 剩餘生命：%d\n", attacker->fable->health);
        
        // 移除一張手牌（簡化為移除第一張）
        if(attacker->hand.cnt > 0) {
            Card *removed = draw_deck(&attacker->hand);
            if(removed) {
                printf("🗑️ 移除了一張手牌\n");
            }
        }
    }
    
    // 等級2和3有持續效果
    if(level >= 2) {
        int continuous_damage = (level == 2) ? 4 : 6;
        printf("⏰ 設置持續效果：下回合開始時的遠程制裁（%d點傷害）\n", continuous_damage);
    }
}

/**
 * 處理輝夜姬的蛻變技能
 * 輝夜的蛻變大多與防禦相關
 */
static void handle_kaguya_twist(Player *attacker, Player *defender, int32_t twist_id) {
    printf("🔄 發動蛻變技能！\n");
    
    switch(twist_id) {
        case 1: // 懲戒時刻
            printf("⚖️ 懲戒時刻：防禦值上限+1\n");
            printf("🔄 每回合限一次：可以將防禦牌當作攻擊牌使用\n");
            printf("🎯 此為被動效果，提升戰術靈活性\n");
            break;
            
        case 2: // 血色月光
            printf("🌙 血色月光：防禦值上限+1\n");
            printf("🔄 清理階段每重置3點防禦即抽取1張牌\n");
            printf("🎯 此為被動效果，提升資源獲取\n");
            break;
            
        case 3: // 靈性本能
            printf("👁️ 靈性本能：防禦值上限+1\n");
            printf("🎯 每個行動回合限一次：如果對手防禦低於你，可以移動對手\n");
            printf("💡 此為主動效果，需要戰術判斷\n");
            break;
            
        case 4: // 月下沉思
            printf("🌜 月下沉思：在回合結束時獲得防禦2\n");
            attacker->fable->blk += 2;
            printf("🛡️ 立即獲得2點防禦，總防禦：%d\n", attacker->fable->blk);
            break;
    }
}

/**
 * 處理輝夜姬的必殺技
 * 輝夜的必殺技展現了她的防禦哲學
 */
static void handle_kaguya_epic(Player *attacker, Player *defender, int32_t epic_id) {
    printf("💫 發動必殺技！\n");
    
    switch(epic_id) {
        case 1: // 炙熱的竹刀
            printf("🔥 炙熱的竹刀：直到下個回合開始時，不會承受傷害也不會失去生命\n");
            printf("🛡️ 輝夜進入無敵狀態！\n");
            break;
            
        case 2: // 注定的審判
            printf("⚖️ 注定的審判：防禦6\n");
            attacker->fable->blk += 6;
            printf("🛡️ 獲得6點防禦，總防禦：%d\n", attacker->fable->blk);
            printf("📅 持續效果：下回合開始階段的審判傷害\n");
            
            // 模擬審判效果
            int defense_difference = attacker->fable->blk - defender->fable->blk;
            if(defense_difference > 0) {
                printf("⚖️ 預計審判傷害：%d 點（防禦差值）\n", defense_difference);
            }
            break;
            
        case 3: // 躁動的血性
            printf("💢 躁動的血性：將對手移到相鄰位置並造成3點傷害\n");
            move_adjacent_to_target(attacker, defender);
            int damage = calculate_damage(defender, 3);
            printf("💥 造成 %d 點傷害\n", damage);
            printf("📅 持續效果：下回合開始階段重複此效果\n");
            break;
    }
}

// ===========================================
// 輝夜姬角色設置函數
// ===========================================

void setup_kaguya(Player *p) {
    printf("🌙 正在設置輝夜姬角色...\n");
    
    // 創建輝夜角色資料
    Fable *kaguya = malloc(sizeof(Fable));
    if(kaguya == NULL) {
        printf("❌ 記憶體分配失敗！\n");
        return;
    }
    
    // 初始化輝夜的基本屬性
    *kaguya = (Fable){
        .health = 32,        // 生命上限32
        .energy = 6,         // 起始能量
        .blk = 0,           // 起始防禦0
        .epic_threshold = 16, // 必殺閥值16
        .lane = 1           // 起始位置（中央）
    };
    
    // 輝夜的防禦特化數據
    kaguya->special.kaguya_data.defense_bonus = 0;
    kaguya->special.kaguya_data.card_draw_bonus = 0;
    
    strcpy(kaguya->name, "輝夜姬");
    p->fable = kaguya;
    
    // 初始化所有牌庫
    memset(&p->draw, 0, sizeof(Deck));
    memset(&p->disc, 0, sizeof(Deck));
    memset(&p->bani, 0, sizeof(Deck));
    memset(&p->hand, 0, sizeof(Deck));
    memset(&p->twist, 0, sizeof(Deck));
    memset(&p->epic, 0, sizeof(Deck));
    
    // 創建輝夜的技能卡牌
    setup_kaguya_cards(p);
    
    // 洗牌並抽取起始手牌
    shuffle_deck(&p->draw);
    draw_hand(p);
    
    printf("✅ 輝夜姬設置完成！\n");
    printf("❤️ 生命值: %d\n", kaguya->health);
    printf("🎯 必殺技閾值: %d\n", kaguya->epic_threshold);
    printf("🛡️ 護盾上限: 6\n");
    printf("🎴 牌庫: %d 張\n", p->draw.cnt);
    printf("🖐️ 手牌: %d 張\n", p->hand.cnt);
    printf("🔄 蛻變牌: %d 張\n", p->twist.cnt);
    printf("💫 必殺牌: %d 張\n", p->epic.cnt);
    printf("💡 特色：防禦強化型角色，擅長透過高防禦值增強戰鬥力\n");
    printf("========================\n");
}

// ===========================================
// 輔助函數實現
// ===========================================

/**
 * 計算實際傷害（考慮護盾）
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
 * 設置輝夜的卡牌
 */
static void setup_kaguya_cards(Player *p) {
    // 攻擊系列卡牌
    static Card attack_cards[] = {
        {SKILL_ATK, 0, 1, 0, 0, 1, kaguya_effect}, // 領悟的光芒
        {SKILL_ATK, 2, 2, 0, 0, 1, kaguya_effect}, // 領悟的榮耀 x2
        {SKILL_ATK, 2, 2, 0, 0, 1, kaguya_effect},
        {SKILL_ATK, 4, 3, 0, 0, 1, kaguya_effect}, // 領悟的化身 x3
        {SKILL_ATK, 4, 3, 0, 0, 1, kaguya_effect},
        {SKILL_ATK, 4, 3, 0, 0, 1, kaguya_effect}
    };
    
    // 防禦系列卡牌
    static Card defense_cards[] = {
        {SKILL_DEF, 0, 1, 1, 0, 0, kaguya_effect}, // 困惑的回聲
        {SKILL_DEF, 2, 2, 2, 0, 0, kaguya_effect}, // 久遠的回響 x2
        {SKILL_DEF, 2, 2, 2, 0, 0, kaguya_effect},
        {SKILL_DEF, 4, 3, 3, 0, 0, kaguya_effect}, // 神性的召換 x3
        {SKILL_DEF, 4, 3, 3, 0, 0, kaguya_effect},
        {SKILL_DEF, 4, 3, 3, 0, 0, kaguya_effect}
    };
    
    // 移動系列卡牌
    static Card movement_cards[] = {
        {SKILL_MOV, 0, 1, 0, 0, 0, kaguya_effect}, // 專注的自省
        {SKILL_MOV, 2, 2, 0, 0, 0, kaguya_effect}, // 頓悟的決心 x2
        {SKILL_MOV, 2, 2, 0, 0, 0, kaguya_effect},
        {SKILL_MOV, 4, 3, 0, 0, 0, kaguya_effect}, // 痛徹的淨化 x3
        {SKILL_MOV, 4, 3, 0, 0, 0, kaguya_effect},
        {SKILL_MOV, 4, 3, 0, 0, 0, kaguya_effect}
    };
    
    // 蛻變卡牌
    static Card twist_cards[] = {
        {TWIST, 0, 1, 0, 0, 0, kaguya_effect}, // 懲戒時刻
        {TWIST, 0, 2, 0, 0, 0, kaguya_effect}, // 血色月光
        {TWIST, 0, 3, 0, 0, 0, kaguya_effect}, // 靈性本能
        {TWIST, 0, 4, 0, 0, 0, kaguya_effect}, // 月下沉思 x3
        {TWIST, 0, 4, 0, 0, 0, kaguya_effect},
        {TWIST, 0, 4, 0, 0, 0, kaguya_effect}
    };
    
    // 必殺技卡牌
    static Card epic_cards[] = {
        {EPIC, 0, 1, 0, 0, 0, kaguya_effect}, // 炙熱的竹刀
        {EPIC, 0, 2, 6, 0, 0, kaguya_effect}, // 注定的審判
        {EPIC, 0, 3, 0, 0, 1, kaguya_effect}  // 躁動的血性
    };
    
    // 將卡牌加入牌庫
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

/*
 * 輝夜姬實作說明：
 * 
 * 1. 標頭檔修正：
 *    - 使用統一的 twisted_fables.h 而不是錯誤的標頭檔
 *    - 確保與整體架構的一致性
 * 
 * 2. 角色特色實現：
 *    - 防禦轉攻擊：高防禦值增強攻擊力
 *    - 牌庫操作：展示並篩選防禦牌
 *    - 生命換效果：失去生命來淨化卡牌
 * 
 * 3. 防禦哲學體現：
 *    - 大部分技能都與防禦相關
 *    - 透過防禦值來觸發特殊效果
 *    - 長期戰略導向的設計
 */