#include "fables.h"
#include "base.h"
#include "declare.h"
//#include "draw.h" 原本有東西但現在是空的所以comment掉
#include "cards.h"
typedef struct {
    const char *txt;
    int32_t x,y;
    int32_t a;
    bool activate;
} Floating;

bool Playing = 0;

//-------------------- 介面 --------------------
void grid(SDL_Renderer *ren){
    SDL_SetRenderDrawColor(ren, 30, 30, 60, 255);
    for (int x = 0; x <= WSCREEN; x += 120) {
        SDL_RenderDrawLine(ren, x, 0, x, HSCREEN);
    }
    for (int y = 0; y <= HSCREEN; y += 120) {
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
                if(mx>=XCENTER-200&&mx<=XCENTER+200&&my>=YCENTER+60&&my<=YCENTER+160){
                    intitle=0;
                }
            }
        }
        SDL_SetRenderDrawColor(ren,255,255,255,255);
        SDL_RenderClear(ren);
        grid(ren);
        SDL_Rect box={XCENTER,YCENTER+110,400,100};
        DrawButton(ren,box,r,btnclr,font,txtclr,"Play 1v1");

        //battle_setup(NULL,NULL,ren);
        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }
}
int randomize_round(SDL_Renderer *ren, TTF_Font *font) {
    int dice_roll = rand() % 6 + 1; // Simulate a dice roll (1-6)
    SDL_Color txtclr = {0, 0, 0, 255};
    char message[50];

    if (dice_roll % 2 == 1) {
        snprintf(message, sizeof(message), "P1 goes first! (Dice: %d)", dice_roll);
        WriteText(ren, message, font, txtclr, WSCREEN / 2, HSCREEN / 2 - 50);
        return 1; // P1 goes first
    } else {
        snprintf(message, sizeof(message), "P2 goes first! (Dice: %d)", dice_roll);
        WriteText(ren, message, font, txtclr, WSCREEN / 2, HSCREEN / 2 - 50);
        return 2; // P2 goes first
    }
}
void BattleScreen(SDL_Renderer *ren,Player *p1,Player *p2,TTF_Font *font){
    bool inbattle=1;
    bool fighting=1;
    SDL_Event e;

    // Randomize who goes first
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    SDL_RenderClear(ren);
    grid(ren);
    int first_player = randomize_round(ren, font);
    SDL_RenderPresent(ren);
    SDL_Delay(2000); // Pause to show the result
    battle_setup(p1,p2,1);
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
        grid(ren);
        BattleLane(ren,p1,p2);
        //printf("%d,%d\n",p1->draw.cnt,p1->hand.cnt);
        for(int i=0;i<p1->hand.cnt;i++){
            SDL_Rect rect={50+i*(WCARD+20),HSCREEN-HCARD+40,WCARD,HCARD};
            SDL_SetRenderDrawColor(ren,200,0,0,255);
            SDL_RenderFillRect(ren,&rect);
            SDL_SetRenderDrawColor(ren,0,0,0,255);
            SDL_RenderDrawRect(ren,&rect);
        }
        
        SDL_RenderPresent(ren);
        SDL_Delay(15);
    }
}
void BattleLane(SDL_Renderer *ren,Player *p1, Player *p2){
    for(int i=-4;i<=4;i++){
        SDL_Rect rect={XCENTER+i*(WCARD+20),YCENTER,WCARD,HCARD};
        SDL_Color relic={0,0,0,255};
        DrawRect(ren,rect,10,relic);
    }
    filledCircleRGBA(ren,XCENTER-(WCARD+20),YCENTER,40,p1->fable->Piece.r,p1->fable->Piece.g,p1->fable->Piece.b,p1->fable->Piece.a);
}
//-------------------- Set up-s --------------------
Deck general_shop={
    .cards={&Attack1,&Defense1,&Move1,&Wild1,&Attack2,&Defense2,&Move2,&Attack3,&Defense3,&Move3},
    .cnt=10
};
void fable_shop_setup(Fable *fable){
    FableShop shop={
        .Attack={
            .cards=memcpy(malloc(sizeof(Card*)*5),fable->skill[0].cards,sizeof(Card*)*5),
            .cnt=5
        },
        .Defense={
            .cards=memcpy(malloc(sizeof(Card*)*5),fable->skill[1].cards,sizeof(Card*)*5),
            .cnt=5
        },
        .Move={
            .cards=memcpy(malloc(sizeof(Card*)*5),fable->skill[2].cards,sizeof(Card*)*5),
            .cnt=5
        }
    };
}
void starting_deck(Player *p){
    for(int i=0;i<3;i++){
        add_deck(&p->draw,&Attack1);
        add_deck(&p->draw,&Defense1);
        add_deck(&p->draw,&Move1);
    }
    add_deck(&p->draw,p->fable->skill[0].cards[0]); // Pot Shot
    add_deck(&p->draw,p->fable->skill[1].cards[0]); // Energy Shield
    add_deck(&p->draw,p->fable->skill[2].cards[0]); // Unleashed Ballistics
    printf("%d\n",p->draw.cnt);
    shuffle_deck(&p->draw);   
}
void setup_player(Player *p, Fable *fable) {
    p->fable = fable;
}
void battle_setup(Player *p1, Player *p2,int starting){
    setup_player(p1, &red_hood);
    starting_deck(p1);
    //setup_player(p2, &red_hood);
    //strcpy(p2->fable->name, "Dummy");
    //filledCircleRGBA(ren,XCENTER+(WCARD+20),YCENTER,40,p2->fable->Piece.r,p2->fable->Piece.g,p2->fable->Piece.b,p2->fable->Piece.a);
    if(starting==1){
        draw_hand(p1,4);
        printf("P1's hand: %d cards\n", p1->hand.cnt);
        /*for(int i=0;i<6;i++){
            draw_hand(p2);
        }*/
    }/*else if(starting==2){
        for(int i=0;i<6;i++){
            draw_hand(p1);
        }
        for(int i=0;i<4;i++){
            draw_hand(p2);
        }
    }*/
}

//-------------------- 主程式 --------------------
int main() {
    srand(time(NULL));
    Playing = 1;
    Player p1 = {0}, p2 = {0};

    // Font Initialization
    TTF_Init();
    TTF_Font *font = TTF_OpenFont("NotoSans-Regular.ttf", 30);
    if(!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return 1;
    }

    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("Twisted Fables",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WSCREEN, HSCREEN, 0);
    if(!win) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if(!ren) {
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    // Main game loop
    while(Playing) {
        TitleScreen(ren, font);
        BattleScreen(ren, &p1, &p2, font);
    }

    // Cleanup
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    free(p1.fable);
    free(p2.fable);
    return 0;
}
