/*
 * 反轉寓言 (Twisted Fables) - 主程式
 * 
 * 這個主程式的設計哲學：
 * 1. 統一管理：協調三個角色檔案的運作
 * 2. 模組化測試：分別展示每個角色的特色
 * 3. 互動演示：讓角色之間進行模擬戰鬥
 * 4. 教學導向：透過註解幫助理解遊戲機制
 */

#include "twisted_fables.h"

// ===========================================
// 第一部分：基礎系統函數實作
// 這些函數就像是遊戲的「基礎設施」
// ===========================================

/**
 * 牌庫操作函數實作
 * 
 * 為什麼我們需要這些函數？
 * 想像牌庫就像是一個魔法袋子，我們需要有規範的方式
 * 來往裡面放卡牌、拿卡牌，以及重新排列卡牌的順序
 */

void add_deck(Deck *deck, Card *card) {
    // 檢查牌庫是否已滿 - 這就像檢查袋子是否還有空間
    if(deck->cnt < MAX_CARD && card != NULL) {
        deck->cards[deck->cnt++] = card;
        printf("📥 成功將卡牌加入牌庫，目前牌數：%d\n", deck->cnt);
    } else {
        printf("⚠️  牌庫已滿或卡牌無效，無法添加\n");
    }
}

Card *draw_deck(Deck *deck) {
    // 從牌庫頂部抽取卡牌 - 就像從一疊牌的最上面拿一張
    if(deck->cnt == 0) {
        printf("🚫 牌庫為空，無法抽牌\n");
        return NULL;
    }
    
    Card *drawn_card = deck->cards[--deck->cnt];
    printf("🎴 從牌庫抽取了一張卡牌，剩餘牌數：%d\n", deck->cnt);
    return drawn_card;
}

void shuffle_deck(Deck *deck) {
    // Fisher-Yates 洗牌算法 - 這是數學上證明最公平的洗牌方式
    printf("🔀 開始洗牌...\n");
    
    for(int i = deck->cnt - 1; i > 0; i--) {
        // 從剩餘的牌中隨機選擇一張
        int32_t j = rand() % (i + 1);
        
        // 交換兩張牌的位置
        Card *temp = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = temp;
    }
    
    printf("✅ 洗牌完成！\n");
}

void draw_hand(Player *player) {
    printf("🖐️ %s 開始抽取初始手牌...\n", player->fable->name);
    
    for(int i = 0; i < HAND_SIZE; i++) {
        Card *card = draw_deck(&player->draw);
        if(card) {
            add_deck(&player->hand, card);
        } else {
            printf("⚠️  牌庫不足，只抽到 %d 張手牌\n", i);
            break;
        }
    }
    
    printf("✅ %s 的手牌準備完成，共 %d 張\n", 
           player->fable->name, player->hand.cnt);
}

// ===========================================
// 第二部分：遊戲輔助函數
// 這些函數處理遊戲中的常見情況
// ===========================================

bool is_at_edge(Player *target) {
    // 檢查角色是否在戰場邊緣（位置0或2）
    bool at_edge = (target->fable->lane == 0 || target->fable->lane == 2);
    
    if(at_edge) {
        printf("🏁 %s 位於戰場邊緣（位置 %d）\n", 
               target->fable->name, target->fable->lane);
    }
    
    return at_edge;
}

void discard_random_card(Player *target) {
    if(target->hand.cnt == 0) {
        printf("🚫 %s 沒有手牌可以棄掉\n", target->fable->name);
        return;
    }
    
    // 隨機選擇一張手牌棄掉
    int random_index = rand() % target->hand.cnt;
    Card *discarded = target->hand.cards[random_index];
    
    // 將選中的牌移到棄牌區
    add_deck(&target->disc, discarded);
    
    // 從手牌中移除（將後面的牌往前移）
    for(int i = random_index; i < target->hand.cnt - 1; i++) {
        target->hand.cards[i] = target->hand.cards[i + 1];
    }
    target->hand.cnt--;
    
    printf("🗑️  %s 被迫棄掉一張手牌，剩餘手牌：%d 張\n", 
           target->fable->name, target->hand.cnt);
}

