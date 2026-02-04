#ifndef MONDE_H
#define MONDE_H

typedef struct Monde Monde;

typedef struct Case {
    CaseType type;
    bool exploree;
} Case;

typedef enum CaseType {
    VIDE,
    DANGER,
    MUR
    // JOUEUR
} CaseType;

typedef struct Decouverte {
    int x;
    int y;
    CaseType type;
} Decouverte;

void carte_vide(Monde* monde, Case** carte);

Monde* creer_monde(int rayon);
void detruire_monde(Monde* monde);

void peupler_monde(Monde* monde);

void mettre_a_jour_journal(Monde* monde, Decouverte* nouvelles_decouvertes, int nb_nouvelles);
void synchroniser_carte_base(Monde* monde, Case** carte, int* index_de_maj);

int get_rayon(Monde* monde);
int get_centre_x(Monde* monde);
int get_centre_y(Monde* monde);
int get_taille_ligne(Monde* monde);
int get_taille_totale(Monde* monde);
int get_nb_cases_explorees(Monde* monde);

#endif  // !MONDE_H