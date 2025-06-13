#ifndef TWISTED_FABLES_H
#define TWISTED_FABLES_H

// 標準函式庫引入
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

// SDL2 函式庫引入（如果需要圖形界面）
#ifdef USE_SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#endif

// 遊戲常數定義
#define MAX_CARD 100        // 最大牌數
#define WCARD 100          // 卡牌寬度
#define HCARD 140          // 卡牌高度
#define WSCREEN 1200       // 螢幕寬度
#define HSCREEN 800        // 螢幕高度
#define HAND_SIZE 5        // 手牌數量
#define MAX_NAME_LEN 50    // 角色名稱最大長度

// 卡牌類型枚舉
// 這個枚舉定義了遊戲中所有可能的卡牌類型
typedef enum {
    BASIC_ATK = 0,  // 基礎攻擊牌
    BASIC_DEF,      // 基礎防禦牌  
    BASIC_MOV,      // 基礎移動牌
    UNIVERSAL,      // 通用牌
    SKILL_ATK,      // 技能攻擊牌
    SKILL_DEF,      // 技能防禦牌
    SKILL_MOV,      // 技能移動牌
    EPIC,           // 必殺牌
    TWIST           // 蛻變牌、反轉牌
} Type;

// 愛麗絲專用的身份枚舉
// 愛麗絲可以在三種身份之間切換，每種身份有不同的效果
typedef enum {
    IDENTITY_RED_QUEEN,    // 紅心皇后：攻擊+1, 移動-1
    IDENTITY_MAD_HATTER,   // 瘋帽子：防禦+1, 攻擊-1  
    IDENTITY_CHESHIRE_CAT  // 柴郡貓：移動+1, 防禦-1
} AliceIdentity;

// 前向宣告（避免循環依賴）
struct Player;
struct Card;

// 卡牌結構體
// 代表遊戲中的一張卡牌，包含所有必要的屬性和效果函數指標
typedef struct Card {
    Type type;              // 卡牌類型
    int32_t cst;           // 能量消耗
    int32_t dmg;           // 攻擊傷害值（同時也代表卡牌等級）
    int32_t blk;           // 防禦值
    int32_t mov;           // 移動距離
    int32_t rng;           // 射程：0=近戰, 1=相鄰, 2=全域
    // 卡牌效果函數指標：接受施法者、目標、卡牌類型和傷害值
    void (*effect)(void *self, void *target, Type type, int32_t dmg);
} Card;

// 牌庫結構體
// 用來管理一組卡牌，例如手牌、抽牌區、棄牌區等
typedef struct {
    Card *cards[MAX_CARD];  // 卡牌指標陣列
    int32_t cnt;           // 目前牌庫中的卡牌數量
} Deck;

// 角色基本屬性結構體
// 儲存角色的基本資訊和狀態
typedef struct {
    char name[MAX_NAME_LEN];    // 角色名稱
    int32_t health;             // 當前生命值
    int32_t energy;             // 當前能量值
    int32_t blk;               // 當前護盾值
    int32_t epic_threshold;     // 必殺技閾值
    int32_t lane;              // 當前位置（0, 1, 或 2）
    
    // 角色特殊屬性
    union {
        // 花木蘭特有的氣值系統
        struct {
            int32_t qi;             // 氣值（花木蘭專用）
        } mulan_data;
        
        // 愛麗絲特有的身份系統
        struct {
            AliceIdentity current_identity;  // 當前身份
        } alice_data;
        
        // 輝夜姬特有的防禦相關數據
        struct {
            int32_t defense_bonus;      // 防禦加成
            int32_t card_draw_bonus;    // 抽牌加成
        } kaguya_data;
    } special;
} Fable;

// 玩家結構體
// 包含角色資訊和所有相關的牌庫
typedef struct Player {
    Fable *fable;           // 角色基本資訊
    Deck draw;              // 牌庫（抽牌區）
    Deck disc;              // 棄牌區
    Deck bani;              // 出牌區
    Deck hand;              // 手牌
    Deck twist;             // 反轉牌區/蛻變牌區
    Deck epic;              // 必殺牌區
} Player;

// 基本牌庫操作函數宣告
// 這些函數提供了基本的牌庫管理功能

