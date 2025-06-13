#include "twisted_fables.h"

// ===========================================
// 前向宣告區域 - 愛麗絲特有函數
// 這些宣告解決了「函數使用順序」的問題
// ===========================================

/*
 * C語言編譯器的「記憶機制」教學：
 * 
 * 想像編譯器是一個按順序閱讀的學生。當它看到一個函數被調用時，
 * 如果之前沒有見過這個函數的「身份證」（宣告），它就會根據
 * 使用方式做出猜測：「這個函數大概返回int，參數是這些類型」。
 * 
 * 當它後來遇到實際定義時，發現自己的猜測是錯的，就會報錯。
 * 前向宣告就是給編譯器一個正確的「身份證」，避免猜測錯誤。
 */

// 字符串輔助函數的前向宣告
static const char* get_identity_name(AliceIdentity identity);
static const char* get_card_type_name(Type type);
static void display_identity_effects(AliceIdentity identity);

// 卡牌操作函數的前向宣告
static Card* get_basic_card_from_supply(Type card_type, int level);
static bool remove_basic_card_from_player(Player *player, Type card_type);

// 愛麗絲技能處理函數的前向宣告
static void handle_alice_attack(Player *attacker, Player *defender, int32_t level);
static void handle_alice_defense(Player *attacker, Player *defender, int32_t level);
static void handle_alice_movement(Player *attacker, Player *defender, int32_t level);
static void handle_alice_twist(Player *attacker, Player *defender, int32_t twist_id);
static void handle_alice_epic(Player *attacker, Player *defender, int32_t epic_id);

// 初始化函數的前向宣告
static void setup_alice_cards(Player *p);

// 輔助函數的前向宣告
static int calculate_damage(Player *target, int base_damage);

// ===========================================
// 愛麗絲身份系統 - 核心機制
// 這是愛麗絲最獨特的機制
// ===========================================

/**
 * 愛麗絲身份切換函數
 * 
 * 修正重點說明：
 * 原本的 printf 格式錯誤是因為 get_identity_name 函數
 * 被編譯器誤認為返回 int，但我們用 %s 來格式化它。
 * 現在有了正確的前向宣告，編譯器知道它返回 const char*。
 */
void switch_alice_identity(Player *player, AliceIdentity new_identity) {
    AliceIdentity old_identity = player->fable->special.alice_data.current_identity;
    
    // 檢查是否真的需要切換身份
    if(old_identity == new_identity) {
        printf("💭 愛麗絲已經是這個身份了\n");
        return;
    }
    
    // 執行身份切換
    player->fable->special.alice_data.current_identity = new_identity;
    
    // 輸出身份切換的視覺效果
    // 注意：現在 printf 可以正確處理字符串格式，因為有了正確的函數宣告
    printf("\n🎭 === 愛麗絲身份轉換 ===\n");
    printf("從「%s」轉換為「%s」\n", 
           get_identity_name(old_identity), 
           get_identity_name(new_identity));
    
    // 顯示新身份的特殊效果
    display_identity_effects(new_identity);
    
    printf("=======================\n");
}

/**
 * 根據當前身份調整卡牌數值
 * 
 * 這個函數展現了愛麗絲系統的技術核心。
 * 每當使用卡牌時，都要根據當前身份來調整數值。
 */
int32_t get_modified_value(Player *player, Type card_type, int32_t base_value) {
    AliceIdentity identity = player->fable->special.alice_data.current_identity;
    int32_t modified_value = base_value;
    
    switch(identity) {
        case IDENTITY_RED_QUEEN:
            if(card_type == SKILL_ATK) {
                modified_value = base_value + 1;
                printf("👑 紅心皇后加成：攻擊力 %d → %d\n", base_value, modified_value);
            } else if(card_type == SKILL_MOV) {
                modified_value = base_value - 1;
                if(modified_value < 0) modified_value = 0;
                printf("👑 紅心皇后限制：移動力 %d → %d\n", base_value, modified_value);
            }
            break;
            
        case IDENTITY_MAD_HATTER:
            if(card_type == SKILL_DEF) {
                modified_value = base_value + 1;
                printf("🎩 瘋帽子加成：防禦力 %d → %d\n", base_value, modified_value);
            } else if(card_type == SKILL_ATK) {
                modified_value = base_value - 1;
                if(modified_value < 0) modified_value = 0;
                printf("🎩 瘋帽子限制：攻擊力 %d → %d\n", base_value, modified_value);
            }
            break;
            
        case IDENTITY_CHESHIRE_CAT:
            if(card_type == SKILL_MOV) {
                modified_value = base_value + 1;
                printf("😸 柴郡貓加成：移動力 %d → %d\n", base_value, modified_value);
            } else if(card_type == SKILL_DEF) {
                modified_value = base_value - 1;
                if(modified_value < 0) modified_value = 0;
                printf("😸 柴郡貓限制：防禦力 %d → %d\n", base_value, modified_value);
            }
            break;
    }
    
    return modified_value;
}

