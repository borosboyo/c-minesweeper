#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#include "debugmalloc.h"
#include "jatekallas.h"

/*Elvégzi az inicializálást. */
void inic(int *magassag, int *szelesseg, int *aknak, int *nehezseg){
    int valasz;
    printf("Nehezsegi szint kivalasztasa:\n");
    printf("(1) Kezdo: 9x9 mezo 10 aknaval\n");
    printf("(2) Halado: 16x16 mezo 40 aknaval\n");
    printf("(3) Mester: 30x16 mezo 99 aknaval\n");
    printf("(4) Egyeni: egyeni meret egyeni aknaszammal (minimum kezdo, maximum mester)\n");
    printf("Valasztas: ");
    scanf("%d", &valasz);
    if( valasz >= 1 && valasz <=4)
        nehezseg_kivalaszt(valasz,magassag,szelesseg,aknak);
    else{
        printf("Nincs ilyen valasztas\n");
        inic(magassag,szelesseg,aknak,nehezseg);
    }
    *nehezseg = valasz;
}

void dicsoseglista_beleir(int nehezseg, int ido){

    FILE *fp;
    fp = fopen("dicsoseglista.txt", "a");

    if (fp == NULL) {
        perror("Fájl megnyitása sikertelen");
        return;
    }
    char becenev[51];
    printf("Jatekosbecenev(egyben): ");
    scanf("%s", &becenev);

    fprintf(fp,"%s %d %d\n",becenev,ido,nehezseg);

    fclose(fp);
}

Dicsoseglista* dicsoseglista_foglal(void){
    char sor[51];
    int meret = 0;

    FILE *fp;
    fp = fopen("dicsoseglista.txt", "r");
    if (fp == NULL) {
        perror("Fajl megnyitasa sikertelen");
        return;
    }
    while(fgets(sor,51,fp) != NULL){
        meret++;
    }
    fclose(fp);

    Dicsoseglista* uj = (Dicsoseglista*) malloc(meret * sizeof(Dicsoseglista));
    if(uj == NULL){
        printf("Nem sikerult foglalni");
        return;
    }
    return uj;
}

void dicsoseglista_rendez(Dicsoseglista *lista, int meret){
    for (int ii = 0; ii < meret-1; ++ii) {
        int minindex = ii;
        for (int jj = ii+1; jj < meret; ++jj){
            if (lista[jj].ido < lista[minindex].ido){
                minindex = jj;
            }
        }
        if (minindex != ii) {
            Dicsoseglista temp;

            strcpy(temp.becenev,lista[minindex].becenev);
            temp.ido = lista[minindex].ido;
            temp.nehezseg = lista[minindex].nehezseg;

            strcpy(lista[minindex].becenev,lista[ii].becenev);
            lista[minindex].ido = lista[ii].ido;
            lista[minindex].nehezseg = lista[ii].nehezseg;

            strcpy(lista[ii].becenev,temp.becenev);
            lista[ii].ido = temp.ido;
            lista[ii].nehezseg = temp.nehezseg;
        }
    }
}

void dicsoseglista_becenevek(Dicsoseglista *lista, int nehezseg, int meret){
    int jj = 1;
        for(int ii = 1; ii <= meret; ii++){
            if(lista[ii].nehezseg == nehezseg){
                if(jj <= 5)
                    printf("%d. %s %2d:%d | ",jj,lista[ii].becenev,lista[ii].ido / 60 , lista[ii].ido % 60);
                jj++;
            }
        }
        printf("\n\n");
}

void dicsoseglista_kiir(Dicsoseglista *lista, int meret){
    printf("Dicsoseglista:\n\nKezdo: ");
    dicsoseglista_becenevek(lista,1,meret);
    printf("Halado: ");
    dicsoseglista_becenevek(lista,2,meret);
    printf("Mester: ");
    dicsoseglista_becenevek(lista,3,meret);
}

void dicsoseglista_elkeszit(Dicsoseglista *lista){
    char sor[51];
    FILE *fp;
    fp = fopen("dicsoseglista.txt", "r");
    if (fp == NULL) {
        perror("Fájl megnyitása sikertelen");
        return;
    }
    int ii = 0;
    while(fgets(sor,51,fp) != NULL){
        sscanf(sor,"%s %d %d", lista[ii].becenev, &lista[ii].ido, &lista[ii].nehezseg);
        ii++;
    }
    dicsoseglista_rendez(lista,ii);
    dicsoseglista_kiir(lista,ii);
    fclose(fp);
}


