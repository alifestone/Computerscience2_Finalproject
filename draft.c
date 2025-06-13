#include "base.h"
#include "red_hood.h"
#include "cards.h"
typedef struct {
    const char *txt;
    int32_t x,y;
    int32_t a;
    bool activate;
} Floating;

TTF_Font *font = NULL;
bool Playing = 0;

//-------------------- 介面 --------------------
void WriteText(SDL_Renderer *ren, const char *text, SDL_Color color, int32_t x, int32_t y, int font_size) {
    TTF_Init();
    if (!font) {
        font = TTF_OpenFont("NotoSans-Regular.ttf", font_size);
        if (!font) {
            printf("Error opening font: %s\n", TTF_GetError());
            return;
        }
    }

    char *text_copy = strdup(text);
    if (!text_copy) return;

    const int line_spacing = font_size + 10;
    int line_count = 0;

    char *line = strtok(text_copy, "\n");
    while (line) {
        SDL_Surface *sur = TTF_RenderText_Solid(font, line, color);
        if (!sur) {
            printf("Error creating surface: %s\n", SDL_GetError());
            free(text_copy);
            return;
        }

        SDL_Texture *txture = SDL_CreateTextureFromSurface(ren, sur);
        if (!txture) {
            printf("Error creating texture: %s\n", SDL_GetError());
            SDL_FreeSurface(sur);
            free(text_copy);
            return;
        }

        int32_t wtext = sur->w, htext = sur->h;
        SDL_FreeSurface(sur);

        SDL_Rect dest = { x - wtext / 2, y - htext / 2 + line_count * line_spacing, wtext, htext };
        SDL_RenderCopy(ren, txture, NULL, &dest);
        SDL_DestroyTexture(txture);

        line = strtok(NULL, "\n");
        line_count++;
    }

    free(text_copy);
}
void DrawRect(SDL_Renderer *ren,SDL_Rect rect,int32_t r,SDL_Color color){
    SDL_Rect cen={rect.x-rect.w/2,rect.y-rect.h/2,rect.w,rect.h};
    roundedBoxRGBA(ren,cen.x,cen.y,cen.x+cen.w,cen.y+cen.h,r,color.r,color.b,color.g,color.a);
}
void DrawButton(SDL_Renderer *ren,SDL_Rect rect,int32_t r,SDL_Color color,SDL_Color txtclr,const char *line){
    DrawRect(ren,rect,r,color);
    WriteText(ren,line,txtclr,rect.x,rect.y,30);
}
void TitleScreen(SDL_Renderer *ren){
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
        SDL_Rect box={XCENTER,YCENTER+110,400,100};
        DrawButton(ren,box,r,btnclr,txtclr,"Play 1v1");

        //battle_setup(NULL,NULL,ren);
        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }
}

