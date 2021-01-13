#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "debugmalloc.h"
#include "megjelenites.h"

/*Időzítés*/
Uint32 idozit(Uint32 ms, void *param){
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;
}

/* Létrehozza az ablakot. */
void sdl_init(char const *felirat, int szelesseg, int magassag, SDL_Window **pwindow, SDL_Renderer **prenderer){
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
        exit(1);
    }
    SDL_Window *window = SDL_CreateWindow(felirat, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, szelesseg, magassag, 0);
    if (window == NULL) {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }
    SDL_RenderClear(renderer);

    *pwindow = window;
    *prenderer = renderer;
}

/* Kirajzol egy ikont; a forrás a betöltött png, a cél nevû képre rajzol.
 * Melyik bábut, milyen koordinátákra: melyik, x, y. */
void ikon_rajzol(SDL_Renderer *renderer, SDL_Texture *ikonok, Ikon melyik, int ikonx, int ikony){
    /* A forrás képbõl ezekrõl a koordinátákról, ilyen méretû részletet másolunk. */
    SDL_Rect src = { melyik*25, 0, 25, 25 };
    /* A cél képre, ezekre a koordinátákra másoljuk. */
    SDL_Rect dest = { ikonx, ikony, 25, 25 };
    /* Képrészlet másolása. */
    SDL_RenderCopy(renderer, ikonok, &src, &dest);
}

void szoveg_rajzol(TTF_Font *font, SDL_Surface *felirat, SDL_Texture *felirat_t,
                   SDL_Rect hova, SDL_Renderer *renderer, SDL_Color fekete, int szelesseg, int magassag){
    felirat_t = SDL_CreateTextureFromSurface(renderer, felirat);
    hova.x = szelesseg;
    hova.y = magassag;
    hova.w = felirat->w;
    hova.h = felirat->h;
    SDL_RenderCopy(renderer, felirat_t, NULL, &hova);
    SDL_FreeSurface(felirat);
    SDL_DestroyTexture(felirat_t);
    SDL_RenderPresent(renderer);

}

void teglalap_rajzol(SDL_Renderer *renderer, int x, int y, int w, int h , int r , int g, int b){
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    SDL_SetRenderDrawColor(renderer, r, g, b, 0);
    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderPresent(renderer);
}

/* Kirajzolja a pályát kezdetben a lefedett ikonokból, pont annyi mezõvel, amit a felhasználó megadott. */
void palya_rajzol(SDL_Renderer *renderer, SDL_Texture *ikonok, Ikon melyik, int szelesseg, int magassag){
    for (int ii = 0; ii < magassag; ii++)
        for(int jj = 0; jj < szelesseg; jj++)
            ikon_rajzol(renderer, ikonok, melyik, 25*jj , 25*ii );
}

void adatok_rajzol(SDL_Renderer *renderer, TTF_Font *font, char* szoveg, int szelesseg, int magassag){
    SDL_Color fekete = {0, 0, 0};
    SDL_Surface *felirat;
    SDL_Texture *felirat_t;
    SDL_Rect hova = { 0, 0, 0, 0 };

    felirat = TTF_RenderUTF8_Blended(font, szoveg, fekete);
    szoveg_rajzol(font, felirat, felirat_t,
                  hova, renderer, fekete, szelesseg, magassag);

}

void infoszoveg_elkeszit(SDL_Renderer *renderer, TTF_Font *font, int magassag){
    char *ido = "Megmaradt aknak:";
    char *megmaradt = "Eltelt ido:";

    adatok_rajzol(renderer, font, ido,0, magassag*25);
    adatok_rajzol(renderer, font, megmaradt, 0, magassag * 25 + 30);
}

void zaszloszam_rajzol(TTF_Font *font, SDL_Renderer *renderer, int szelesseg, int magassag, int zaszlok){
    char tmp[6];
    sprintf(tmp,"%d",zaszlok);
    adatok_rajzol(renderer, font, tmp, 150, magassag * 25);
}

