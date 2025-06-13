/*
 * 反轉寓言 (Twisted Fables) - 白雪公主角色實作
 * 
 * 設計理念：中毒控制大師
 * 
 * 白雪公主是一個以「中毒牌庫系統」為核心的控制型角色
 * 她的戰術重點在於：
 * 1. 資源剝奪：透過磨牌削弱對手的戰鬥潛力
 * 2. 持續傷害：中毒牌系統提供長期威脅
 * 3. 場面控制：限制對手的行動選項
 * 4. 後期優勢：隨著中毒牌累積，威力遞增
 * 
 * 這個檔案專注於白雪公主的獨特機制實作
 * 所有基礎函數都在 game_base.c 中實作
 */

#include "twisted_fables.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 白雪公主專用常數定義
#define POISON_DECK_SIZE 18  // 中毒牌庫大小 (每等級6張，共3等級)

// ===========================================
// 白雪公主的核心設計概念
// ===========================================

/*
 * 白雪公主代表著童話中「毒蘋果」的概念在卡牌遊戲中的體現
 * 她的設計哲學圍繞著幾個核心主題：
 * 
 * 1. 【美麗與危險並存】
 *    如同童話中的毒蘋果外表美麗卻致命，白雪公主的技能看似優雅
 *    但實際上在暗中削弱對手，這種「隱藏的威脅」是她的核心特色
 * 
 * 2. 【間接勝利路線】
 *    不同於花木蘭的直接爆發，白雪公主走的是「溫水煮青蛙」路線
 *    透過持續的資源剝奪和控制效果，逐漸將對手逼入絕境
 * 
 * 3. 【資源管理大師】
 *    中毒牌庫系統體現了「有限資源」的遊戲設計概念
 *    玩家必須策略性地運用中毒牌，不能無腦地浪費這個寶貴資源
 */

// 全域中毒牌庫系統
// 這個設計選擇體現了白雪公主的「稀有資源」概念
static Deck global_poison_deck;
static bool poison_deck_initialized = false;

// ===========================================
// 白雪公主的中毒牌庫系統
// 這是她最核心、最獨特的機制
// ===========================================

/*
 * 中毒牌庫系統的設計理念深度解析：
 * 
 * 為什麼要設計一個「全域」中毒牌庫？這個設計選擇背後有深刻的遊戲設計考量：
 * 
 * 1. 【稀缺性創造策略深度】
 *    如果中毒牌是無限的，玩家就會無腦地使用，失去策略思考的必要性
 *    有限的中毒牌迫使玩家必須精打細算，每一張中毒牌的使用都變得重要
 * 
 * 2. 【模擬現實的邏輯】
 *    在童話中，毒蘋果也不是無限供應的，白雪公主需要精心調製每一份毒藥
 *    這種「有限資源」的概念讓遊戲更貼近童話的原始邏輯
 * 
 * 3. 【平衡性考量】
 *    如果白雪公主可以無限制地投放中毒牌，遊戲的平衡性會被破壞
 *    有限的中毒牌確保她不會過於強勢，需要與其他機制配合才能發揮最大效果
 */

/**
 * 初始化中毒牌庫
 * 
 * 這個函數負責設置白雪公主專用的中毒牌庫
 * 它展示了如何在C語言中實現「單例模式」的概念
 */
