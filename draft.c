#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<stdint.h>
#include<stdbool.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include<SDL2/SDL2_gfxPrimitives.h>
#define MAX_CARD 50
#define WCARD 100
#define HCARD 140
#define WSCREEN 1200
#define HSCREEN 800
#define HAND_SIZE 5
typedef enum{
    CARD_ATK,           //ATK = attack
    CARD_DEF,           //DEF = defense
    CARD_TEC,           //TEC = tech
    CARD_SPE            //SPE = special
} Type;
typedef struct{
    char name[50];
    Type type;
    int32_t cst;        // energy cost
    int32_t dmg;        // for attack
    int32_t blk;        // for defense
    int32_t mov;        // lane movement
    int32_t rng;        // 0 = melee , 1 = adjacent, 2 = global
    void (*effect)(void *self,void *target);
} Card;
typedef struct{
    Card *cards[MAX_CARD];
    int32_t cnt;        //cnt
} Deck;
typedef struct{
    char name[50];
    int32_t health;
    int32_t energy;
    int32_t lane;       //0, 1, or 2
    Deck base;
    Deck skilltree1;
    Deck skilltree2;
    Deck skilltree3;
} Fable;
typedef struct{
    Fable *fable;
    Deck draw;          //抽牌
    Deck disc;          //墓地
    Deck bani;          //除外
    Deck hand;          //手牌
} Player;

typedef struct{
    const char *txt;
    int32_t x,y;
    int32_t a;
    bool activate;
} Floating;

bool Playing=0;