// ===========================================
// 愛麗絲的核心效果函數
// 這是所有愛麗絲技能的實現中心
// ===========================================

void alice_effect(void *self, void *target, Type type, int32_t dmg) {
    Player *attacker = (Player *)self;
    Player *defender = (Player *)target;
    
    printf("\n🎭 === 愛麗絲使用技能 ===\n");
    
    /*
     * 格式化字符串修正說明：
     * 這裡的 printf 語句之前會產生警告，因為編譯器認為
     * get_identity_name 和 get_card_type_name 返回 int。
     * 現在有了正確的前向宣告，編譯器知道它們返回 const char*，
     * 所以可以安全地使用 %s 格式符。
     */
    printf("當前身份：%s\n", 
           get_identity_name(attacker->fable->special.alice_data.current_identity));
    printf("技能類型：%s (等級 %d)\n", get_card_type_name(type), dmg);
    
    // 根據技能類型分發到對應的處理函數
    // 現在這些函數調用不會產生編譯錯誤，因為我們已經有了前向宣告
    switch(type) {
        case SKILL_ATK:
            handle_alice_attack(attacker, defender, dmg);
            break;
        case SKILL_DEF:
            handle_alice_defense(attacker, defender, dmg);
            break;
        case SKILL_MOV:
            handle_alice_movement(attacker, defender, dmg);
            break;
        case TWIST:
            handle_alice_twist(attacker, defender, dmg);
            break;
        case EPIC:
            handle_alice_epic(attacker, defender, dmg);
            break;
    }
    
    printf("========================\n");
}

// ===========================================
// 愛麗絲技能處理函數的實現
// 這些函數現在可以安全地被上面的代碼調用
// ===========================================

/**
 * 處理愛麗絲的攻擊技能
 * 包括：開啟牌局、扭轉牌局、操控牌局
 */
static void handle_alice_attack(Player *attacker, Player *defender, int32_t level) {
    const char *skill_names[] = {"", "開啟牌局", "扭轉牌局", "操控牌局"};
    
    printf("🃏 使用攻擊技能：%s\n", skill_names[level]);
    
    // 根據等級決定要求對手展示的基本牌數量
    int cards_to_show = level + 2; // 3, 4, 5張
    printf("🔍 要求對手展示 %d 張基本牌\n", cards_to_show);
    
    // 計算基礎傷害（考慮身份加成）
    int base_damage = level;
    int actual_damage = get_modified_value(attacker, SKILL_ATK, base_damage);
    
    // 模擬對手無法展示牌的情況（簡化處理）
    int unable_to_show = rand() % (cards_to_show + 1);
    if(unable_to_show > 0) {
        printf("💥 對手無法展示 %d 張牌，愛麗絲獲得額外傷害\n", unable_to_show);
        actual_damage += unable_to_show;
    }
    
    // 造成傷害
    int damage_dealt = calculate_damage(defender, actual_damage);
    printf("⚡ 總傷害：%d 點\n", damage_dealt);
    
    // 愛麗絲可以從基本牌庫獲得一張牌
    printf("🎁 愛麗絲從基本牌庫獲得一張牌\n");
    Card *gained_card = get_basic_card_from_supply(BASIC_ATK, 1);
    if(gained_card) {
        add_deck(&attacker->disc, gained_card);
        printf("📦 獲得一張基本攻擊牌\n");
    }
    
    // 技能使用後可以切換為紅心皇后
    printf("👑 可以切換身份為紅心皇后\n");
    switch_alice_identity(attacker, IDENTITY_RED_QUEEN);
}

/**
 * 處理愛麗絲的防禦技能
 * 包括：魔力技巧、精神幻術、帽子戲法
 */