void init_poison_deck(){
    // 單例模式的實現：確保中毒牌庫只被初始化一次
    // 這個檢查很重要，因為如果重複初始化會造成資源浪費
    if(poison_deck_initialized) {
        return; // 如果已經初始化過，直接返回
    }
    
    // 清空全域中毒牌庫
    global_poison_deck.cnt = 0;
    
    /*
     * 中毒牌的等級設計解析：
     * 
     * 我們創建三個等級的中毒牌，每個等級各6張，總共18張
     * 這個數量設計有其深層考量：
     * 
     * 1. 【等級差異化】
     *    不同等級的中毒牌可以對應不同的戰術需求
     *    低等級適合頻繁使用，高等級適合關鍵時刻
     * 
     * 2. 【總量平衡】
     *    18張是一個精心計算的數量，既不會讓白雪公主過於依賴中毒牌
     *    也不會讓她的核心機制太快耗盡
     * 
     * 3. 【分配策略】
     *    每個等級6張的平均分配，讓玩家在使用時有均衡的選擇
     */
    
    // 創建靜態中毒牌陣列
    // 使用靜態陣列是為了避免動態記憶體分配的複雜性
    static Card poison_cards[POISON_DECK_SIZE];
    
    // 建立三個等級的中毒牌
    for(int level = 1; level <= 3; level++){
        for(int i = 0; i < 6; i++){
            int index = (level - 1) * 6 + i; // 計算在陣列中的位置
            
            // 設置中毒牌的屬性
            poison_cards[index] = (Card){
                .type = POISON,     // 標記為中毒牌類型
                .cst = 0,          // 中毒牌本身不消耗能量
                .dmg = level,      // 等級作為傷害值標識
                .blk = 0,          // 中毒牌不提供防禦
                .mov = 0,          // 中毒牌不提供移動
                .rng = 0,          // 中毒牌沒有射程概念
                .effect = NULL     // 中毒牌的效果由其他機制處理
            };
            
            // 將建立好的中毒牌加入全域牌庫
            add_deck(&global_poison_deck, &poison_cards[index]);
        }
    }
    
    // 標記初始化完成
    poison_deck_initialized = true;
    
    printf("🍎 白雪公主的中毒藥劑已準備就緒，共 %d 瓶毒藥\n", global_poison_deck.cnt);
}

/**
 * 從中毒牌庫取得指定等級的中毒牌
 * 
 * 這個函數實現了「智能選擇」邏輯：
 * 不是隨機取牌，而是根據需求取得特定等級的中毒牌
 * 這讓白雪公主的玩家可以更精確地控制戰術節奏
 */
Card* get_poison_card(int32_t level){
    // 確保中毒牌庫已經初始化
    if(!poison_deck_initialized) {
        init_poison_deck();
    }
    
    /*
     * 搜尋演算法的選擇說明：
     * 
     * 這裡我們使用線性搜尋來找指定等級的中毒牌
     * 雖然時間複雜度是 O(n)，但考慮到以下因素，這是合理的選擇：
     * 
     * 1. 【數據量小】：最多只有18張牌，線性搜尋的效能影響微乎其微
     * 2. 【程式碼簡潔】：線性搜尋易於理解和維護
     * 3. 【記憶體效率】：不需要額外的索引結構
     */
    
    // 搜尋指定等級的中毒牌
    for(int i = 0; i < global_poison_deck.cnt; i++){
        if(global_poison_deck.cards[i]->dmg == level){
            // 找到了！現在需要將其從牌庫中移除
            Card *found_poison = global_poison_deck.cards[i];
            
            /*
             * 陣列壓縮技術：
             * 
             * 當我們從陣列中間移除一個元素時，需要將後面的元素往前移
             * 這確保陣列中沒有「空洞」，維持資料結構的完整性
             */
            for(int j = i; j < global_poison_deck.cnt - 1; j++){
                global_poison_deck.cards[j] = global_poison_deck.cards[j + 1];
            }
            global_poison_deck.cnt--; // 更新牌庫計數
            
            return found_poison; // 返回找到的中毒牌
        }
    }
    
    // 如果找不到指定等級的中毒牌，給出警告但不會讓程式崩潰
    printf("⚠️  白雪公主的毒藥已經用完了（等級 %d）\n", level);
    return NULL;
}

/**
 * 將中毒牌放入對手棄牌堆，並觸發至純之毒效果
 * 
 * 這個函數體現了白雪公主機制的「連鎖反應」設計：
 * 不只是單純放入中毒牌，還會檢查並觸發相關的強化效果
 */
