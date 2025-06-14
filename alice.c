#include "alice.h"
#include "cards.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// 愛麗絲狀態管理
AliceState alice_state = {0};

// --- 效果函數宣告 ---
void open_game_effect(void *self, void *target);
void twist_game_effect(void *self, void *target);
void control_game_effect(void *self, void *target);
void magic_trick_effect(void *self, void *target);
void mental_magic_effect(void *self, void *target);
void hat_trick_effect(void *self, void *target);
void strange_agility_effect(void *self, void *target);
void strange_stealth_effect(void *self, void *target);
void strange_strangeness_effect(void *self, void *target);

// 蛻變效果函數
void off_with_her_head_effect(void *self, void *target);
void start_my_show_effect(void *self, void *target);
void wonderland_comes_effect(void *self, void *target);
void we_are_all_mad_effect(void *self, void *target);

// 必殺技效果函數
void endless_party_effect(void *self, void *target);
void wonderful_day_effect(void *self, void *target);
void game_control_effect(void *self, void *target);

// ------------------- 攻擊技能 -------------------

Card open_game = {
    .name = "Open Game", .type = SKILL_ATK, .cst = 0, .dmg = 1, .defense = 0,
    .mov = 0, .rng = 1, .link = false, .effect = open_game_effect
};

Card twist_game = {
    .name = "Twist Game", .type = SKILL_ATK, .cst = 2, .dmg = 2, .defense = 0,
    .mov = 0, .rng = 2, .link = false, .effect = twist_game_effect
};

Card control_game = {
    .name = "Control Game", .type = SKILL_ATK, .cst = 4, .dmg = 3, .defense = 0,
    .mov = 0, .rng = 3, .link = false, .effect = control_game_effect
};

// ------------------- 防禦技能 -------------------

Card magic_trick = {
    .name = "Magic Trick", .type = SKILL_DEF, .cst = 0, .dmg = 0, .defense = 1,
    .mov = 0, .rng = 0, .link = false, .effect = magic_trick_effect
};

Card mental_magic = {
    .name = "Mental Magic", .type = SKILL_DEF, .cst = 2, .dmg = 0, .defense = 2,
    .mov = 0, .rng = 0, .link = false, .effect = mental_magic_effect
};

Card hat_trick = {
    .name = "Hat Trick", .type = SKILL_DEF, .cst = 4, .dmg = 0, .defense = 3,
    .mov = 0, .rng = 0, .link = false, .effect = hat_trick_effect
};

// ------------------- 移動技能 -------------------

Card strange_agility = {
    .name = "Strange Agility", .type = SKILL_MOV, .cst = 0, .dmg = 0, .defense = 0,
    .mov = 1, .rng = 0, .link = false, .effect = strange_agility_effect
};

Card strange_stealth = {
    .name = "Strange Stealth", .type = SKILL_MOV, .cst = 2, .dmg = 0, .defense = 0,
    .mov = 2, .rng = 0, .link = false, .effect = strange_stealth_effect
};

Card strange_strangeness = {
    .name = "Strange Strangeness", .type = SKILL_MOV, .cst = 4, .dmg = 0, .defense = 0,
    .mov = 3, .rng = 0, .link = false, .effect = strange_strangeness_effect
};

// ------------------- TWIST 蛻變卡 -------------------

Card off_with_her_head = {
    .name = "Off With Her Head", .type = TWIST, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = off_with_her_head_effect
};

Card start_my_show = {
    .name = "Start My Show", .type = TWIST, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = start_my_show_effect
};

Card wonderland_comes = {
    .name = "Wonderland Comes", .type = TWIST, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = wonderland_comes_effect
};

Card we_are_all_mad = {
    .name = "We Are All Mad", .type = TWIST, .cst = 0, .dmg = 0,
    .defense = 0, .mov = 0, .rng = 0, .link = false, .effect = we_are_all_mad_effect
};

// ------------------- EPIC 必殺卡 -------------------

Card endless_party = {
    .name = "Endless Party", .type = EPIC, .cst = 0, .dmg = 0, .defense = 0,
    .mov = 0, .rng = 0, .link = false, .effect = endless_party_effect
};

Card wonderful_day = {
    .name = "Wonderful Day", .type = EPIC, .cst = 0, .dmg = 0, .defense = 0,
    .mov = 0, .rng = 0, .link = false, .effect = wonderful_day_effect
};

Card game_control = {
    .name = "Game Control", .type = EPIC, .cst = 0, .dmg = 0, .defense = 0,
    .mov = 0, .rng = 3, .link = false, .effect = game_control_effect
};

// ------------------- Fable 初始化 -------------------

Fable alice;