static void handle_alice_defense(Player *attacker, Player *defender, int32_t level) {
    const char *skill_names[] = {"", "魔力技巧", "精神幻術", "帽子戲法"};
    
    printf("🎩 使用防禦技能：%s\n", skill_names[level]);
    
    // 計算防禦值（考慮身份加成）
    int base_defense = level;
    int actual_defense = get_modified_value(attacker, SKILL_DEF, base_defense);
    
    // 增加護盾
    attacker->fable->blk += actual_defense;
    printf("🛡️ 獲得 %d 點護盾，總護盾：%d\n", actual_defense, attacker->fable->blk);
    
    // 愛麗絲的防禦技能特效：操作基本牌
    printf("🔄 可以移除1張基本牌，並獲得等級不高於 %d 的基本牌\n", level);
    
    // 模擬移除一張基本攻擊牌
    if(remove_basic_card_from_player(attacker, BASIC_ATK)) {
        // 獲得一張新的基本牌
        Card *new_card = get_basic_card_from_supply(BASIC_DEF, level);
        if(new_card) {
            add_deck(&attacker->disc, new_card);
            printf("🎁 獲得一張等級 %d 的基本防禦牌\n", level);
        }
    }
    
    // 技能使用後可以切換為瘋帽子
    printf("🎩 可以切換身份為瘋帽子\n");
    switch_alice_identity(attacker, IDENTITY_MAD_HATTER);
}

/**
 * 處理愛麗絲的移動技能
 * 包括：詭異的敏捷、詭異的隱蔽、詭異的詭異
 */
static void handle_alice_movement(Player *attacker, Player *defender, int32_t level) {
    const char *skill_names[] = {"", "詭異的敏捷", "詭異的隱蔽", "詭異的詭異"};
    
    printf("😸 使用移動技能：%s\n", skill_names[level]);
    
    // 計算移動距離（考慮身份加成）
    int base_movement = level;
    int actual_movement = get_modified_value(attacker, SKILL_MOV, base_movement);
    
    printf("🏃‍♀️ 向一個方向移動至多 %d 格\n", actual_movement);
    
    // 模擬移動過程中穿過對手的效果
    bool passed_through_opponent = (rand() % 2 == 0);
    if(passed_through_opponent) {
        printf("👻 穿過對手！愛麗絲抽取 %d 張牌\n", level);
        
        // 抽牌效果
        for(int i = 0; i < level; i++) {
            Card *drawn_card = draw_deck(&attacker->draw);
            if(drawn_card) {
                add_deck(&attacker->hand, drawn_card);
            } else {
                printf("⚠️ 牌庫不足，只抽到 %d 張牌\n", i);
                break;
            }
        }
    }
    
    // 技能使用後可以切換為柴郡貓
    printf("😸 可以切換身份為柴郡貓\n");
    switch_alice_identity(attacker, IDENTITY_CHESHIRE_CAT);
}

/**
 * 處理愛麗絲的蛻變技能
 * 包括：身份特定的蛻變和通用蛻變
 */
static void handle_alice_twist(Player *attacker, Player *defender, int32_t twist_id) {
    printf("🔄 發動蛻變技能！\n");
    
    switch(twist_id) {
        case 1: // 砍掉她的頭（紅心皇后蛻變）
            printf("👑 砍掉她的頭：紅心皇后身份時，攻擊技能獲得的牌可加入手牌\n");
            printf("🎯 此為被動效果，使用攻擊技能時觸發\n");
            break;
            
        case 2: // 仙境降臨（瘋帽子蛻變）
            printf("🎩 仙境降臨：瘋帽子身份時，防禦技能獲得的牌可選擇加入手牌\n");
            printf("🎯 此為被動效果，使用防禦技能時觸發\n");
            break;
            
        case 3: // 我們全是瘋子（柴郡貓蛻變）
            printf("😸 我們全是瘋子：柴郡貓身份時，穿越效果增強\n");
            printf("🎯 此為被動效果，移動穿越時觸發\n");
            break;
            
        case 4: // 開始我的表演（通用蛻變）
            printf("🎭 開始我的表演：回合結束時額外抽1張牌\n");
            printf("🎴 此效果可疊加多次\n");
            
            // 立即演示抽牌效果
            Card *bonus_card = draw_deck(&attacker->draw);
            if(bonus_card) {
                add_deck(&attacker->hand, bonus_card);
                printf("🎁 立即額外抽取1張牌\n");
            }
            break;
    }
}

/**
 * 處理愛麗絲的必殺技
 * 包括：無休止的派對、精彩的奇妙日、遊戲盡在掌控
 */