void add_poison_to_discard(Player *snowwhite, Player *target, int32_t level){
    // 首先嘗試獲取指定等級的中毒牌
    Card *poison_card = get_poison_card(level);
    
    if(poison_card != NULL){
        // 成功獲得中毒牌，將其放入對手的棄牌堆
        add_deck(&target->disc, poison_card);
        printf("🍎 一份等級 %d 的毒藥悄悄滲入了%s的靈魂...\n", 
               level, target->fable->name);
        
        /*
         * 至純之毒效果的觸發機制：
         * 
         * 這裡展示了「累積強化」的遊戲設計概念
         * 隨著白雪公主使用更多的蛻變牌，她的中毒效果會變得越來越致命
         * 這種「滾雪球」效應讓後期的白雪公主變得非常可怕
         */
        int enhancement_count = snowwhite->fable->snowwhite_data.poison_enhancement_count;
        if(enhancement_count > 0){
            target->fable->health -= enhancement_count;
            printf("💀 至純之毒的效果發作！%s額外失去 %d 點生命\n", 
                   target->fable->name, enhancement_count);
            printf("❤️  %s剩餘生命：%d\n", 
                   target->fable->name, target->fable->health);
        }
    } else {
        // 如果中毒牌已經用完，給出遊戲化的提示
        printf("🚫 白雪公主的毒藥庫存不足，無法製作等級 %d 的毒藥\n", level);
    }
}

/**
 * 磨牌功能：將對手牌庫頂的牌放入棄牌堆
 * 
 * 磨牌是一種「間接攻擊」的概念，不直接造成傷害，
 * 但會削弱對手的戰鬥潛力，是一種高級的戰術手段
 */
void mill_cards(Player *target, int32_t count){
    printf("⚡ 水晶的力量開始侵蝕%s的戰術準備...\n", target->fable->name);
    
    for(int i = 0; i < count; i++){
        Card *milled_card = draw_deck(&target->draw);
        if(milled_card != NULL){
            add_deck(&target->disc, milled_card);
            printf("   💎 一張珍貴的卡牌化為了水晶碎片\n");
        } else {
            printf("   🏜️  %s的戰術儲備已經枯竭，無法繼續侵蝕\n", target->fable->name);
            break; // 如果牌庫已空，停止磨牌
        }
    }
    
    printf("📊 磨牌完成：%s失去了 %d 張潛在的戰術選擇\n", 
           target->fable->name, (count <= target->draw.cnt + target->disc.cnt) ? count : target->draw.cnt + target->disc.cnt);
}

// ===========================================
// 白雪公主的技能效果系統
// 這裡實現她所有技能的具體邏輯
// ===========================================

/**
 * 白雪公主的核心效果處理函數
 * 
 * 這個函數是整個白雪公主系統的核心，它根據不同的卡牌類型和等級
 * 來執行相應的效果。這種設計讓我們可以用統一的接口處理多樣的效果
 */
void snowwhite_effect(void *self, void *target, Type type, int32_t dmg){
    // 型別轉換：將void指標轉換為具體的Player指標
    Player *attacker = (Player *)self;
    Player *defender = (Player *)target;
    
    printf("\n❄️  === 白雪公主的魔法啟動 ===\n");
    printf("🍎 %s正在施展她的童話魔法...\n", attacker->fable->name);
    
    /*
     * 技能分類處理的設計說明：
     * 
     * 我們使用switch語句來處理不同類型的技能，這種設計有幾個優點：
     * 1. 【清晰的邏輯分組】：相同類型的技能被歸類在一起
     * 2. 【易於維護】：添加新技能時只需要在對應的case中添加
     * 3. 【效能考量】：switch語句通常比多層if-else更高效
     */
    
    switch(type){
        case SKILL_ATK: // 攻擊技能系列
            handle_snowwhite_attack_skills(attacker, defender, dmg);
            break;
            
        case SKILL_DEF: // 防禦技能系列
            handle_snowwhite_defense_skills(attacker, defender, dmg);
            break;
            
        case SKILL_MOV: // 移動技能系列
            handle_snowwhite_movement_skills(attacker, defender, dmg);
            break;
            
        case TWIST: // 蛻變技能系列
            handle_snowwhite_twist_skills(attacker, defender, dmg);
            break;
            
        case EPIC: // 必殺技系列
            handle_snowwhite_epic_skills(attacker, defender, dmg);
            break;
            
        default:
            printf("🤔 白雪公主試圖使用一個未知的魔法...\n");
            break;
    }
    
    // 顯示戰鬥結果
    printf("💖 %s剩餘生命：%d\n", defender->fable->name, defender->fable->health);
    printf("❄️  ========================\n");
}

