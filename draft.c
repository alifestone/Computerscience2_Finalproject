#include "base.h"
#include "declare.h"
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
    roundedBoxRGBA(ren,cen.x,cen.y,cen.x+cen.w,cen.y+cen.h,r,color.r,color.g,color.b,color.a);
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
int judge(Card *card, Player *p1, Player *p2, SDL_Renderer *ren, Card *linked) {
    if (card->link) {
        return 3;
    }

    if (card->type == BASIC_ATK || card->type == BASIC_DEF || card->type == BASIC_MOV) {
        switch (card->type) {
            case BASIC_ATK:
                if (abs(p1->pos - p2->pos) <= card->rng) {
                    p2->health -= card->dmg;
                    p1->power += card->val;
                    return 1;
                } else {
                    char buffer[100];
                    snprintf(buffer, sizeof(buffer), "Attack out of range! (Range: %d)", card->rng);
                    WriteText(ren, buffer, (SDL_Color){255, 0, 0, 255}, WSCREEN / 2, HSCREEN / 2 + 100, 30);
                    SDL_RenderPresent(ren);
                    SDL_Delay(2000);
                    return 0;
                }
            case BASIC_DEF:
                if (p1->defense + card->defense > p1->fable->defense) {
                    char buffer[100];
                    snprintf(buffer, sizeof(buffer), "Defense cannot exceed %d.", p1->fable->defense);
                    WriteText(ren, buffer, (SDL_Color){255, 0, 0, 255}, WSCREEN / 2, HSCREEN / 2 + 100, 30);
                    SDL_RenderPresent(ren);
                    SDL_Delay(2000);
                    p1->defense = p1->fable->defense;
                } else {
                    p1->defense += card->defense;
                }
                p1->power += card->val;
                return 1;
            case BASIC_MOV:
                return 2;
        }
    }

    // Skill cards require a basic card to be linked
    if (linked && (linked->type == SKILL_ATK || linked->type == SKILL_DEF || linked->type == SKILL_MOV)) {
        switch (linked->type) {
            case SKILL_ATK:
                if (abs(p1->pos - p2->pos) <= linked->rng) {
                    p2->health -= linked->dmg;
                    p1->power += linked->val;
                    return 1;
                } else {
                    char buffer[100];
                    snprintf(buffer, sizeof(buffer), "Skill attack out of range! (Range: %d)", linked->rng);
                    WriteText(ren, buffer, (SDL_Color){255, 0, 0, 255}, WSCREEN / 2, HSCREEN / 2 + 100, 30);
                    SDL_RenderPresent(ren);
                    SDL_Delay(2000);
                    return 0;
                }
            case SKILL_DEF:
                if (p1->defense + linked->defense > p1->fable->defense) {
                    char buffer[100];
                    snprintf(buffer, sizeof(buffer), "Defense cannot exceed %d.", p1->fable->defense);
                    WriteText(ren, buffer, (SDL_Color){255, 0, 0, 255}, WSCREEN / 2, HSCREEN / 2 + 100, 30);
                    SDL_RenderPresent(ren);
                    SDL_Delay(2000);
                    p1->defense = p1->fable->defense;
                } else {
                    p1->defense += linked->defense;
                }
                p1->power += linked->val;
                return 1;
            case SKILL_MOV:
                return 2;
        }
    }

    return 0; // default: invalid or unknown combination
}
bool linkcheck(Type skill, Type candidate){
    return (skill>=SKILL_ATK&&skill<=SKILL_MOV)&&(candidate==skill-4);
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
    int p1_epic_selected=0,p2_epic_selected=0;
    while(inbattle){
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT){
                inbattle=0;
            }
            if(e.type==SDL_MOUSEBUTTONDOWN&&e.button.button==SDL_BUTTON_LEFT){
                int32_t mx=e.button.x, my=e.button.y;
                // End turn button
                SDL_Rect rect={WSCREEN-250, HSCREEN/2-50, 200, 100};
                //end phase
                if(SDL_PointInRect(&(SDL_Point){mx,my},&rect)){
                    printf("Ending turn.\n");
                    if(turn%2==1) {
                        for(int i=0;i<p1->hand.cnt;i++){
                            p1->hand.cards[i]=NULL;
                        }
                        p1->hand.cnt=0;
                        draw_hand(p1,6);
                        p1->power=0;
                    }    
                    if(turn%2==0) {
                        for(int i=0;i<p2->hand.cnt;i++){
                            p2->hand.cards[i]=NULL;
                        }
                        p2->hand.cnt=0;
                        draw_hand(p2,6);
                        p2->power=0;
                    }
                    turn++;
                    break;
                }
                if(p1_epic_selected==0){
                    for(int i=0;i<3;i++){
                        SDL_Rect epic_rect={50, HSCREEN-(300+i*70), 300, 50};
                        if(SDL_PointInRect(&(SDL_Point){mx,my},&epic_rect)&&turn%2==1&&p1->health<=p1->fable->epic_threshold){
                            p1_epic_selected=i+1;
                            break;
                        }
                    }
                }
                // p1 hand
                for(int i=0;i<p1->hand.cnt;i++){
                    int32_t x=50+i*(WCARD+20), y=HSCREEN-HCARD+40;
                    SDL_Rect rect={x,y,WCARD,HCARD};
                    Card *linkholder=NULL;
                    if(SDL_PointInRect(&(SDL_Point){mx,my},&rect)&&turn%2==1){
                        printf("p1 Playing...\n");
                        int validity=judge(p1->hand.cards[i], p1, p2, ren,linkholder);
                        if(validity==1){
                            p1->disc.cards[p1discardindex++]=p1->hand.cards[i];
                            for(int j=i;j<p1->hand.cnt-1;j++){
                                p1->hand.cards[j]=p1->hand.cards[j+1];
                            }
                            p1->hand.cnt--;
                        }else if(validity==2){
                            int origin = p1->pos; // current position
                            int move = p1->hand.cards[i]->mov;
                            int from = origin - move;
                            int to = origin + move;

                            // Clamp to lane range
                            if (from < -4) from = -4;
                            if (to > 4) to = 4;

                            // Highlight movement blocks
                            for (int j = from; j <= to; j++) {
                                if(j!=p1->pos)filledCircleRGBA(ren, XCENTER + j * (WField + 20), YCENTER, 20, 0, 255, 0, 255);
                            }
                            SDL_RenderPresent(ren);

                            // Wait for valid block click
                            SDL_Event e2;
                            int picked = -5;
                            while (picked == -5 && SDL_WaitEvent(&e2)) {
                                if (e2.type == SDL_QUIT) {
                                    inbattle = 0;
                                    break;
                                }
                                if (e2.type == SDL_MOUSEBUTTONDOWN && e2.button.button == SDL_BUTTON_LEFT) {
                                    int mx = e2.button.x, my = e2.button.y;
                                    for (int j = from; j <= to; j++) {
                                        SDL_Rect move_rect = {XCENTER + j * (WField + 20)-20, YCENTER - 20, 40, 40};
                                        if (SDL_PointInRect(&(SDL_Point){mx, my}, &move_rect)) {
                                            picked = j;
                                            if (picked != p2->pos|| picked != p1->pos) {
                                                p1->pos = picked;
                                                p1->power += p1->hand.cards[i]->val;
                                                //discard
                                                p1->disc.cards[p1discardindex++] = p1->hand.cards[i];
                                                for (int j = i; j < p1->hand.cnt - 1; j++) {
                                                    p1->hand.cards[j] = p1->hand.cards[j + 1];
                                                }
                                                p1->hand.cnt--;
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }else if(validity==3){
                            int samecnt=0;
                            for(int j=0;j<p1->hand.cnt;j++){
                                if(linkcheck(p1->hand.cards[i]->type, p1->hand.cards[j]->type)){
                                    filledCircleRGBA(ren, 100+j*(WCARD+20), HSCREEN-40, 20, 255, 255, 0, 255);
                                    samecnt++;
                                }
                            }
                            SDL_RenderPresent(ren);
                            if(samecnt==0){
                                WriteText(ren, "No valid link cards found!", (SDL_Color){255, 0, 0, 255}, WSCREEN / 2, HSCREEN / 2+100,30);
                                SDL_RenderPresent(ren);
                                SDL_Delay(2000); // Pause to show the message
                                continue;
                            }
                            SDL_Event e2;
                            int link_failed = 0,link_completed=0;
                            while (SDL_WaitEvent(&e2)) {
                                if (e2.type == SDL_QUIT) {
                                    inbattle = 0;
                                    break;
                                }
                                if (e2.type == SDL_MOUSEBUTTONDOWN && e2.button.button == SDL_BUTTON_LEFT) {
                                    int mx = e2.button.x, my = e2.button.y;
                                    for (int j = 0; j < p1->hand.cnt; j++) {
                                        SDL_Rect link_rect = {50 + j * (WCARD + 20), HSCREEN - HCARD - 40, WCARD, HCARD};
                                        if (SDL_PointInRect(&(SDL_Point){mx, my}, &link_rect)) {
                                            if (!linkcheck(p1->hand.cards[i]->type, p1->hand.cards[j]->type)) {
                                                WriteText(ren, "Invalid link card type!", (SDL_Color){255, 0, 0, 255}, WSCREEN / 2, HSCREEN / 2 + 100, 30);
                                                SDL_RenderPresent(ren);
                                                SDL_Delay(1500);
                                                link_failed = 1;
                                                break;
                                            }
                                            int linkvalidity = judge(p1->hand.cards[j], p1, p2, ren, p1->hand.cards[i]);
                                            if (linkvalidity == 1) {
                                                // Discard both cards
                                                p1->disc.cards[p1discardindex++] = p1->hand.cards[i];
                                                p1->disc.cards[p1discardindex++] = p1->hand.cards[j];

                                                // Remove higher index first to avoid shifting issues
                                                int max_idx = (i > j) ? i : j;
                                                int min_idx = (i > j) ? j : i;

                                                for (int k = max_idx; k < p1->hand.cnt - 1; k++) {
                                                    p1->hand.cards[k] = p1->hand.cards[k + 1];
                                                }
                                                p1->hand.cnt--;
                                                for (int k = min_idx; k < p1->hand.cnt - 1; k++) {
                                                    p1->hand.cards[k] = p1->hand.cards[k + 1];
                                                }
                                                p1->hand.cnt--;
                                                printf("%d ",p1->hand.cnt);
                                                link_completed = 1;
                                                //SDL_RenderPresent(ren);
                                            } else {
                                                link_failed = 1;
                                            }
                                            break; // break the for loop
                                        }
                                    }
                                    if (link_failed||link_completed) break; // break the SDL_WaitEvent loop
                                }
                            }
                        }else{
                            continue;
                        }   
                    }
                }
                if(p2_epic_selected==0){
                    for(int i=0;i<3;i++){
                        SDL_Rect epic_rect={WSCREEN-370, 280+(i*70), 300, 50};
                        if(SDL_PointInRect(&(SDL_Point){mx,my},&epic_rect)&&turn%2==0&&p2->health<=p2->fable->epic_threshold){
                            p2_epic_selected=i+1;
                            break;
                        }
                    }
                }
                // p2 hand
                for(int i=0;i<p2->hand.cnt;i++){
                    int32_t x=WSCREEN-(50+(i+1)*(WCARD+20)), y=-40;
                    SDL_Rect rect={x,y,WCARD,HCARD};
                    Card *linkholder=NULL;
                    if(SDL_PointInRect(&(SDL_Point){mx,my},&rect)&&turn%2==0){
                        printf("p2 Playing...\n");
                        int validity=judge(p2->hand.cards[i], p2, p1, ren,linkholder);
                        if(validity==1){
                            p2->disc.cards[p1discardindex++]=p2->hand.cards[i];
                            for(int j=i;j<p2->hand.cnt-1;j++){
                                p2->hand.cards[j]=p2->hand.cards[j+1];
                            }
                            p2->hand.cnt--;
                        }else if(validity==2){
                            int origin = p2->pos; // current position
                            int move = p2->hand.cards[i]->mov;
                            int from = origin - move;
                            int to = origin + move;

                            // Clamp to lane range
                            if (from < -4) from = -4;
                            if (to > 4) to = 4;

                            // Highlight movement blocks
                            for (int j = from; j <= to; j++) {
                                if(j!=p2->pos)filledCircleRGBA(ren, XCENTER + j * (WField + 20), YCENTER, 20, 0, 255, 0, 255);
                            }
                            SDL_RenderPresent(ren);

                            // Wait for valid block click
                            SDL_Event e2;
                            int picked = -5;
                            while (picked == -5 && SDL_WaitEvent(&e2)) {
                                if (e2.type == SDL_QUIT) {
                                    inbattle = 0;
                                    break;
                                }
                                if (e2.type == SDL_MOUSEBUTTONDOWN && e2.button.button == SDL_BUTTON_LEFT) {
                                    int mx = e2.button.x, my = e2.button.y;
                                    for (int j = from; j <= to; j++) {
                                        SDL_Rect move_rect = {XCENTER + j * (WField + 20)-20, YCENTER - 20, 40, 40};
                                        if (SDL_PointInRect(&(SDL_Point){mx, my}, &move_rect)) {
                                            picked = j;
                                            if (picked != p1->pos|| picked != p2->pos) {
                                                p2->pos = picked;
                                                p2->power += p2->hand.cards[i]->val;
                                                //discard
                                                p2->disc.cards[p1discardindex++] = p2->hand.cards[i];
                                                for (int j = i; j < p2->hand.cnt - 1; j++) {
                                                    p2->hand.cards[j] = p2->hand.cards[j + 1];
                                                }
                                                p2->hand.cnt--;
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }else if(validity==3){
                            int samecnt=0;
                            for(int j=0;j<p2->hand.cnt;j++){
                                if(linkcheck(p2->hand.cards[i]->type, p2->hand.cards[j]->type)){
                                    filledCircleRGBA(ren, WSCREEN-((j+1)*(WCARD+20)), 40, 20, 255, 255, 0, 255);
                                    samecnt++;
                                }
                            }
                            SDL_RenderPresent(ren);
                            if(samecnt==0){
                                WriteText(ren, "No valid link cards found!", (SDL_Color){255, 0, 0, 255}, WSCREEN / 2, HSCREEN / 2+100,30);
                                SDL_RenderPresent(ren);
                                SDL_Delay(2000); // Pause to show the message
                                continue;
                            }
                            SDL_Event e2;
                            int link_failed = 0,link_completed=0;
                            while (SDL_WaitEvent(&e2)) {
                                if (e2.type == SDL_QUIT) {
                                    inbattle = 0;
                                    break;
                                }
                                if (e2.type == SDL_MOUSEBUTTONDOWN && e2.button.button == SDL_BUTTON_LEFT) {
                                    int mx = e2.button.x, my = e2.button.y;
                                    for (int j = 0; j < p2->hand.cnt; j++) {
                                        SDL_Rect link_rect = {WSCREEN-(50+(j+1)*(WCARD+20)), - 40, WCARD, HCARD};
                                        if (SDL_PointInRect(&(SDL_Point){mx, my}, &link_rect)) {
                                            if (!linkcheck(p2->hand.cards[i]->type, p2->hand.cards[j]->type)) {
                                                WriteText(ren, "Invalid link card type!", (SDL_Color){255, 0, 0, 255}, WSCREEN / 2, HSCREEN / 2 + 100, 30);
                                                SDL_RenderPresent(ren);
                                                SDL_Delay(1500);
                                                link_failed = 1;
                                                break;
                                            }
                                            int linkvalidity = judge(p2->hand.cards[j], p2, p1, ren, p2->hand.cards[i]);
                                            if (linkvalidity == 1) {
                                                // Discard both cards
                                                p2->disc.cards[p1discardindex++] = p2->hand.cards[i];
                                                p2->disc.cards[p1discardindex++] = p2->hand.cards[j];

                                                // Remove higher index first to avoid shifting issues
                                                int max_idx = (i > j) ? i : j;
                                                int min_idx = (i > j) ? j : i;

                                                for (int k = max_idx; k < p2->hand.cnt - 1; k++) {
                                                    p2->hand.cards[k] = p2->hand.cards[k + 1];
                                                }
                                                p2->hand.cnt--;
                                                for (int k = min_idx; k < p2->hand.cnt - 1; k++) {
                                                    p2->hand.cards[k] = p2->hand.cards[k + 1];
                                                }
                                                p2->hand.cnt--;
                                                printf("%d ",p2->hand.cnt);
                                                link_completed = 1;
                                                //SDL_RenderPresent(ren);
                                            } else {
                                                link_failed = 1;
                                            }
                                            break; // break the for loop
                                        }
                                    }
                                    if (link_failed||link_completed) break; // break the SDL_WaitEvent loop
                                }
                            }
                        }else{
                            continue;
                        }
                    }
                }
            }
            if(e.type==SDL_MOUSEBUTTONDOWN&&fighting==0){
                inbattle=0;
            }
        }
        if(p1->health<=0||p2->health<=0){
            inbattle=0;
            fighting=0;
            continue;
        }
        SDL_SetRenderDrawColor(ren,255,255,255,255);
        SDL_RenderClear(ren);
        BattleLane(ren,p1,p2);
        // P1 hand
        for(int i=0;i<p1->hand.cnt;i++){
            SDL_Rect rect={50+i*(WCARD+20),HSCREEN-HCARD+40,WCARD,HCARD};
            SDL_SetRenderDrawColor(ren,200,0,0,255);
            SDL_RenderFillRect(ren,&rect);
            WriteText(ren,p1->hand.cards[i]->name,(SDL_Color){255,255,255,255},rect.x+WCARD/2,rect.y+HCARD/2,16);
        }
        // P1 health
        SDL_Rect p1_health_rect={WSCREEN-350, HSCREEN-100, p1->health*300/p1->fable->health, 30};
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        SDL_RenderFillRect(ren, &p1_health_rect);
        SDL_Rect p1_health_frame={WSCREEN-350, HSCREEN-100, 300, 30};
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderDrawRect(ren, &p1_health_frame);
        // P1 epic cards
        if(p1->health<=p1->fable->epic_threshold){
            if(p1_epic_selected==0){
                for(int i=0;i<3;i++){
                    SDL_Rect epic_rect={50, HSCREEN-(300+i*70), 300, 50};
                    SDL_SetRenderDrawColor(ren, 129, 41, 144, 255);
                    SDL_RenderFillRect(ren, &epic_rect);
                    WriteText(ren,p1->fable->epic[i].name,(SDL_Color){255,255,255,255}, epic_rect.x+150, epic_rect.y+25,10);
                }
            }else{
                SDL_Rect epic_rect={50, HSCREEN-300, 300, 50};
                SDL_SetRenderDrawColor(ren, 129, 41, 144, 255);
                SDL_RenderFillRect(ren, &epic_rect);
                WriteText(ren,p1->fable->epic[p1_epic_selected-1].name,(SDL_Color){255,255,255,255}, epic_rect.x+150, epic_rect.y+25,10);
            }
        }
        // P1 epic indicator
        if(p1_epic_selected==0){
            SDL_Rect epic_p1={WSCREEN-350+(p1->fable->epic_threshold*300/p1->fable->health), HSCREEN-105, 20, 40};
            SDL_SetRenderDrawColor(ren, 129, 41, 144, 255);
            SDL_RenderFillRect(ren, &epic_p1);
        }
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
        SDL_Rect p1_defense_rect={WSCREEN-350, HSCREEN-200, p1->defense*24, 30};
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
            WriteText(ren,p2->hand.cards[i]->name,(SDL_Color){255,255,255,255},rect.x+WCARD/2,rect.y+HCARD/2,16);
        }
        // P2 health
        SDL_Rect p2_health_rect={50, 200, p2->health*300/p2->fable->health, 30};
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        SDL_RenderFillRect(ren, &p2_health_rect);
        SDL_Rect p2_health_frame={50, 200, 300, 30};
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderDrawRect(ren, &p2_health_frame);
        // P2 epic cards
        if(p2->health<=p2->fable->epic_threshold){
            if(p2_epic_selected==0){
                for(int i=0;i<3;i++){
                    SDL_Rect epic_rect={WSCREEN-370, 280+(i*70), 300, 50};
                    SDL_SetRenderDrawColor(ren, 129, 41, 144, 255);
                    SDL_RenderFillRect(ren, &epic_rect);
                    WriteText(ren,p2->fable->epic[i].name,(SDL_Color){255,255,255,255}, epic_rect.x+150, epic_rect.y+25,10);
                }
            }else{
                SDL_Rect epic_rect={WSCREEN-370, 280, 300, 50};
                SDL_SetRenderDrawColor(ren, 129, 41, 144, 255);
                SDL_RenderFillRect(ren, &epic_rect);
                WriteText(ren,p2->fable->epic[p2_epic_selected-1].name,(SDL_Color){255,255,255,255}, epic_rect.x+150, epic_rect.y+25,10);
            }
        }
        // P2 epic indicator
        if(p2_epic_selected==0){
            SDL_Rect epic_p2={50+(p2->fable->epic_threshold*300/p2->fable->health), 195, 20, 40};
        SDL_SetRenderDrawColor(ren, 129, 41, 144, 255);
        SDL_RenderFillRect(ren, &epic_p2);
        }
        // P2 power
        SDL_Rect p2_power_frame={50, 150, p2->power*12, 30};
        SDL_SetRenderDrawColor(ren, 124, 199, 232, 255);
        SDL_RenderFillRect(ren, &p2_power_frame);
        SDL_Rect p2_power_rect={50, 150, 300, 30};
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderDrawRect(ren, &p2_power_rect);
        for(int x=1;x<=24;x++){
            SDL_RenderDrawLine(ren,50+x*12,150,50+x*12,180);
        }
        // P2 defense
        SDL_Rect p2_defense_limit={50+(p2->fable->defense)*24, 100, (9-p2->fable->defense)*24, 30};
        SDL_SetRenderDrawColor(ren, 139, 0, 0, 255);
        SDL_RenderFillRect(ren, &p2_defense_limit);
        SDL_Rect p2_defense_rect={50, 100, p2->defense*24, 30};
        SDL_SetRenderDrawColor(ren, 255, 153, 19, 255);
        SDL_RenderFillRect(ren, &p2_defense_rect);
        SDL_Rect p2_defense_frame={50, 100, 216, 30};
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderDrawRect(ren, &p2_defense_frame);
        for(int x=1;x<=8;x++){
            SDL_RenderDrawLine(ren,50+x*24,100,50+x*24,130);
        }

        // Turn indicator
        SDL_Rect end_turn_button={WSCREEN-150, HSCREEN/2, 200, 100};
        SDL_Color turn_color = {0, 0, 0, 255};
        if(turn % 2 == 1) {
            WriteText(ren, "P1's Turn", turn_color, 100, HSCREEN/2,30);
            SDL_Color end_turn_color = {200, 0, 0, 255};
            DrawButton(ren, end_turn_button, 10, end_turn_color, (SDL_Color){255, 255, 255, 255}, "End Turn");
        } else {
            WriteText(ren, "P2's Turn", turn_color, 100, HSCREEN/2,30);
            SDL_Color end_turn_color = {0, 0, 200, 255};
            DrawButton(ren, end_turn_button, 10, end_turn_color, (SDL_Color){255, 255, 255, 255}, "End Turn");
        }

        
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
    filledCircleRGBA(ren,XCENTER+p1->pos*(WField+20),YCENTER,40,p1->fable->Piece.r,p1->fable->Piece.g,p1->fable->Piece.b,p1->fable->Piece.a);
    filledCircleRGBA(ren,XCENTER+p2->pos*(WField+20),YCENTER,40,p2->fable->Piece.r,p2->fable->Piece.g,p2->fable->Piece.b,p2->fable->Piece.a);
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
    p1->pos=-1;
    starting_deck(p1);
    setup_player(p2, &red_hood);
    p2->pos=1;
    starting_deck(p2);
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