static void handle_alice_epic(Player *attacker, Player *defender, int32_t epic_id) {
    printf("💫 發動必殺技！\n");
    
    switch(epic_id) {
        case 1: // 無休止的派對
            printf("🎉 無休止的派對：抽牌後棄至4張手牌，立即開始新回合\n");
            
            // 模擬抽牌階段
            printf("🎴 模擬抽牌階段...\n");
            for(int i = 0; i < 2; i++) {
                Card *drawn = draw_deck(&attacker->draw);
                if(drawn) add_deck(&attacker->hand, drawn);
            }
            
            // 棄牌至4張
            while(attacker->hand.cnt > 4) {
                Card *discarded = draw_deck(&attacker->hand);
                if(discarded) add_deck(&attacker->disc, discarded);
            }
            printf("🗑️ 棄牌至4張手牌\n");
            printf("🔄 立即開始新回合！\n");
            break;
            
        case 2: // 精彩的奇妙日
            printf("✨ 精彩的奇妙日：所有牌的數值+1\n");
            printf("🎯 此效果持續到本回合結束\n");
            printf("💡 這會影響攻擊力、防禦力和移動力\n");
            break;
            
        case 3: // 遊戲盡在掌控
            printf("🎮 遊戲盡在掌控：射程3，傷害3，將基本牌洗入對手牌庫\n");
            
            // 造成傷害
            int damage = get_modified_value(attacker, SKILL_ATK, 3);
            calculate_damage(defender, damage);
            
            // 模擬將基本牌洗入對手牌庫
            printf("🔀 將至多5張基本牌洗入對手牌庫\n");
            for(int i = 0; i < 5; i++) {
                Card *basic_card = get_basic_card_from_supply(BASIC_ATK, 1);
                if(basic_card) {
                    add_deck(&defender->draw, basic_card);
                }
            }
            shuffle_deck(&defender->draw);
            printf("🎯 對手的牌庫被污染了！\n");
            break;
    }
}

// ===========================================
// 愛麗絲角色設置函數
// 初始化愛麗絲的所有資料和牌組
// ===========================================

void setup_alice(Player *p) {
    printf("🎭 正在設置愛麗絲角色...\n");
    
    // 分配記憶體給愛麗絲
    Fable *alice = malloc(sizeof(Fable));
    if(alice == NULL) {
        printf("❌ 記憶體分配失敗！\n");
        return;
    }
    
    // 初始化愛麗絲的基本屬性
    *alice = (Fable){
        .health = 32,
        .energy = 6,
        .blk = 0,
        .epic_threshold = 16,
        .lane = 1,
        .special.alice_data.current_identity = IDENTITY_RED_QUEEN
    };
    
    strcpy(alice->name, "愛麗絲");
    p->fable = alice;
    
    // 初始化所有牌庫
    memset(&p->draw, 0, sizeof(Deck));
    memset(&p->disc, 0, sizeof(Deck));
    memset(&p->bani, 0, sizeof(Deck));
    memset(&p->hand, 0, sizeof(Deck));
    memset(&p->twist, 0, sizeof(Deck));
    memset(&p->epic, 0, sizeof(Deck));
    
    // 設置愛麗絲的卡牌
    setup_alice_cards(p);
    
    // 洗牌並抽取起始手牌
    shuffle_deck(&p->draw);
    draw_hand(p);
    
    printf("✅ 愛麗絲設置完成！\n");
    printf("❤️ 生命值: %d\n", alice->health);
    printf("🎯 必殺技閾值: %d\n", alice->epic_threshold);
    
    /*
     * 格式化字符串修正示例：
     * 這個 printf 語句之前會產生警告，現在可以正常工作，
     * 因為 get_identity_name 有了正確的函數簽名宣告。
     */
    printf("🎭 起始身份: %s\n", get_identity_name(alice->special.alice_data.current_identity));
    
    printf("🎴 牌庫: %d 張\n", p->draw.cnt);
    printf("🖐️ 手牌: %d 張\n", p->hand.cnt);
    printf("🔄 蛻變牌: %d 張\n", p->twist.cnt);
    printf("💫 必殺牌: %d 張\n", p->epic.cnt);
    printf("========================\n");
}

// ===========================================
// 輔助函數的實現
// 這些函數支援愛麗絲的特殊機制
// ===========================================

/**
 * 模擬基本牌庫操作
 * 
 * 在完整的遊戲中，這會是一個真正的基本牌庫，
 * 包含所有基本攻擊、防禦、移動牌。
 */