void move_adjacent_to_target(Player *attacker, Player *target) {
    int target_lane = target->fable->lane;
    int old_lane = attacker->fable->lane;
    
    // 智能移動邏輯：選擇最靠近目標的相鄰位置
    if(target_lane == 0) {
        attacker->fable->lane = 1;  // 目標在左邊緣，移動到中央
    } else if(target_lane == 2) {
        attacker->fable->lane = 1;  // 目標在右邊緣，移動到中央
    } else {
        // 目標在中央，隨機選擇左邊或右邊
        attacker->fable->lane = (rand() % 2 == 0) ? 0 : 2;
    }
    
    printf("🚶 %s 從位置 %d 移動到位置 %d（與 %s 相鄰）\n", 
           attacker->fable->name, old_lane, attacker->fable->lane, 
           target->fable->name);
}

// ===========================================
// 第三部分：遊戲狀態管理
// 這些函數管理整體遊戲流程
// ===========================================

void init_game(void) {
    printf("\n🎮 ===== 反轉寓言遊戲系統初始化 =====\n");
    
    // 初始化隨機數種子 - 確保每次遊戲都有不同的隨機結果
    srand((unsigned int)time(NULL));
    printf("🎲 隨機數系統已初始化\n");
    
    printf("✅ 遊戲系統初始化完成！\n\n");
}

bool is_game_over(Player *player1, Player *player2) {
    // 檢查是否有玩家生命值歸零
    if(player1->fable->health <= 0) {
        printf("💀 %s 生命值歸零！%s 獲勝！\n", 
               player1->fable->name, player2->fable->name);
        return true;
    }
    
    if(player2->fable->health <= 0) {
        printf("💀 %s 生命值歸零！%s 獲勝！\n", 
               player2->fable->name, player1->fable->name);
        return true;
    }
    
    return false;
}

void display_player_status(Player *player) {
    printf("\n📊 === %s 的狀態資訊 ===\n", player->fable->name);
    printf("❤️  生命值：%d\n", player->fable->health);
    printf("⚡ 能量值：%d\n", player->fable->energy);
    printf("🛡️  護盾值：%d\n", player->fable->blk);
    printf("🎯 必殺閾值：%d\n", player->fable->epic_threshold);
    printf("📍 位置：%d\n", player->fable->lane);
    
    // 顯示各個牌區的牌數
    printf("🎴 牌庫：%d 張\n", player->draw.cnt);
    printf("🖐️ 手牌：%d 張\n", player->hand.cnt);
    printf("🗑️  棄牌區：%d 張\n", player->disc.cnt);
    printf("🔄 蛻變牌：%d 張\n", player->twist.cnt);
    printf("💫 必殺牌：%d 張\n", player->epic.cnt);
    
    printf("================================\n\n");
}

void display_deck_contents(Deck *deck, const char *deck_name) {
    printf("📋 %s 內容（共%d張）：\n", deck_name, deck->cnt);
    
    if(deck->cnt == 0) {
        printf("   (空)\n");
        return;
    }
    
    for(int i = 0; i < deck->cnt; i++) {
        Card *card = deck->cards[i];
        const char *type_name;
        
        // 將卡牌類型轉換為可讀的名稱
        switch(card->type) {
            case BASIC_ATK: type_name = "基礎攻擊"; break;
            case BASIC_DEF: type_name = "基礎防禦"; break;
            case BASIC_MOV: type_name = "基礎移動"; break;
            case UNIVERSAL: type_name = "通用"; break;
            case SKILL_ATK: type_name = "技能攻擊"; break;
            case SKILL_DEF: type_name = "技能防禦"; break;
            case SKILL_MOV: type_name = "技能移動"; break;
            case EPIC: type_name = "必殺"; break;
            case TWIST: type_name = "蛻變"; break;
            default: type_name = "未知"; break;
        }
        
        printf("   %d. %s Lv%d (消耗:%d, 傷害:%d, 防禦:%d)\n", 
               i+1, type_name, card->dmg, card->cst, card->dmg, card->blk);
    }
    printf("\n");
}

// ===========================================
// 第四部分：角色展示和測試系統
// 這部分展示每個角色的獨特能力
// ===========================================

void demonstrate_character_abilities() {
    printf("\n🎭 ===== 角色能力展示 =====\n");
    
    // 創建三個玩家來展示不同角色
    Player mulan_player = {0};
    Player kaguya_player = {0};
    Player alice_player = {0};
    
    printf("\n1️⃣ 花木蘭角色展示：\n");
    printf("花木蘭是氣值系統的大師，她可以累積氣來強化技能\n");
    setup_mulan(&mulan_player);
    display_player_status(&mulan_player);
    
    printf("\n2️⃣ 輝夜姬角色展示：\n");
    printf("輝夜姬專精防禦，她可以透過高防禦值來增強攻擊力\n");
    setup_kaguya(&kaguya_player);
    display_player_status(&kaguya_player);
    
    printf("\n3️⃣ 愛麗絲角色展示：\n");
    printf("愛麗絲擁有三重身份，可以根據戰況切換不同的戰鬥風格\n");
    setup_alice(&alice_player);
    display_player_status(&alice_player);
    
    // 清理記憶體
    free(mulan_player.fable);
    free(kaguya_player.fable);
    free(alice_player.fable);
}