void init_alice_fable(void) {
    alice = (Fable){
        .name = "Alice",
        .Piece = {135, 206, 235, 255}, // 天藍色
        .health = 32,
        .energy = 0,
        .defense = 6,
        .epic_threshold = 16,
        .lane = 1
    };
    
    // 攻擊技能牌庫
    alice.skill[0] = (Deck){
        .cards = { &open_game, &twist_game, &twist_game, &control_game, &control_game },
        .cnt = 5
    };
    
    // 防禦技能牌庫
    alice.skill[1] = (Deck){
        .cards = { &magic_trick, &mental_magic, &mental_magic, &hat_trick, &hat_trick },
        .cnt = 5
    };
    
    // 移動技能牌庫
    alice.skill[2] = (Deck){
        .cards = { &strange_agility, &strange_stealth, &strange_stealth, &strange_strangeness, &strange_strangeness },
        .cnt = 5
    };
    
    // 必殺技
    alice.epic[0] = endless_party;
    alice.epic[1] = wonderful_day;
    alice.epic[2] = game_control;
}

// ------------------- 愛麗絲狀態管理 -------------------

void init_alice_state(Player *p) {
    alice_state.player = p;
    alice_state.current_identity = IDENTITY_NONE;
    alice_state.available_identities[0] = IDENTITY_QUEEN_OF_HEARTS;
    alice_state.available_identities[1] = IDENTITY_MAD_HATTER;
    alice_state.available_identities[2] = IDENTITY_CHESHIRE_CAT;
    alice_state.identity_count = 3;
    alice_state.first_turn_identity_selected = false;
    alice_state.identity_changed_this_turn = false;
    alice_state.show_performance_stacks = 0;
    alice_state.endless_party_uses = 0;
    alice_state.wonderland_day_active = false;
    
    printf("🎭 Alice initialized with 3 identity options.\n");
}

void alice_change_identity(AliceIdentity new_identity) {
    if (alice_can_change_identity(new_identity)) {
        alice_state.current_identity = new_identity;
        alice_state.identity_changed_this_turn = true;
        
        const char* identity_names[] = {"None", "Queen of Hearts", "Mad Hatter", "Cheshire Cat"};
        printf("🎭 Alice transforms into %s!\n", identity_names[new_identity]);
    }
}

void alice_reset_identity_change_flag(void) {
    alice_state.identity_changed_this_turn = false;
}

bool alice_can_change_identity(AliceIdentity target_identity) {
    // 不能變為當前身份
    if (target_identity == alice_state.current_identity) {
        return false;
    }
    
    // 第一回合可以選擇任何身份
    if (!alice_state.first_turn_identity_selected) {
        return true;
    }
    
    // 其他回合必須變為不同身份
    return target_identity != alice_state.current_identity;
}

// ------------------- 身份加成效果 -------------------

int alice_get_attack_modifier(void) {
    switch (alice_state.current_identity) {
        case IDENTITY_QUEEN_OF_HEARTS:
            return 1;  // 攻擊牌攻擊值+1
        case IDENTITY_MAD_HATTER:
            return -1; // 攻擊牌攻擊值-1
        case IDENTITY_CHESHIRE_CAT:
            return 0;  // 無修正
        default:
            return 0;
    }
}

int alice_get_defense_modifier(void) {
    switch (alice_state.current_identity) {
        case IDENTITY_QUEEN_OF_HEARTS:
            return 0;  // 無修正
        case IDENTITY_MAD_HATTER:
            return 1;  // 防禦牌防禦值+1
        case IDENTITY_CHESHIRE_CAT:
            return -1; // 防禦牌防禦值-1
        default:
            return 0;
    }
}

int alice_get_movement_modifier(void) {
    switch (alice_state.current_identity) {
        case IDENTITY_QUEEN_OF_HEARTS:
            return -1; // 移動牌移動值-1
        case IDENTITY_MAD_HATTER:
            return 0;  // 無修正
        case IDENTITY_CHESHIRE_CAT:
            return 1;  // 移動牌移動值+1
        default:
            return 0;
    }
}

// ------------------- 攻擊技能效果函數 -------------------

void open_game_effect(void *self, void *target) {
    Player *alice_player = (Player *)self;
    Player *enemy = (Player *)target;
    
    printf("🃏 Alice opens the game!\n");
    
    // 對手展示3張基本牌的邏輯需要在遊戲主循環中實作
    // 這裡我們簡化處理
    
    // 可以變為紅心皇后
    if (alice_can_change_identity(IDENTITY_QUEEN_OF_HEARTS)) {
        printf("🃏 Alice can become Queen of Hearts!\n");
        // 在實際遊戲中，這應該是玩家的選擇
    }
}

void twist_game_effect(void *self, void *target) {
    Player *alice_player = (Player *)self;
    Player *enemy = (Player *)target;
    
    printf("🃏 Alice twists the game!\n");
    
    // 對手展示4張基本牌的邏輯
    
    // 可以變為紅心皇后
    if (alice_can_change_identity(IDENTITY_QUEEN_OF_HEARTS)) {
        printf("🃏 Alice can become Queen of Hearts!\n");
    }
}

void control_game_effect(void *self, void *target) {
    Player *alice_player = (Player *)self;
    Player *enemy = (Player *)target;
    
    printf("🃏 Alice controls the game!\n");
    
    // 對手展示5張基本牌的邏輯
    
    // 可以變為紅心皇后
    if (alice_can_change_identity(IDENTITY_QUEEN_OF_HEARTS)) {
        printf("🃏 Alice can become Queen of Hearts!\n");
    }
}