/**
 * 處理白雪公主的攻擊技能
 * 
 * 攻擊技能的設計理念是「美麗而致命」：
 * 既有直接的傷害效果，又有獨特的磨牌機制
 */
void handle_snowwhite_attack_skills(Player *attacker, Player *defender, int32_t level){
    // 技能名稱的詩意設計體現了白雪公主的美學特色
    const char* skill_names[] = {"", "水晶碎片", "水晶漩渦", "水晶風暴"};
    
    printf("✨ 白雪公主使用「%s」\n", skill_names[level]);
    printf("💎 水晶的力量開始顯現，既美麗又危險...\n");
    
    // 計算並造成傷害
    int damage = level; // 基礎傷害等於技能等級
    damage = calculate_final_damage(defender, damage);
    
    if(damage > 0){
        defender->fable->health -= damage;
        printf("⚔️  %s受到了 %d 點水晶傷害\n", defender->fable->name, damage);
        
        // 檢查水晶之棺的觸發條件
        if(damage >= 2 && attacker->fable->snowwhite_data.crystal_coffin_active){
            printf("⚱️  水晶之棺的詛咒被觸發！\n");
            add_poison_to_discard(attacker, defender, 1);
        }
    }
    
    // 磨牌效果：攻擊技能的核心特色
    mill_cards(defender, level);
}

/**
 * 處理白雪公主的防禦技能
 * 
 * 防禦技能體現了「玷污」的主題：
 * 不是傳統的防禦，而是透過散佈毒素來控制戰場
 */
void handle_snowwhite_defense_skills(Player *attacker, Player *defender, int32_t level){
    const char* skill_names[] = {"", "玷污的恩惠", "玷污的盛筵", "玷污的狂歡"};
    
    printf("🖤 白雪公主使用「%s」\n", skill_names[level]);
    printf("☠️  黑暗的恩賜開始散佈，帶來甜美的毒素...\n");
    
    // 造成傷害
    int damage = level;
    damage = calculate_final_damage(defender, damage);
    
    if(damage > 0){
        defender->fable->health -= damage;
        printf("💀 %s被毒素侵蝕，失去 %d 點生命\n", defender->fable->name, damage);
    }
    
    // 放入中毒牌：防禦技能的核心機制
    for(int i = 0; i < level; i++){
        add_poison_to_discard(attacker, defender, 1);
    }
}

/**
 * 處理白雪公主的移動技能
 * 
 * 移動技能體現了「破碎」的主題：
 * 代表著童話世界的扭曲和現實的破滅
 */
void handle_snowwhite_movement_skills(Player *attacker, Player *defender, int32_t level){
    const char* skill_names[] = {"", "破碎的幻想", "破碎的現實", "破碎的命運"};
    
    printf("💔 白雪公主使用「%s」\n", skill_names[level]);
    printf("🌪️  現實開始扭曲，美好的幻象正在破滅...\n");
    
    // 造成傷害
    int damage = level;
    damage = calculate_final_damage(defender, damage);
    
    if(damage > 0){
        defender->fable->health -= damage;
        printf("💸 %s的希望破滅，受到 %d 點心靈創傷\n", defender->fable->name, damage);
    }
    
    // 移動到相鄰位置
    move_adjacent_to_target(attacker, defender);
    printf("👻 白雪公主如幽靈般出現在%s身邊\n", defender->fable->name);
}

/**
 * 處理白雪公主的蛻變技能
 * 
 * 蛻變技能代表著角色能力的永久性提升
 * 這些效果會持續影響後續的戰鬥
 */