/**
 * 向牌庫添加一張卡牌
 * @param deck 目標牌庫
 * @param card 要添加的卡牌
 */
void add_deck(Deck *deck, Card *card);

/**
 * 從牌庫抽取一張卡牌
 * @param deck 目標牌庫
 * @return 抽取的卡牌指標，如果牌庫為空則返回NULL
 */
Card *draw_deck(Deck *deck);

/**
 * 洗牌函數
 * @param deck 要洗的牌庫
 */
void shuffle_deck(Deck *deck);

/**
 * 抽取起始手牌
 * @param player 玩家指標
 */
void draw_hand(Player *player);

// 角色設置函數宣告
// 每個角色都有自己的設置函數來初始化角色資料和牌組

/**
 * 設置花木蘭角色
 * @param player 玩家指標
 */
void setup_mulan(Player *player);

/**
 * 設置輝夜姬角色  
 * @param player 玩家指標
 */
void setup_kaguya(Player *player);

/**
 * 設置愛麗絲角色
 * @param player 玩家指標
 */
void setup_alice(Player *player);

// 角色效果函數宣告
// 每個角色都有自己的效果處理函數

/**
 * 花木蘭的卡牌效果處理
 * @param self 施法者
 * @param target 目標
 * @param type 卡牌類型
 * @param dmg 傷害值/等級
 */
void mulan_effect(void *self, void *target, Type type, int32_t dmg);

/**
 * 輝夜姬的卡牌效果處理
 * @param self 施法者
 * @param target 目標
 * @param type 卡牌類型
 * @param dmg 傷害值/等級
 */
void kaguya_effect(void *self, void *target, Type type, int32_t dmg);

/**
 * 愛麗絲的卡牌效果處理
 * @param self 施法者
 * @param target 目標
 * @param type 卡牌類型
 * @param dmg 傷害值/等級
 */
void alice_effect(void *self, void *target, Type type, int32_t dmg);

// 愛麗絲特殊函數宣告

/**
 * 愛麗絲身份切換函數
 * @param player 愛麗絲玩家指標
 * @param new_identity 新身份
 */
void switch_alice_identity(Player *player, AliceIdentity new_identity);

/**
 * 根據愛麗絲當前身份調整卡牌數值
 * @param player 愛麗絲玩家指標
 * @param card_type 卡牌類型
 * @param base_value 基礎數值
 * @return 調整後的數值
 */
int32_t get_modified_value(Player *player, Type card_type, int32_t base_value);

// 花木蘭特殊函數宣告

/**
 * 花木蘭的防禦反應（被攻擊時觸發）
 * @param mulan_player 花木蘭玩家指標
 * @param attacker 攻擊者
 * @param incoming_damage 即將受到的傷害
 */
void mulan_defensive_reaction(Player *mulan_player, Player *attacker, int incoming_damage);

// 通用輔助函數宣告

/**
 * 檢查目標是否在戰場邊緣
 * @param target 目標玩家
 * @return 如果在邊緣返回true，否則返回false
 */
bool is_at_edge(Player *target);

/**
 * 隨機棄掉目標玩家的一張手牌
 * @param target 目標玩家
 */
void discard_random_card(Player *target);

/**
 * 將攻擊者移動到目標相鄰的位置
 * @param attacker 攻擊者
 * @param target 目標
 */
void move_adjacent_to_target(Player *attacker, Player *target);

// 遊戲狀態管理函數宣告

/**
 * 初始化遊戲
 */
void init_game(void);

/**
 * 清理遊戲資源
 */
void cleanup_game(void);

/**
 * 檢查遊戲是否結束
 * @param player1 玩家1
 * @param player2 玩家2
 * @return 如果遊戲結束返回true
 */
bool is_game_over(Player *player1, Player *player2);

// 調試和測試函數宣告

/**
 * 顯示玩家狀態
 * @param player 玩家指標
 */
void display_player_status(Player *player);

/**
 * 顯示牌庫內容（用於調試）
 * @param deck 牌庫指標
 * @param deck_name 牌庫名稱
 */
void display_deck_contents(Deck *deck, const char *deck_name);

#endif // TWISTED_FABLES_H