void ido_rajzol(TTF_Font *font, SDL_Renderer *renderer, int szelesseg, int magassag, int ido){
    char tmp[6];
    sprintf(tmp,"%d:%d", ido / 60 , ido % 60);

    adatok_rajzol(renderer, font, tmp, 150, magassag * 25 + 30);
}

void tisztit_ido(SDL_Renderer *renderer, int szelesseg, int magassag){
    teglalap_rajzol(renderer, 150, magassag * 25 + 30, szelesseg * 25 - 90, 90 , 190 , 190 , 195);
}

void tisztit_zaszlo(SDL_Renderer *renderer, int szelesseg, int magassag){
    teglalap_rajzol(renderer, 150, magassag * 25, szelesseg * 25 - 90, 30 , 190 , 190 , 195);
}

void hatter(SDL_Renderer *renderer, int szelesseg, int magassag){
    teglalap_rajzol(renderer, 0, magassag * 25, szelesseg * 25, 90, 190, 190, 195);
}

/*Kirajzolja az összes másik aknát, ha rányomtunk egyre.*/
void vesztett_rajzol(SDL_Renderer *renderer, SDL_Texture *ikonok, Ikon melyik, Palya **tomb, int szelesseg, int magassag){
    for(int ii = 1; ii < magassag+2; ii++){
        for(int jj = 1; jj < szelesseg+2; jj++){
            tomb[ii][jj].felfedve = true;
            if(tomb[ii][jj].mennyi == -1){
                if(tomb[ii][jj].zaszlos == false){
                    ikon_rajzol(renderer,ikonok,bomb,(jj-1)*25,(ii-1)*25);
                }
                else{
                    ikon_rajzol(renderer,ikonok,bombx,(jj-1)*25, (ii-1)*25);
                }
            }
        }
    }
}

void rekurziv(SDL_Renderer *renderer, SDL_Texture *ikonok, Palya **tomb, int ikonx, int ikony, int szelesseg, int magassag){
    if(tomb[ikony][ikonx].felfedve == false){
    tomb[ikony][ikonx].felfedve = true;
    ikon_rajzol(renderer,ikonok,tomb[ikony][ikonx].mennyi,(ikonx-1) * 25 , (ikony-1) * 25);

    if(tomb[ikony][ikonx].mennyi == 0){
        if(ikony+1 > 0 && ikony+1 < magassag + 1 ){
            rekurziv(renderer,ikonok,tomb,ikonx,ikony+1,szelesseg,magassag);
        }
        if(ikony-1 > 0 && ikony-1 < magassag +1){
            rekurziv(renderer,ikonok,tomb,ikonx,ikony-1,szelesseg,magassag);
        }

        if(ikonx+1 > 0 && ikonx+1 < szelesseg +1){
            rekurziv(renderer,ikonok,tomb,ikonx+1,ikony,szelesseg,magassag);
        }
        if(ikonx-1 > 0 && ikonx-1 < szelesseg +1){
            rekurziv(renderer,ikonok,tomb,ikonx-1,ikony,szelesseg,magassag);
        }
    }
    }
    else{
        return;
    }
}


void vegsoszoveg_elkeszit(SDL_Renderer *renderer, TTF_Font *font, int szelesseg, int magassag, char* statusz, char* kilepes){
    adatok_rajzol(renderer,font, statusz, szelesseg * 25 / 2 - 35, magassag * 25 /2 );
    adatok_rajzol(renderer,font, kilepes, szelesseg * 25 / 2 - 15, magassag * 25 / 2 + 30);
}

void jatekvege(SDL_Renderer *renderer, TTF_Font *font, int szelesseg, int magassag){
    teglalap_rajzol(renderer,  (szelesseg * 25) / 2 - 50,  (magassag * 25) / 2,      110, 60, 190, 190, 195);
    teglalap_rajzol(renderer,  (szelesseg * 25) / 2 - 25 , (magassag * 25) / 2 + 30, 60,  30, 255, 0,   0);
}

void kilepesgomb(int szelesseg, int magassag, int xpoz, int ypoz){
    if( xpoz >= szelesseg && xpoz <= szelesseg + 60 && ypoz >= magassag && ypoz <= magassag + 30){
        SDL_Quit();
    }
}