void handle_snowwhite_twist_skills(Player *attacker, Player *defender, int32_t twist_id){
    switch(twist_id){
        case 1: // 水晶之棺
            printf("⚱️  發動蛻變：水晶之棺！\n");
            printf("💀 當造成2點以上傷害時，將額外放入中毒牌\n");
            attacker->fable->snowwhite_data.crystal_coffin_active = true;
            break;
            
        case 2: // 至純之毒
            printf("☠️  發動蛻變：至純之毒！\n");
            attacker->fable->snowwhite_data.poison_enhancement_count++;
            printf("🧪 毒素的純度提升了！當前純度等級：%d\n", 
                   attacker->fable->snowwhite_data.poison_enhancement_count);
            break;
            
        case 3: // 墮落之劫
            printf("😈 發動蛻變：墮落之劫！\n");
            printf("🔄 現在可以將中毒牌洗入對手牌庫而非棄牌堆\n");
            attacker->fable->snowwhite_data.corruption_raid_active = true;
            break;
            
        case 4: // 劇毒之蝕
            printf("🐍 發動蛻變：劇毒之蝕！\n");
            printf("🚶 移動穿越時可以散佈毒素\n");
            attacker->fable->snowwhite_data.toxic_erosion_active = true;
            break;
    }
}

/**
 * 處理白雪公主的必殺技
 * 
 * 必殺技是白雪公主最強大的招式
 * 每一個都有著戲劇性的效果和童話般的表現
 */
void handle_snowwhite_epic_skills(Player *attacker, Player *defender, int32_t epic_id){
    switch(epic_id){
        case 1: // 七蛇之怒
            {
                printf("🐉 發動必殺技：七蛇之怒！\n");
                printf("⚡ 所有潛伏的毒素同時爆發！\n");
                
                // 計算對手棄牌堆中的中毒牌數量
                int poison_count = count_poison_cards_in_discard(defender);
                defender->fable->health -= poison_count;
                
                printf("💥 %s棄牌堆中的 %d 張中毒牌同時發作！\n", 
                       defender->fable->name, poison_count);
                printf("☠️  造成了 %d 點致命傷害！\n", poison_count);
            }
            break;
            
        case 2: // 魔鏡之雨
            printf("🪞 發動必殺技：魔鏡之雨！\n");
            printf("🌧️  真相的雨水洗刷一切虛假...\n");
            
            // 造成基礎傷害
            int damage = calculate_final_damage(defender, 3);
            if(damage > 0){
                defender->fable->health -= damage;
                printf("⚔️  %s受到 %d 點真相傷害\n", defender->fable->name, damage);
            }
            
            // 強制重置手牌
            force_hand_reset(defender);
            break;
            
        case 3: // 醞釀之災
            printf("🌪️  發動必殺技：醞釀之災！\n");
            printf("⏳ 時間開始倒流，過去與現在交融...\n");
            
            // 造成傷害
            damage = calculate_final_damage(defender, 3);
            if(damage > 0){
                defender->fable->health -= damage;
                printf("⚔️  %s受到 %d 點時空傷害\n", defender->fable->name, damage);
            }
            
            // 將棄牌堆的牌洗回牌庫
            recycle_discard_to_deck(defender, 3);
            break;
    }
}

// ===========================================
// 輔助函數：支援主要功能的小工具
// ===========================================

/**
 * 計算最終傷害（考慮護盾）
 */
int calculate_final_damage(Player *target, int base_damage){
    if(target->fable->blk > 0){
        int blocked = (target->fable->blk >= base_damage) ? base_damage : target->fable->blk;
        target->fable->blk -= blocked;
        base_damage -= blocked;
        
        if(blocked > 0){
            printf("🛡️  護盾吸收了 %d 點傷害，剩餘護盾：%d\n", 
                   blocked, target->fable->blk);
        }
    }
    return base_damage;
}

/**
 * 計算棄牌堆中的中毒牌數量
 */
int count_poison_cards_in_discard(Player *player){
    int poison_count = 0;
    for(int i = 0; i < player->disc.cnt; i++){
        if(player->disc.cards[i]->type == POISON){
            poison_count++;
        }
    }
    return poison_count;
}

/**
 * 強制手牌重置（魔鏡之雨專用）
 */