static Card* get_basic_card_from_supply(Type card_type, int level) {
    // 這裡使用靜態陣列來模擬基本牌庫
    static Card basic_attack_cards[] = {
        {BASIC_ATK, 1, 1, 0, 0, 1, NULL},
        {BASIC_ATK, 2, 2, 0, 0, 1, NULL},
        {BASIC_ATK, 3, 3, 0, 0, 1, NULL}
    };
    
    static Card basic_defense_cards[] = {
        {BASIC_DEF, 1, 1, 1, 0, 0, NULL},
        {BASIC_DEF, 2, 2, 2, 0, 0, NULL},
        {BASIC_DEF, 3, 3, 3, 0, 0, NULL}
    };
    
    static Card basic_movement_cards[] = {
        {BASIC_MOV, 1, 1, 0, 1, 0, NULL},
        {BASIC_MOV, 2, 2, 0, 2, 0, NULL},
        {BASIC_MOV, 3, 3, 0, 3, 0, NULL}
    };
    
    // 根據類型和等級返回對應的基本牌
    switch(card_type) {
        case BASIC_ATK:
            if(level >= 1 && level <= 3) {
                return &basic_attack_cards[level - 1];
            }
            break;
        case BASIC_DEF:
            if(level >= 1 && level <= 3) {
                return &basic_defense_cards[level - 1];
            }
            break;
        case BASIC_MOV:
            if(level >= 1 && level <= 3) {
                return &basic_movement_cards[level - 1];
            }
            break;
        default:
            break;
    }
    
    return NULL;
}

/**
 * 從手牌或棄牌區移除基本牌
 * 
 * 這個函數展示了複雜的牌庫操作邏輯。
 * 修正重點：現在 get_card_type_name 有正確的返回類型。
 */
static bool remove_basic_card_from_player(Player *player, Type card_type) {
    // 先嘗試從手牌中移除
    for(int i = 0; i < player->hand.cnt; i++) {
        if(player->hand.cards[i]->type == card_type) {
            printf("🗑️ 從手牌移除一張基本%s牌\n", get_card_type_name(card_type));
            
            // 將後面的牌往前移
            for(int j = i; j < player->hand.cnt - 1; j++) {
                player->hand.cards[j] = player->hand.cards[j + 1];
            }
            player->hand.cnt--;
            return true;
        }
    }
    
    // 如果手牌中沒有，嘗試從棄牌區移除
    for(int i = 0; i < player->disc.cnt; i++) {
        if(player->disc.cards[i]->type == card_type) {
            printf("🗑️ 從棄牌區移除一張基本%s牌\n", get_card_type_name(card_type));
            
            // 將後面的牌往前移
            for(int j = i; j < player->disc.cnt - 1; j++) {
                player->disc.cards[j] = player->disc.cards[j + 1];
            }
            player->disc.cnt--;
            return true;
        }
    }
    
    printf("❌ 沒有找到可移除的基本%s牌\n", get_card_type_name(card_type));
    return false;
}

/**
 * 設置愛麗絲的卡牌
 * 愛麗絲的卡牌數量遵循遊戲規則
 * 
 * 函數組織說明：
 * 將這個函數定義在這裡而不是在 setup_alice 之前，
 * 是因為我們有了前向宣告，編譯器知道這個函數的存在。
 */