void nehezseg_kivalaszt(int valasz, int* magassag, int* szelesseg, int* aknak){
    switch (valasz){
    case 1:
        *magassag = 9;
        *szelesseg = 9;
        *aknak = 10;
        break;
    case 2:
        *magassag = 16;
        *szelesseg = 16;
        *aknak = 40;
        break;
    case 3:
        *magassag = 30;
        *szelesseg = 16;
        *aknak = 99;
        break;
    case 4:
        printf("Magassag: ");
        scanf("%d", magassag);
        printf("Szelesseg: ");
        scanf("%d", szelesseg);
        printf("Aknak: ");
        scanf("%d", aknak);
        break;
    }
}
/*Dinamikusan foglal egy két dimenziós Palya típusú tömböt. */
Palya** dintomb_foglal(int magassag, int szelesseg){
    Palya **tomb;
    tomb = (Palya**) malloc(magassag * sizeof(Palya*));
    tomb[0] = (Palya*) malloc(szelesseg * magassag * sizeof(Palya));
    for (int ii = 1; ii < magassag; ii++)
        tomb[ii] = tomb[0] + ii*szelesseg;
    return tomb;
}

/*Felszabadítja a dinamikus tömböt. */
void dintomb_felszabadit(Palya **tomb){
    free(tomb[0]);
    free(tomb);
}
/* A Palya típusú tömb "mennyi" kezdeti értékét nullára állítja. */
void dintomb_nullaz(int magassag, int szelesseg, Palya **tomb){
    int ii, jj;
    for(ii = 0; ii < magassag; ii++){
        for(jj = 0; jj < szelesseg; jj++){
            tomb[ii][jj].mennyi = 0;
        }
    }
}

/*Véletlenszerûen lehelyez aknákat a tömbben, olyan helyekre, ahol még nincs. */
/* Mivel 0-val körülvett a tényleges játéktér ezért csak ott generál aknákat. */
void dintomb_akna_letesz(int magassag, int szelesseg, int aknak, Palya **tomb){
    int ii, jj;
    while(aknak > 0){
       ii = rand() % magassag;
       jj = rand() % szelesseg;
        if(tomb[ii][jj].mennyi != -1 && ii != 0 && jj != 0 && ii != magassag+2 && jj != szelesseg+2){
            tomb[ii][jj].mennyi = -1;
            aknak--;
        }
    }
}

/*Egy mezõre mind a 8 irányban megállapítja, hogy hány akna van körülötte az alábbi módon: */

/*           É.NY   É   É.K
                \   |   /
                 \  |  /
            NY----mezõ----K
                 / | \
               /   |  \
            D.NY    D   D.K
*/
void dintomb_aknanelkul_megszamol(int ii, int jj, Palya **tomb){
        /* Észak */
       if(tomb[ii-1][jj].mennyi == -1)
            tomb[ii][jj].mennyi++;
       /* Dél */
       if(tomb[ii+1][jj].mennyi == -1)
                tomb[ii][jj].mennyi++;
       /* Kelet */
       if(tomb[ii][jj+1].mennyi == -1)
                tomb[ii][jj].mennyi++;
       /* Nyugat */
       if(tomb[ii][jj-1].mennyi == -1)
                tomb[ii][jj].mennyi++;
       /* Észak-Kelet */
       if(tomb[ii-1][jj+1].mennyi == -1)
                tomb[ii][jj].mennyi++;
       /* Észak-Nyugat */
       if(tomb[ii-1][jj-1].mennyi == -1)
                tomb[ii][jj].mennyi++;
       /* Dél-Kelet */
       if(tomb[ii+1][jj+1].mennyi == -1)
                tomb[ii][jj].mennyi++;
       /* Dél-Nyugat */
       if(tomb[ii+1][jj-1].mennyi == -1)
                tomb[ii][jj].mennyi++;
}

/* Az õsszes mezõre elvégzi a szomszédos aknák megszámolását a dintomb_aknanelkul_megszamol függvénnyel */
void dintomb_aknanelkul_letesz(int magassag, int szelesseg, Palya **tomb){
    int ii, jj;
    for(ii = 1; ii < magassag; ii++){
        for(jj = 1; jj < szelesseg; jj++){
            if(tomb[ii][jj].mennyi != -1){
                dintomb_aknanelkul_megszamol(ii,jj,tomb);
            }
        }
    }
}