void force_hand_reset(Player *player){
    printf("🃏 %s被迫棄掉所有手牌...\n", player->fable->name);
    
    // 將所有手牌移到棄牌堆
    while(player->hand.cnt > 0){
        Card *discarded = draw_deck(&player->hand);
        if(discarded) add_deck(&player->disc, discarded);
    }
    
    // 抽取4張新牌
    printf("🎴 %s抽取4張新牌\n", player->fable->name);
    for(int i = 0; i < 4; i++){
        Card *drawn = draw_deck(&player->draw);
        if(drawn) add_deck(&player->hand, drawn);
    }
}

/**
 * 將棄牌堆的牌回收到牌庫（醞釀之災專用）
 */
void recycle_discard_to_deck(Player *player, int max_cards){
    int recycled = 0;
    printf("♻️  正在將%s的過去重新編織...\n", player->fable->name);
    
    for(int i = 0; i < max_cards && player->disc.cnt > 0; i++){
        Card *recycled_card = draw_deck(&player->disc);
        if(recycled_card){
            add_deck(&player->draw, recycled_card);
            recycled++;
        }
    }
    
    if(recycled > 0){
        shuffle_deck(&player->draw);
        printf("🔄 %d 張牌從遺忘中歸來，重新洗入牌庫\n", recycled);
    }
}

// ===========================================
// 白雪公主角色設置系統
// ===========================================

/**
 * 設置白雪公主角色
 * 
 * 這個函數是白雪公主的「出生地」，在這裡她的所有能力被賦予
 * 每一個數值的設定都經過精心考量，以確保遊戲的平衡性
 */