void simulate_combat_demo() {
    printf("\n⚔️  ===== 模擬戰鬥演示 =====\n");
    printf("讓我們來看看角色之間的技能互動\n\n");
    
    // 創建兩個玩家進行模擬戰鬥
    Player player1 = {0};
    Player player2 = {0};
    
    // 設置花木蘭 vs 愛麗絲的對戰
    setup_mulan(&player1);
    setup_alice(&player2);
    
    printf("🥊 今日對戰：%s VS %s\n\n", 
           player1.fable->name, player2.fable->name);
    
    // 模擬花木蘭使用攻擊技能
    printf("🗡️  %s 使用攻擊技能：\n", player1.fable->name);
    mulan_effect(&player1, &player2, SKILL_ATK, 2);
    display_player_status(&player2);
    
    // 模擬愛麗絲使用防禦技能
    printf("🛡️  %s 使用防禦技能：\n", player2.fable->name);
    alice_effect(&player2, &player1, SKILL_DEF, 1);
    display_player_status(&player2);
    
    printf("🎯 戰鬥演示完成！\n");
    
    // 清理記憶體
    free(player1.fable);
    free(player2.fable);
}

void interactive_menu() {
    int choice;
    
    do {
        printf("\n🎮 ===== 反轉寓言互動選單 =====\n");
        printf("1. 角色能力展示\n");
        printf("2. 模擬戰鬥演示\n");
        printf("3. 創建自定義角色測試\n");
        printf("4. 查看遊戲統計\n");
        printf("0. 退出遊戲\n");
        printf("請選擇功能 (0-4): ");
        
        if(scanf("%d", &choice) != 1) {
            printf("❌ 輸入格式錯誤，請輸入數字\n");
            // 清除輸入緩衝區
            while(getchar() != '\n');
            continue;
        }
        
        switch(choice) {
            case 1:
                demonstrate_character_abilities();
                break;
            case 2:
                simulate_combat_demo();
                break;
            case 3:
                printf("🚧 自定義角色功能開發中...\n");
                break;
            case 4:
                printf("📈 遊戲統計功能開發中...\n");
                break;
            case 0:
                printf("👋 感謝遊玩反轉寓言！再見！\n");
                break;
            default:
                printf("❌ 無效選擇，請重新輸入\n");
                break;
        }
        
    } while(choice != 0);
}

// ===========================================
// 第五部分：主程式入口
// 這是整個程式的起始點
// ===========================================

int main() {
    // 遊戲啟動歡迎訊息
    printf("🌟 ========================================\n");
    printf("🎭    歡迎來到反轉寓言 (Twisted Fables)    \n");
    printf("🌟 ========================================\n");
    printf("\n這是一個卡牌戰鬥遊戲，三位來自不同寓言的角色\n");
    printf("將在這裡展現她們獨特的戰鬥風格和能力！\n\n");
    
    // 初始化遊戲系統
    init_game();
    
    // 進入互動選單
    interactive_menu();
    
    printf("\n💫 程式執行完畢，所有資源已清理\n");
    return 0;
}

/*
 * 程式設計總結與學習重點：
 * 
 * 1. 模組化設計：
 *    - 每個函數都有明確的職責
 *    - 相關功能被組織在一起
 *    - 易於維護和擴展
 * 
 * 2. 錯誤處理：
 *    - 檢查指標是否為NULL
 *    - 驗證陣列邊界
 *    - 提供有意義的錯誤訊息
 * 
 * 3. 用戶體驗：
 *    - 豐富的視覺回饋（表情符號）
 *    - 清楚的功能說明
 *    - 互動式選單系統
 * 
 * 4. 記憶體管理：
 *    - 適當的動態記憶體分配
 *    - 及時釋放不再使用的記憶體
 *    - 避免記憶體洩露
 * 
 * 5. 可擴展性：
 *    - 預留了擴展接口
 *    - 統一的資料結構
 *    - 靈活的配置系統
 */