//-------------------- 介面 --------------------
void grid(SDL_Renderer *ren){
    SDL_SetRenderDrawColor(ren, 30, 30, 60, 255);
    for (int x = 0; x <= WSCREEN; x += 100) {
        SDL_RenderDrawLine(ren, x, 0, x, HSCREEN);
    }
    for (int y = 0; y <= HSCREEN; y += 100) {
        SDL_RenderDrawLine(ren, 0, y, WSCREEN, y);
    }
}
void WriteText(SDL_Renderer *ren,const char *text,TTF_Font *font,SDL_Color color,int32_t x,int32_t y){
    SDL_Surface *sur=TTF_RenderText_Solid(font,text,color);
    if(!sur){
        printf("Error creating surfaces: %s\n",SDL_GetError());
        return;
    }
    SDL_Texture *txture=SDL_CreateTextureFromSurface(ren,sur);
    if(!txture){
        printf("Error creating texture: %s\n",SDL_GetError());
        SDL_FreeSurface(sur);
        return;
    }
    int32_t wtext=sur->w, htext=sur->h;
    SDL_FreeSurface(sur);
    SDL_Rect dest={x-wtext/2,y-htext/2,wtext,htext};
    SDL_RenderCopy(ren,txture,NULL,&dest);
    SDL_DestroyTexture(txture);
}
void DrawRect(SDL_Renderer *ren,SDL_Rect rect,int32_t r,SDL_Color color){
    SDL_Rect cen={rect.x-rect.w/2,rect.y-rect.h/2,rect.w,rect.h};
    roundedBoxRGBA(ren,cen.x,cen.y,cen.x+cen.w,cen.y+cen.h,r,color.r,color.b,color.g,color.a);
}
void DrawButton(SDL_Renderer *ren,SDL_Rect rect,int32_t r,SDL_Color color,TTF_Font *font,SDL_Color txtclr,const char *line){
    DrawRect(ren,rect,r,color);
    WriteText(ren,line,font,txtclr,rect.x,rect.y);
}
void TitleScreen(SDL_Renderer *ren,TTF_Font *font){
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
                if(mx>=400&&mx<=800&&my>=550&&my<=650){
                    intitle=0;
                }
            }
        }
        SDL_SetRenderDrawColor(ren,255,255,255,255);
        SDL_RenderClear(ren);
        grid(ren);
        SDL_Rect box={600,600,400,100};
        DrawButton(ren,box,r,btnclr,font,txtclr,"Play 1v1");

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }
}
void BattleScreen(SDL_Renderer *ren,Player *p1,Player *p2,TTF_Font *font){
    bool inbattle=1;
    bool fighting=1;
    SDL_Event e;
    while(inbattle){
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT){
                inbattle=0;
            }
            if(e.type==SDL_MOUSEBUTTONDOWN&&e.button.button==SDL_BUTTON_LEFT){
                int32_t mx=e.button.x, my=e.button.y;
                for(int i=0;i<p1->hand.cnt;i++){
                    int32_t x=50+i*(WCARD+20), y=HSCREEN-HCARD-40;
                    SDL_Rect rect={x,y,WCARD,HCARD};
                    if(SDL_PointInRect(&(SDL_Point){mx,my},&rect)){
                        printf("Playing %s...\n",p1->hand.cards[i]->name);
                        if(p1->hand.cards[i]->effect){
                            p1->hand.cards[i]->effect(p1,p2);
                        }
                    }
                }
                for(int i=0;i<p2->hand.cnt;i++){
                    int32_t x=WSCREEN-(50+i*(WCARD+20)), y=HCARD+40;
                    SDL_Rect rect={x,y,WCARD,HCARD};
                    if(SDL_PointInRect(&(SDL_Point){mx,my},&rect)){
                        printf("Playing %s...\n",p2->hand.cards[i]->name);
                        if(p2->hand.cards[i]->effect){
                            p1->hand.cards[i]->effect(p1,p2);
                        }
                    }
                }
            }
            if(e.type==SDL_MOUSEBUTTONDOWN&&fighting==0){
                inbattle=0;
            }
        }
        SDL_SetRenderDrawColor(ren,255,255,255,255);
        SDL_RenderClear(ren);
        for(int i=0;i<p1->hand.cnt;i++){
            SDL_Rect rect={50+i*(WCARD+20),HSCREEN-HCARD+40,WCARD,HCARD};
            SDL_SetRenderDrawColor(ren,200,0,0,255);
            SDL_RenderFillRect(ren,&rect);
            SDL_SetRenderDrawColor(ren,0,0,0,255);
            SDL_RenderDrawRect(ren,&rect);
        }
        grid(ren);
        SDL_Rect hp1={50,(HCARD-40)/2,p2->fable->health*4,20};
        SDL_Rect hp2={WSCREEN-(p1->fable->health*4+50),HSCREEN-(HCARD-40)/2,p1->fable->health*4,20};
        SDL_SetRenderDrawColor(ren,0,200,0,255);
        SDL_RenderFillRect(ren,&hp1);
        SDL_RenderFillRect(ren,&hp2);
        SDL_Color txtclr={0,0,0,255};
        
        if(p1->fable->health==0&&p2->fable->health==0){
            WriteText(ren,"Draw",font,txtclr,WSCREEN/2,HSCREEN/2);
            fighting=0;
        }else if(p1->fable->health==0){
            WriteText(ren,"P2 wins",font,txtclr,WSCREEN/2,HSCREEN/2);
            fighting=0;
        }else if(p2->fable->health==0){
            WriteText(ren,"P1 wins",font,txtclr,WSCREEN/2,HSCREEN/2);
            fighting=0;
        }
        SDL_RenderPresent(ren);
        SDL_Delay(15);
    }
}
//-------------------- 機制 --------------------
void add_deck(Deck *deck, Card *card){
    if(deck->cnt<MAX_CARD)deck->cards[deck->cnt++]=card;
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
//-------------------- 效果 --------------------
void redhood_effect(void *self, void *target){
    Player *atker=(Player *)self;
    Player *defer=(Player *)target;
    if(atker->fable->lane==defer->fable->lane){
        defer->fable->health-=4;
        printf("[%s hits %s with Red Slash for 4 damage.]\n",atker->fable->name,defer->fable->name);
    }else{
        printf("[%s's Red Slash missed.\n]",atker->fable->name);
    }
}
//-------------------- 角色 --------------------
Card red_hood={
    .name="Red Riding Hood",
    .type=CARD_ATK,
    .cst=1,
    .dmg=4,
    .blk=0,
    .mov=0,
    .rng=0,
    .effect=redhood_effect
};
void setup_red(Player *p){
    Fable *red=malloc(sizeof(Fable));
    *red=(Fable){
        .health=40,
        .energy=2,
        .lane=1
    };
    strcpy(red->name,"Red Riding Hood");
    p->fable=red;
    add_deck(&p->draw,&red_hood);
    add_deck(&p->draw,&red_hood);
    add_deck(&p->draw,&red_hood);
    add_deck(&p->draw,&red_hood);
    add_deck(&p->draw,&red_hood);
    shuffle_deck(&p->draw);
    draw_hand(p);
}
//-------------------- 模擬 --------------------
int main(){
    srand(time(NULL));
    Playing=1;
    Player p1={0}, p2={0};

    // Font Initialization
    TTF_Init();
    TTF_Font *font=TTF_OpenFont("NotoSans-Regular.ttf",30);
    if(!font){
        printf("Failed to load font: %s\n",TTF_GetError());
        return 1;
    }
    //-----------------------------
    setup_red(&p1); setup_red(&p2);
    strcpy(p2.fable->name,"Dummy");
    p2.fable->lane=1;

    if(SDL_Init(SDL_INIT_VIDEO)!=0){
        printf("SDL_Init Error: %s\n",SDL_GetError());
        return 1;
    }
    SDL_Window *win=SDL_CreateWindow("Twisted Fables",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WSCREEN,HSCREEN,0);
    if(!win){
        printf("SDL_CreateWindow Error: %s\n",SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Renderer *ren=SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED);
    if(!ren){
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }
    while(Playing){
        TitleScreen(ren,font);
        BattleScreen(ren,&p1,&p2,font);
    }
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    free(p1.fable);
    free(p2.fable);
    return 0;
}