void setup_snowwhite(Player *p){
    printf("🍎 === 白雪公主的故事開始 ===\n");
    printf("從前有一位美麗的公主，她擁有如雪般純白的肌膚...\n");
    printf("但在這個扭曲的寓言世界中，她學會了更危險的技藝...\n\n");
    
    // 初始化中毒牌庫系統
    init_poison_deck();
    
    // 定義白雪公主的所有技能卡牌
    // 每張卡牌都是她故事的一個片段
    
    // 攻擊系列：水晶的美麗與危險
    static Card attack_lv1 = {
        .type = SKILL_ATK, .cst = 0, .dmg = 1, .blk = 0, .mov = 0, .rng = 1, .effect = snowwhite_effect
    };
    static Card attack_lv2 = {
        .type = SKILL_ATK, .cst = 2, .dmg = 2, .blk = 0, .mov = 0, .rng = 1, .effect = snowwhite_effect
    };
    static Card attack_lv3 = {
        .type = SKILL_ATK, .cst = 4, .dmg = 3, .blk = 0, .mov = 0, .rng = 1, .effect = snowwhite_effect
    };
    
    // 防禦系列：毒素的恩賜
    static Card defense_lv1 = {
        .type = SKILL_DEF, .cst = 0, .dmg = 1, .blk = 0, .mov = 0, .rng = 1, .effect = snowwhite_effect
    };
    static Card defense_lv2 = {
        .type = SKILL_DEF, .cst = 2, .dmg = 2, .blk = 0, .mov = 0, .rng = 1, .effect = snowwhite_effect
    };
    static Card defense_lv3 = {
        .type = SKILL_DEF, .cst = 4, .dmg = 3, .blk = 0, .mov = 0, .rng = 1, .effect = snowwhite_effect
    };
    
    // 移動系列：現實的破碎
    static Card move_lv1 = {
        .type = SKILL_MOV, .cst = 0, .dmg = 1, .blk = 0, .mov = 0, .rng = 0, .effect = snowwhite_effect
    };
    static Card move_lv2 = {
        .type = SKILL_MOV, .cst = 2, .dmg = 2, .blk = 0, .mov = 0, .rng = 1, .effect = snowwhite_effect
    };
    static Card move_lv3 = {
        .type = SKILL_MOV, .cst = 4, .dmg = 3, .blk = 0, .mov = 0, .rng = 2, .effect = snowwhite_effect
    };
    
    // 蛻變系列：力量的進化
    static Card twist_cards[4] = {
        {TWIST, 0, 1, 0, 0, 0, snowwhite_effect}, // 水晶之棺
        {TWIST, 0, 2, 0, 0, 0, snowwhite_effect}, // 至純之毒
        {TWIST, 0, 3, 0, 0, 0, snowwhite_effect}, // 墮落之劫
        {TWIST, 0, 4, 0, 0, 0, snowwhite_effect}  // 劇毒之蝕
    };
    
    // 必殺技系列：終極奧義
    static Card epic_cards[3] = {
        {EPIC, 0, 1, 0, 0, 1, snowwhite_effect}, // 七蛇之怒
        {EPIC, 0, 2, 0, 0, 1, snowwhite_effect}, // 魔鏡之雨
        {EPIC, 0, 3, 0, 0, 3, snowwhite_effect}  // 醞釀之災
    };
    
    // 創建白雪公主的角色數據
    Fable *snowwhite = malloc(sizeof(Fable));
    *snowwhite = (Fable){
        .health = 34,           // 生命上限：脆弱但不至於太弱
        .energy = 6,            // 起始能量：標準配置
        .blk = 0,              // 起始護盾：依賴技能提供防護
        .epic_threshold = 17,   // 必殺技閾值：稍高，體現控制型角色的定位
        .lane = 1,             // 起始位置：中央，便於施展技能
        .snowwhite_data = {     // 專有數據：全部從零開始
            .poison_enhancement_count = 0,
            .crystal_coffin_active = false,
            .corruption_raid_active = false,
            .toxic_erosion_active = false
        }
    };
    
    strcpy(snowwhite->name, "白雪公主");
    p->fable = snowwhite;
    
    // 初始化所有牌區
    p->draw.cnt = 0; p->disc.cnt = 0; p->bani.cnt = 0;
    p->hand.cnt = 0; p->twist.cnt = 0; p->epic.cnt = 0;
    
    // 按照遊戲平衡設計添加卡牌
    // 攻擊牌：1+2+3=6張
    add_deck(&p->draw, &attack_lv1);
    add_deck(&p->draw, &attack_lv2); add_deck(&p->draw, &attack_lv2);
    add_deck(&p->draw, &attack_lv3); add_deck(&p->draw, &attack_lv3); add_deck(&p->draw, &attack_lv3);
    
    // 防禦牌：1+2+3=6張
    add_deck(&p->draw, &defense_lv1);
    add_deck(&p->draw, &defense_lv2); add_deck(&p->draw, &defense_lv2);
    add_deck(&p->draw, &defense_lv3); add_deck(&p->draw, &defense_lv3); add_deck(&p->draw, &defense_lv3);
    
    // 移動牌：1+2+3=6張
    add_deck(&p->draw, &move_lv1);
    add_deck(&p->draw, &move_lv2); add_deck(&p->draw, &move_lv2);
    add_deck(&p->draw, &move_lv3); add_deck(&p->draw, &move_lv3); add_deck(&p->draw, &move_lv3);
    
    // 蛻變牌：每種各1張，總共4張
    for(int i = 0; i < 4; i++){
        add_deck(&p->twist, &twist_cards[i]);
    }
    
    // 必殺技：每種各1張，總共3張
    for(int i = 0; i < 3; i++){
        add_deck(&p->epic, &epic_cards[i]);
    }
    
    // 洗牌並抽取起始手牌
    shuffle_deck(&p->draw);
    draw_hand(p);
    
    printf("✨ 白雪公主已經準備好展示她的黑暗魔法了！\n");
    printf("📊 角色數據總覽：\n");
    printf("   ❤️  生命值：%d\n", snowwhite->health);
    printf("   🎯 必殺閾值：%d\n", snowwhite->epic_threshold);
    printf("   🎴 牌庫：%d 張卡牌\n", p->draw.cnt);
    printf("   🖐️ 手牌：%d 張\n", p->hand.cnt);
    printf("   🍎 中毒牌庫：%d 瓶毒藥\n", global_poison_deck.cnt);
    printf("🍎 ===========================\n");
}