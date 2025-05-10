#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<stdint.h>
#include<stdbool.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include<SDL2/SDL2_gfxPrimitives.h>
#define MAX_CARD 100
#define WCARD 100
#define HCARD 140
#define WSCREEN 1200
#define HSCREEN 800
#define HAND_SIZE 5

typedef enum{
    BASIC_ATK  //基礎攻擊牌
    BASIC_DEF  //基礎防禦牌
    BASIC_MOV  //基礎移動牌
    UNIVERSAL  //通用牌
    SKILL_ATK  //技能攻擊牌
    SKILL_DEF  //技能防禦牌
    SKILL_MOV  //技能移動牌
    EPIC  //必殺牌
    TWIST //蛻變牌、反轉牌
} Type;

typedef struct{
    Type type;
    int32_t cst;        // energy cost
    int32_t dmg;        // for attack
    int32_t blk;        // for defense
    int32_t mov;        // lane movement
    int32_t rng;        // 0 = melee , 1 = adjacent, 2 = global
    void (*effect)(void *self, void *target, Type type, int32_t dmg);
} Card;

typedef struct{
    Card *cards[MAX_CARD];
    int32_t cnt;        //cnt
} Deck;

typedef struct{
    char name[50];
    int32_t health;
    int32_t energy;
    int32_t blk;
    int32_t epic_threshold;
    int32_t lane;       //0, 1, or 2
} Fable;

typedef struct{
    Fable *fable;
    Deck draw;          //牌庫（抽牌區）
    Deck disc;          //棄牌區
    Deck bani;          //出牌區
    Deck hand;          //手牌
    Deck twist;         //反轉牌區
    Deck epic;          //必殺牌
} Player;

//-------------------- 機制 --------------------
void add_deck(Deck *deck, Card *card){
    if(deck->cnt<MAX_CARD)deck->cards[deck->cnt++]=card;
    else return NULL;
}
Card *draw_deck(Deck *deck){
    if(deck->cnt==0)return NULL;
    return deck->cards[--deck->cnt];
}
void shuffle_deck(Deck *deck){
    for(int i=deck->cnt-1;i>0;i--){
        int32_t j=rand()%(i+1);
        Card *tmp=deck->cards[i];
        deck->cards[i]=deck->cards[j];
        deck->cards[j]=tmp;
    }
}
void draw_hand(Player *player){
    for(int i=0;i<HAND_SIZE;i++){
        Card *card=draw_deck(&player->draw);
        if(card)add_deck(&player->hand,card);
    }
}

//-------------------- 卡牌效果 --------------------
__effect(void *self, void *target, Type type, int32_t dmg){ //底線處請填入角色名，例如小紅帽請填入 redhood_effect
    Player *atker = (Player *)self;   // 將 self 轉型為 Player 指標
    Player *defer = (Player *)target; // 將 target 轉型為 Player 指標

    //更多卡牌效果請自行填入
    //for example, please change to your version
    Card card;
    if(card.type == 4 || card.dmg==1){ //represent that this card is 弱點射擊，following code should execute this card's effect
        ...
    }
}


//-------------------- 角色 --------------------
void setup_character(Player *p){//character should be changed to the name of character that you are in charge of
    Card card1={ //cardi should be changed to the skill name that you are in charge of
        .type=,
        .cst=,
        .dmg=,
        .blk=,
        .mov=,
        .rng=,
       .effect=__effect
    }

    Card card2={

    }

    .
    .
    .

    Fable *character=malloc(sizeof(Fable)); //character should be changed to the name of character that you are in charge of

    *character=(Fable){
        .health=,
        .energy=,
	.blk=,
	.epic_threshold=,
        .lane=  //4 or 6
    };
    strcpy(red->name,"character");//character should be changed to the name of character that you are in charge of
    p->fable=character;
    //remeber that some card need to add twice、three times or more also the cards are in order
    add_deck(&p->draw,&card1);
    add_deck(&p->draw,&card2);
    .
    .
    .
    shuffle_deck(&p->draw);
    draw_hand(p);
}