// ------------------- 防禦技能效果函數 -------------------

void magic_trick_effect(void *self, void *target) {
    Player *alice_player = (Player *)self;
    
    printf("🎩 Alice performs a magic trick!\n");
    
    // 可以從手牌、棄牌堆移除1張基本牌的邏輯需要在UI中實作
    
    // 可以變為瘋帽子
    if (alice_can_change_identity(IDENTITY_MAD_HATTER)) {
        printf("🎩 Alice can become Mad Hatter!\n");
    }
}

void mental_magic_effect(void *self, void *target) {
    Player *alice_player = (Player *)self;
    
    printf("🎩 Alice performs mental magic!\n");
    
    // 可以變為瘋帽子
    if (alice_can_change_identity(IDENTITY_MAD_HATTER)) {
        printf("🎩 Alice can become Mad Hatter!\n");
    }
}

void hat_trick_effect(void *self, void *target) {
    Player *alice_player = (Player *)self;
    
    printf("🎩 Alice performs the ultimate hat trick!\n");
    
    // 可以變為瘋帽子
    if (alice_can_change_identity(IDENTITY_MAD_HATTER)) {
        printf("🎩 Alice can become Mad Hatter!\n");
    }
}

// ------------------- 移動技能效果函數 -------------------

void strange_agility_effect(void *self, void *target) {
    Player *alice_player = (Player *)self;
    Player *enemy = (Player *)target;
    
    printf("😸 Alice moves with strange agility!\n");
    
    // 移動邏輯需要在主遊戲循環中處理
    // 穿過對手時抽1張牌
    
    // 可以變為柴郡貓
    if (alice_can_change_identity(IDENTITY_CHESHIRE_CAT)) {
        printf("😸 Alice can become Cheshire Cat!\n");
    }
}

void strange_stealth_effect(void *self, void *target) {
    Player *alice_player = (Player *)self;
    Player *enemy = (Player *)target;
    
    printf("😸 Alice moves with strange stealth!\n");
    
    // 穿過對手時抽2張牌
    
    // 可以變為柴郡貓
    if (alice_can_change_identity(IDENTITY_CHESHIRE_CAT)) {
        printf("😸 Alice can become Cheshire Cat!\n");
    }
}

void strange_strangeness_effect(void *self, void *target) {
    Player *alice_player = (Player *)self;
    Player *enemy = (Player *)target;
    
    printf("😸 Alice moves with strange strangeness!\n");
    
    // 穿過對手時抽3張牌
    
    // 可以變為柴郡貓
    if (alice_can_change_identity(IDENTITY_CHESHIRE_CAT)) {
        printf("😸 Alice can become Cheshire Cat!\n");
    }
}

// ------------------- 蛻變效果函數 -------------------

void off_with_her_head_effect(void *self, void *target) {
    printf("👑 Off with her head! (Queen of Hearts ability active)\n");
    // 當身份為紅心皇后時，攻擊技能將牌放入棄牌堆時可以加入手牌
}

void start_my_show_effect(void *self, void *target) {
    alice_state.show_performance_stacks++;
    printf("🎭 Start my show! (Stacks: %d)\n", alice_state.show_performance_stacks);
    // 回合結束階段額外抽牌
}

void wonderland_comes_effect(void *self, void *target) {
    printf("🌟 Wonderland comes! (Mad Hatter ability active)\n");
    // 當身份為瘋帽子時，防禦技能將牌放入棄牌堆時可以選擇加入手牌
}

void we_are_all_mad_effect(void *self, void *target) {
    printf("😸 We are all mad here! (Cheshire Cat ability active)\n");
    // 當身份為柴郡貓時，穿過對手或被穿過時可以抽牌
}

// ------------------- 必殺技效果函數 -------------------

void endless_party_effect(void *self, void *target) {
    Player *alice_player = (Player *)self;
    
    if (alice_state.endless_party_uses >= 3) {
        printf("🎉 Endless party limit reached!\n");
        return;
    }
    
    alice_state.endless_party_uses++;
    printf("🎉 Endless party! (Uses: %d/3)\n", alice_state.endless_party_uses);
    
    // 回合結束階段抽牌後，棄牌至4張，立即開始新回合
    // 這個效果需要在主遊戲邏輯中處理
}

void wonderful_day_effect(void *self, void *target) {
    alice_state.wonderland_day_active = true;
    printf("✨ What a wonderful day! All card values +1 this turn!\n");
    
    // 直到本回合結束前，攻擊牌、防禦牌、移動牌的值+1
    // 這個效果需要在卡牌使用時檢查
}

void game_control_effect(void *self, void *target) {
    Player *alice_player = (Player *)self;
    Player *enemy = (Player *)target;
    
    printf("🎮 Game in complete control!\n");
    
    // 將手牌和/或棄牌堆中的至多5張基本牌洗入對手牌庫
    // 這個效果需要UI支援選擇卡牌
}