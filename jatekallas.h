#ifndef JATEKALLAS_H_INCLUDED
#define JATEKALLAS_H_INCLUDED
/*Maga a pálya struktúrája*/
typedef struct Palya{
    /*Jelzi, hogy fel van-e fedve egy mezõ*/
    bool felfedve;
    bool zaszlos;
    /*Minden mezõ rendelkezik egy ilyen értékkel;
    Ha akna van benne akkor ez = -1,
    Ha üres és nincs a környezetében akna, akkor = 0,
    Ha üres és van a környezetében akna, akkor megadja azok számát. */
    int mennyi;
}Palya;

/* Dicsőséglista struktúrája */
typedef struct Dicsoseglista{
    char becenev[51];
    int ido;
    int nehezseg;
}Dicsoseglista;

/* Elvégzi az inicializálást. */
void inic(int *magassag, int *szelesseg, int *aknak, int *nehezseg);

void dicsoseglista_beleir(int nehezseg, int ido);

Dicsoseglista* dicsoseglista_foglal(void);

/* Eltelt idő szerint rendez */
void dicsoseglista_rendez(Dicsoseglista *lista, int meret);

void dicsoseglista_becenevek(Dicsoseglista *lista, int nehezseg, int meret);

void dicsoseglista_kiir(Dicsoseglista *lista, int meret);

void dicsoseglista_elkeszit(Dicsoseglista *lista);

void nehezseg_kivalaszt(int valasz, int* magassag, int* szelesseg, int* aknak);

Palya** dintomb_foglal(int magassag, int szelesseg);

void dintomb_felszabadit(Palya **tomb);

/* A Palya típusú tömb "mennyi" kezdeti értékét nullára állítja. */
void dintomb_nullaz(int  magassag, int szelesseg, Palya **tomb);

/* Véletlenszerûen lehelyez aknákat a tömbben, olyan helyekre, ahol még nincs. */
void dintomb_akna_letesz(int magassag, int szelesseg, int aknak, Palya **tomb);

/* Az õsszes mezõre elvégzi a szomszédos aknák megszámolását a dintomb_aknanelkul_megszamol függvénnyel */
void dintomb_aknanelkul_megszamol(int ii, int jj, Palya **tomb);

/* Egy mezõre mind a 8 irányban megállapítja, hogy hány akna van körülötte az alábbi módon: */
void dintomb_aknanelkul_letesz(int magassag, int szelesseg, Palya **tomb);

#endif