int randomize_round(SDL_Renderer *ren) {
    int dice_roll = rand() % 6 + 1; // Simulate a dice roll (1-6)
    SDL_Color txtclr = {0, 0, 0, 255};
    char message[50];

    if (dice_roll % 2 == 1) {
        snprintf(message, sizeof(message), "P1 goes first! (Dice: %d)", dice_roll);
        WriteText(ren, message, txtclr, WSCREEN / 2, HSCREEN / 2 - 50,30);
        return 1; // P1 goes first
    } else {
        snprintf(message, sizeof(message), "P2 goes first! (Dice: %d)", dice_roll);
        WriteText(ren, message, txtclr, WSCREEN / 2, HSCREEN / 2 - 50,30);
        return 2; // P2 goes first
    }
}
void BattleScreen(SDL_Renderer *ren,Player *p1,Player *p2){
    bool inbattle=1;
    bool fighting=1;
    int32_t turn=0;
    memset(p1,0,sizeof(Player));
    memset(p2,0,sizeof(Player));
    SDL_Event e;

    // Randomize who goes first
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    SDL_RenderClear(ren);
    int first_player = randomize_round(ren);
    turn+=first_player;
    SDL_RenderPresent(ren);
    SDL_Delay(2000); // Pause to show the result
    battle_setup(p1,p2,first_player);
    int p1discardindex=0, p2discardindex=0;
    int change=0;
    while(inbattle){
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT){
                inbattle=0;
            }
            if(e.type==SDL_MOUSEBUTTONDOWN&&e.button.button==SDL_BUTTON_LEFT){
                int32_t mx=e.button.x, my=e.button.y;
                // End turn button
                SDL_Rect rect={WSCREEN-250, HSCREEN/2-50, 200, 100};
                
                if(SDL_PointInRect(&(SDL_Point){mx,my},&rect)){
                    printf("%d ",turn);
                    printf("Ending turn.\n");
                    turn++;
                    break;
                }
                // p1 hand
                for(int i=0;i<p1->hand.cnt;i++){
                    int32_t x=50+i*(WCARD+20), y=HSCREEN-HCARD-40;
                    SDL_Rect rect={x,y,WCARD,HCARD};
                    if(SDL_PointInRect(&(SDL_Point){mx,my},&rect)&&turn%2==1){
                        printf("p1 Playing\n");
                        if(p1->hand.cards[i]->effect){
                            p1->hand.cards[i]->effect(p1,p2);
                        }
                        p1->disc.cards[p1discardindex++]=p1->hand.cards[i];
                        for(int j=i;j<p1->hand.cnt-1;j++){
                            p1->hand.cards[j]=p1->hand.cards[j+1];
                        }
                        p1->hand.cnt--;
                    }
                }
                // p2 hand
                for(int i=0;i<p2->hand.cnt;i++){
                    int32_t x=WSCREEN-(50+(i+1)*(WCARD+20)), y=-40;
                    SDL_Rect rect={x,y,WCARD,HCARD};
                    if(SDL_PointInRect(&(SDL_Point){mx,my},&rect)&&turn%2==0){
                        printf("p2 Playing\n");
                        if(p2->hand.cards[i]->effect){
                            p2->hand.cards[i]->effect(p2,p1);
                        }
                        p2->disc.cards[p2discardindex++]=p2->hand.cards[i];
                        for(int j=i;j<p2->hand.cnt-1;j++){
                            p2->hand.cards[j]=p2->hand.cards[j+1];
                        }
                        p2->hand.cnt--;
                    }
                }
            }
            if(e.type==SDL_MOUSEBUTTONDOWN&&fighting==0){
                inbattle=0;
            }
        }

        SDL_SetRenderDrawColor(ren,255,255,255,255);
        SDL_RenderClear(ren);
        BattleLane(ren,p1,p2);
        // P1 hand
        for(int i=0;i<p1->hand.cnt;i++){
            SDL_Rect rect={50+i*(WCARD+20),HSCREEN-HCARD+40,WCARD,HCARD};
            SDL_SetRenderDrawColor(ren,200,0,0,255);
            SDL_RenderFillRect(ren,&rect);
            SDL_SetRenderDrawColor(ren,0,0,0,255);
            SDL_RenderDrawRect(ren,&rect);
            WriteText(ren,p1->hand.cards[i]->name,(SDL_Color){255,255,255,255},rect.x+WCARD/2,rect.y+HCARD/2,16);
        }
        // P1 health
        SDL_Rect p1_health_rect={WSCREEN-350, HSCREEN-100, p1->health*300/p1->fable->health, 30};
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        SDL_RenderFillRect(ren, &p1_health_rect);
        SDL_Rect p1_health_frame={WSCREEN-350, HSCREEN-100, 300, 30};
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderDrawRect(ren, &p1_health_frame);
        // P1 epic indicator
        SDL_Rect epic_p1={WSCREEN-350+(p1->fable->epic_threshold*300/p1->fable->health), HSCREEN-105, 20, 40};
        SDL_SetRenderDrawColor(ren, 129, 41, 144, 255);
        SDL_RenderFillRect(ren, &epic_p1);
        // P1 power
        SDL_Rect p1_power_rect={WSCREEN-350, HSCREEN-150, p1->power*12, 30};
        SDL_SetRenderDrawColor(ren, 124, 199, 232, 255);
        SDL_RenderFillRect(ren, &p1_power_rect);
        SDL_Rect p1_power_frame={WSCREEN-350, HSCREEN-150, 300, 30};
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderDrawRect(ren, &p1_power_frame);
        for(int x=1;x<=24;x++){
            SDL_RenderDrawLine(ren,WSCREEN-350+x*12,HSCREEN-150,WSCREEN-350+x*12,HSCREEN-120);
        }
        // P1 defense
        SDL_Rect p1_defense_limit={WSCREEN-350+(p1->fable->defense)*24, HSCREEN-200, (9-p1->fable->defense)*24, 30};
        SDL_SetRenderDrawColor(ren, 139, 0, 0, 255);
        SDL_RenderFillRect(ren, &p1_defense_limit);
        SDL_Rect p1_defense_rect={WSCREEN-350, HSCREEN-200, p1->defense*12/p1->fable->defense, 30};
        SDL_SetRenderDrawColor(ren, 255, 153, 19, 255);
        SDL_RenderFillRect(ren, &p1_defense_rect);
        SDL_Rect p1_defense_frame={WSCREEN-350, HSCREEN-200, 216, 30};
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderDrawRect(ren, &p1_defense_frame);
        for(int x=1;x<=8;x++){
            SDL_RenderDrawLine(ren,WSCREEN-350+x*24,HSCREEN-200,WSCREEN-350+x*24,HSCREEN-170);
        }
        

        // P2 hand
        for(int i=0;i<p2->hand.cnt;i++){
            SDL_Rect rect={WSCREEN-(50+(i+1)*(WCARD+20)),-40,WCARD,HCARD};
            SDL_SetRenderDrawColor(ren,0,0,200,255);
            SDL_RenderFillRect(ren,&rect);
            SDL_SetRenderDrawColor(ren,0,0,0,255);
            SDL_RenderDrawRect(ren,&rect);
            WriteText(ren,p2->hand.cards[i]->name,(SDL_Color){255,255,255,255},rect.x+WCARD/2,rect.y+HCARD/2,16);
        }
        // P2 health
        SDL_Rect p2_health_rect={50, 100, p2->health*300/p2->fable->health, 30};
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        SDL_RenderFillRect(ren, &p2_health_rect);
        SDL_Rect p2_health_frame={50, 100, 300, 30};
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderDrawRect(ren, &p2_health_frame);
        // P2 epic indicator
        SDL_Rect epic_p2={50+(p2->fable->epic_threshold*300/p2->fable->health), 95, 20, 40};
        SDL_SetRenderDrawColor(ren, 129, 41, 144, 255);
        SDL_RenderFillRect(ren, &epic_p2);
        // P2 power
        SDL_Rect p2_power_rect={50, 50, 300, 30};
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderDrawRect(ren, &p2_power_rect);
        for(int x=1;x<=24;x++){
            SDL_RenderDrawLine(ren,50+x*12,50,50+x*12,80);
        }
        SDL_Rect p2_power_frame={50, 50, p2->power*12, 30};
        SDL_SetRenderDrawColor(ren, 124, 199, 232, 255);
        SDL_RenderFillRect(ren, &p2_power_frame);
        // P2 defense
        SDL_Rect p2_defense_limit={50+(p2->fable->defense)*24, 0, (9-p2->fable->defense)*24, 30};
        SDL_SetRenderDrawColor(ren, 139, 0, 0, 255);
        SDL_RenderFillRect(ren, &p2_defense_limit);
        SDL_Rect p2_defense_rect={50, 20, p2->defense*12/p2->fable->defense, 30};
        SDL_SetRenderDrawColor(ren, 255, 153, 19, 255);
        SDL_RenderFillRect(ren, &p2_defense_rect);
        SDL_Rect p2_defense_frame={50, 0, 216, 30};
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderDrawRect(ren, &p2_defense_frame);
        for(int x=1;x<=8;x++){
            SDL_RenderDrawLine(ren,50+x*24,0,50+x*24,30);
        }

        // Turn indicator
        SDL_Color turn_color = {0, 0, 0, 255};
        if(turn % 2 == 1) {
            WriteText(ren, "P1's Turn", turn_color, 100, HSCREEN/2,30);
        } else {
            WriteText(ren, "P2's Turn", turn_color, 100, HSCREEN/2,30);
        }
        // Turn end button
        SDL_Rect end_turn_button={WSCREEN-150, HSCREEN/2, 200, 100};
        SDL_Color end_turn_color = {194, 194, 194, 255};
        DrawButton(ren, end_turn_button, 10, end_turn_color, (SDL_Color){0, 0, 0, 255}, "End Turn");
        
        SDL_RenderPresent(ren);
        SDL_Delay(15);
    }
}
void BattleLane(SDL_Renderer *ren,Player *p1, Player *p2){
    for(int i=-4;i<=4;i++){
        SDL_Rect rect={XCENTER+i*(WField+20),YCENTER,WField,HField};
        SDL_Color relic={0,0,0,255};
        DrawRect(ren,rect,10,relic);
    }
    filledCircleRGBA(ren,XCENTER-(WField+20),YCENTER,40,p1->fable->Piece.r,p1->fable->Piece.g,p1->fable->Piece.b,p1->fable->Piece.a);
    filledCircleRGBA(ren,XCENTER+(WField+20),YCENTER,40,p2->fable->Piece.r,p2->fable->Piece.g,p2->fable->Piece.b,p2->fable->Piece.a);
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
    shuffle_deck(&p->draw);   
}
void setup_player(Player *p, Fable *fable) {
    p->fable = fable;
    p->health = fable->health;
    p->power=0;
}
void battle_setup(Player *p1, Player *p2,int starting){
    setup_player(p1, &red_hood);
    starting_deck(p1);
    setup_player(p2, &red_hood);
    starting_deck(p2);
    printf("%d\n",starting);
    if(starting==1){
        draw_hand(p1,4);
        draw_hand(p2,6);
    }else if(starting==2){
        draw_hand(p1,6);
        draw_hand(p2,4);
    }
}

//-------------------- 主程式 --------------------
int main() {
    srand(time(NULL));
    Playing = 1;
    Player p1 = {0}, p2 = {0};

    // Font Initialization

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
        TitleScreen(ren);
        BattleScreen(ren, &p1, &p2);
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
