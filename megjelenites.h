#ifndef MEGJELENITES_H_INCLUDED
#define MEGJELENITES_H_INCLUDED

#include "jatekallas.h"

/* A képek struktúrája*/
typedef enum Ikon {
    zero, one, two, three, four, five, six, seven, eight,
    bomb, boom, facingdown, flagged, bombx
}Ikon;

/*Időzítés*/
Uint32 idozit(Uint32 ms, void *param);

/* Létrehozza az ablakot. */
void sdl_init(char const *felirat, int szeles, int magas, SDL_Window **pwindow, SDL_Renderer **prenderer);

/* Kirajzol egy ikont; a forrás a betöltött png, a cél nevû képre rajzol. */
void ikon_rajzol(SDL_Renderer *renderer, SDL_Texture *ikonok, Ikon melyik, int ikonx, int ikony);


/* A szöveg rajzolásáért felelõs */
void szoveg_rajzol(TTF_Font *font, SDL_Surface *felirat, SDL_Texture *felirat_t,
                   SDL_Rect hova, SDL_Renderer *renderer, SDL_Color fekete, int szelesseg, int magassag);

void teglalap_rajzol(SDL_Renderer *renderer, int x, int y, int w, int h , int r , int g, int b);

/* Kirajzolja a pályát kezdetben a lefedett ikonokból, pont annyi mezõvel, amit a felhasználó megadott. */
void palya_rajzol(SDL_Renderer *renderer, SDL_Texture *ikonok, Ikon melyik, int szelesseg, int magassag);

void adatok_rajzol(SDL_Renderer *renderer, TTF_Font *font, char* szoveg, int szelesseg, int magassag);

void infoszoveg_elkeszit(SDL_Renderer *renderer, TTF_Font *font, int magassag);

void zaszloszam_rajzol(TTF_Font *font, SDL_Renderer *renderer, int szelesseg, int magassag, int zaszlok);

void ido_rajzol(TTF_Font *font, SDL_Renderer *renderer, int szelesseg, int magassag, int ido);

void tisztit_ido(SDL_Renderer *renderer, int szelesseg, int magassag);

void tisztit_zaszlo(SDL_Renderer *renderer, int szelesseg, int magassag);

void hatter(SDL_Renderer *renderer, int szelesseg, int magassag);

void vesztett_rajzol(SDL_Renderer *renderer, SDL_Texture *ikonok, Ikon melyik, Palya **tomb, int szelesseg, int magassag);

void rekurziv(SDL_Renderer *renderer, SDL_Texture *ikonok, Palya **tomb, int ikonx, int ikony, int szelesseg, int magassag);

void vegsoszoveg_elkeszit(SDL_Renderer *renderer, TTF_Font *font, int szelesseg, int magassag, char* statusz, char* kilepes);

void jatekvege(SDL_Renderer *renderer, TTF_Font *font, int szelesseg, int magassag);

void kilepesgomb(int szelesseg, int magassag, int xpoz, int ypoz);

#endif