static void setup_alice_cards(Player *p) {
    // 攻擊系列卡牌
    static Card attack_cards[] = {
        {SKILL_ATK, 0, 1, 0, 0, 1, alice_effect}, // 開啟牌局
        {SKILL_ATK, 2, 2, 0, 0, 2, alice_effect}, // 扭轉牌局 x2
        {SKILL_ATK, 2, 2, 0, 0, 2, alice_effect},
        {SKILL_ATK, 4, 3, 0, 0, 3, alice_effect}, // 操控牌局 x3
        {SKILL_ATK, 4, 3, 0, 0, 3, alice_effect},
        {SKILL_ATK, 4, 3, 0, 0, 3, alice_effect}
    };
    
    // 防禦系列卡牌
    static Card defense_cards[] = {
        {SKILL_DEF, 0, 1, 1, 0, 0, alice_effect}, // 魔力技巧
        {SKILL_DEF, 2, 2, 2, 0, 0, alice_effect}, // 精神幻術 x2
        {SKILL_DEF, 2, 2, 2, 0, 0, alice_effect},
        {SKILL_DEF, 4, 3, 3, 0, 0, alice_effect}, // 帽子戲法 x3
        {SKILL_DEF, 4, 3, 3, 0, 0, alice_effect},
        {SKILL_DEF, 4, 3, 3, 0, 0, alice_effect}
    };
    
    // 移動系列卡牌
    static Card movement_cards[] = {
        {SKILL_MOV, 0, 1, 0, 1, 0, alice_effect}, // 詭異的敏捷
        {SKILL_MOV, 2, 2, 0, 2, 0, alice_effect}, // 詭異的隱蔽 x2
        {SKILL_MOV, 2, 2, 0, 2, 0, alice_effect},
        {SKILL_MOV, 4, 3, 0, 3, 0, alice_effect}, // 詭異的詭異 x2
        {SKILL_MOV, 4, 3, 0, 3, 0, alice_effect}
    };
    
    // 蛻變卡牌
    static Card twist_cards[] = {
        {TWIST, 0, 1, 0, 0, 0, alice_effect}, // 砍掉她的頭
        {TWIST, 0, 2, 0, 0, 0, alice_effect}, // 仙境降臨
        {TWIST, 0, 3, 0, 0, 0, alice_effect}, // 我們全是瘋子
        {TWIST, 0, 4, 0, 0, 0, alice_effect}, // 開始我的表演 x3
        {TWIST, 0, 4, 0, 0, 0, alice_effect},
        {TWIST, 0, 4, 0, 0, 0, alice_effect}
    };
    
    // 必殺技卡牌
    static Card epic_cards[] = {
        {EPIC, 0, 1, 0, 0, 0, alice_effect}, // 無休止的派對
        {EPIC, 0, 2, 0, 0, 0, alice_effect}, // 精彩的奇妙日
        {EPIC, 0, 3, 0, 0, 3, alice_effect}  // 遊戲盡在掌控
    };
    
    // 將卡牌加入牌庫
    for(int i = 0; i < 6; i++) {
        add_deck(&p->draw, &attack_cards[i]);
        add_deck(&p->draw, &defense_cards[i]);
    }
    
    for(int i = 0; i < 5; i++) {
        add_deck(&p->draw, &movement_cards[i]);
    }
    
    for(int i = 0; i < 6; i++) {
        add_deck(&p->twist, &twist_cards[i]);
    }
    
    for(int i = 0; i < 3; i++) {
        add_deck(&p->epic, &epic_cards[i]);
    }
}

/**
 * 獲取身份名稱的字符串表示
 * 
 * 函數簽名修正說明：
 * 這個函數必須返回 const char* 而不是 int，
 * 因為我們在 printf 中使用 %s 格式符來格式化它的返回值。
 */
static const char* get_identity_name(AliceIdentity identity) {
    switch(identity) {
        case IDENTITY_RED_QUEEN: return "紅心皇后";
        case IDENTITY_MAD_HATTER: return "瘋帽子";  
        case IDENTITY_CHESHIRE_CAT: return "柴郡貓";
        default: return "未知身份";
    }
}

/**
 * 顯示身份效果說明
 */
static void display_identity_effects(AliceIdentity identity) {
    switch(identity) {
        case IDENTITY_RED_QUEEN:
            printf("👑 紅心皇后效果：攻擊力+1, 移動力-1\n");
            printf("💭 適合攻擊型戰術\n");
            break;
            
        case IDENTITY_MAD_HATTER:
            printf("🎩 瘋帽子效果：防禦力+1, 攻擊力-1\n");
            printf("💭 適合防守型戰術\n");
            break;
            
        case IDENTITY_CHESHIRE_CAT:
            printf("😸 柴郡貓效果：移動力+1, 防禦力-1\n");
            printf("💭 適合機動型戰術\n");
            break;
    }
}

/**
 * 獲取卡牌類型的字符串表示
 * 
 * 返回類型修正說明：
 * 這個函數必須返回 const char* 才能與 printf 的 %s 格式符兼容。
 */
static const char* get_card_type_name(Type type) {
    switch(type) {
        case BASIC_ATK: return "基礎攻擊";
        case BASIC_DEF: return "基礎防禦";
        case BASIC_MOV: return "基礎移動";
        case UNIVERSAL: return "通用";
        case SKILL_ATK: return "技能攻擊";
        case SKILL_DEF: return "技能防禦";
        case SKILL_MOV: return "技能移動";
        case EPIC: return "必殺";
        case TWIST: return "蛻變";
        default: return "未知";
    }
}

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