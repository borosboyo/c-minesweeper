#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#include "debugmalloc.h"
#include "megjelenites.h"
#include "jatekallas.h"

int main(int argc, char *argv[]) {
    /* Inicializálási folyamat. */
    srand(time(NULL));
    int magassag, szelesseg, aknak, nehezseg;

    Dicsoseglista *lista = dicsoseglista_foglal();
    dicsoseglista_elkeszit(lista);

    inic(&magassag,&szelesseg,&aknak,&nehezseg);
    Palya **tomb;

    /* Azért foglalunk 2-vel nagyobb tömböt, hogy a tényleges pályát nullával vegyük körbe. (Későbbi printfnél látszik) */
    /* Ennek segítségével sokkal könnyebben megy az aknás szomszédok összeszámolása, hiszen nincsenek első/utolsó sorok/oszlopok. */
    /* A szomszédos aknákat számoló függvény tehát sokkal egyszerűbben tud dolgozni.*/

    tomb = dintomb_foglal(magassag+2,szelesseg+2);

    dintomb_nullaz(magassag+2,szelesseg+2,tomb);

    dintomb_akna_letesz(magassag+1,szelesseg+1,aknak,tomb);

    dintomb_aknanelkul_letesz(magassag+1,szelesseg+1,tomb);


    /* Ablak létrehozása. */
    SDL_Window *window;
    SDL_Renderer *renderer;
    sdl_init("Minesweeper", szelesseg * 25, magassag*25+60, &window, &renderer);

    /* Kép betöltése. */
    SDL_Texture *ikonok = IMG_LoadTexture(renderer, "main.png");
    if (ikonok == NULL) {
        SDL_Log("Nem nyithato meg a kepfajl: %s", IMG_GetError());
        exit(1);
    }

    /* Betűtípus betöltése, 32 pont magassággal. */
    TTF_Init();
    TTF_Font *font = TTF_OpenFont("LiberationSerif-Regular.ttf", 20);
    if (!font) {
        SDL_Log("Nem sikerult megnyitni a fontot! %s\n", TTF_GetError());
        exit(1);
    }


    palya_rajzol(renderer, ikonok, facingdown, szelesseg, magassag);
    hatter(renderer,szelesseg,magassag);

    /* Felirat megrajzolása. Ennek segítségével lesz elkészítve a hátramaradt aknák száma és az eltelt ídő. */
    infoszoveg_elkeszit(renderer, font, magassag);

    SDL_TimerID id = SDL_AddTimer(1000, idozit, NULL);


    /* Az eseményvezérelt hurok. */;
    bool quit = false, vege = false;
    int ikonx = 0, ikony = 0, zaszlok = aknak, ido = 10, nyer = 0;
    SDL_Event event;
    zaszloszam_rajzol(font,renderer,szelesseg,magassag,zaszlok);
    while(!quit){
        if(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_USEREVENT:
                    if(vege == false){
                        ido++;
                        tisztit_ido(renderer,szelesseg,magassag);
                        ido_rajzol(font,renderer,szelesseg,magassag,ido);
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    ikony = event.motion.y / 25 +1;
                    ikonx = event.motion.x / 25 +1;
                    if(ikony <= magassag){
                        if (event.button.button == SDL_BUTTON_LEFT) {
                            if(vege == true){
                                kilepesgomb((szelesseg * 25) / 2 - 25,(magassag * 25) / 2 + 30,event.motion.x,event.motion.y);
                                quit = true;
                                if(nyer == (szelesseg * magassag - aknak)-1){
                                    dicsoseglista_beleir(nehezseg,ido);
                                }
                            }
                            else{
                                if(tomb[ikony][ikonx].felfedve == false){
                                    if(tomb[ikony][ikonx].mennyi == -1){
                                        vesztett_rajzol(renderer,ikonok,bomb,tomb,szelesseg,magassag);
                                        ikon_rajzol(renderer, ikonok, boom, (ikonx-1)*25, (ikony-1)*25);
                                        jatekvege(renderer,font,szelesseg,magassag);
                                        vegsoszoveg_elkeszit(renderer, font, szelesseg, magassag, "Vesztettel", "Kilep");
                                        tomb[ikony][ikonx].felfedve = true;
                                        vege = true;
                                    }
                                    else{
                                        if(nyer == (szelesseg * magassag - aknak)-1){
                                            ikon_rajzol(renderer,ikonok,tomb[ikony][ikonx].mennyi,(ikonx-1)*25,(ikony-1)*25);
                                            jatekvege(renderer,font,szelesseg,magassag);
                                            vegsoszoveg_elkeszit(renderer, font, szelesseg, magassag, "Gratulalok", "Kilep");
                                            tomb[ikony][ikonx].felfedve = true;
                                            vege = true;
                                        }
                                        else{
                                            rekurziv(renderer,ikonok,tomb,ikonx,ikony,szelesseg,magassag);
                                            ikon_rajzol(renderer,ikonok,tomb[ikony][ikonx].mennyi,(ikonx-1)*25,(ikony-1)*25);
                                            tomb[ikony][ikonx].felfedve = true;
                                            nyer++;
                                        }

                                    }
                                }
                            }
                        }
                        if(event.button.button == SDL_BUTTON_RIGHT){
                            if(tomb[ikony][ikonx].felfedve == false){
                                switch(tomb[ikony][ikonx].zaszlos){
                                    case false:
                                        if(zaszlok > 0){
                                            ikon_rajzol(renderer, ikonok, flagged, (ikonx-1)*25, (ikony-1)*25);
                                            tomb[ikony][ikonx].zaszlos = true;
                                            zaszlok--;
                                            tisztit_zaszlo(renderer,szelesseg,magassag);
                                            zaszloszam_rajzol(font,renderer,szelesseg,magassag,zaszlok);
                                        }
                                        break;
                                    case true:
                                        if(zaszlok <= 10){
                                            ikon_rajzol(renderer, ikonok, facingdown, (ikonx-1)*25, (ikony-1)*25);
                                            tomb[ikony][ikonx].zaszlos = false;
                                            zaszlok++;
                                            tisztit_zaszlo(renderer,szelesseg,magassag);
                                            zaszloszam_rajzol(font,renderer,szelesseg,magassag,zaszlok);
                                        }
                                        break;
                                }
                            }
                        }
                    }
                    break;
                case SDL_QUIT:
                    quit = true;
                    SDL_Quit();
                    break;
            }

            if(!quit)
                SDL_RenderPresent(renderer);
        }
    }

    SDL_DestroyTexture(renderer);
    SDL_Event ev;
    SDL_RemoveTimer(id);
    TTF_CloseFont(font);
    dintomb_felszabadit(tomb);
    free(lista);
    return 0;
}
