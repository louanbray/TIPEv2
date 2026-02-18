#ifndef MONDE_H
#define MONDE_H
#include <stdbool.h>

//* Utile pour la dispertion de danger dans le monde
#define PROBA_DANGER 0.1
#define MILIEU 0.2
#define DEVIATION 0.1

typedef struct Monde Monde;

//* Permet de définir le type de case
typedef enum CaseType {
    VIDE,
    DANGER,
    MUR
    // JOUEUR
} CaseType;

//* Partie élémentaire d'un monde ou d'une carte
typedef struct Case {
    CaseType type;
    bool exploree;
    double proba_danger;
} Case;

//* Représente une nouvelle découverte en (x, y) de CaseType type
typedef struct Decouverte {
    int x;
    int y;
    CaseType type;
} Decouverte;

//* Pour une carte de la taille du monde à mettre dans un état vierge
void carte_vide(Monde* monde, Case** carte);
//* Pour afficher une carte de la taille du monde
void print_carte(Monde* monde, Case** carte, int x, int y);

//* Créé un nouveau Monde
Monde* creer_monde(int rayon);
//* Libère la mémoire du Monde
void detruire_monde(Monde* monde);

//! TODO -> SUPPRIMER CES DEUX LIGNES (FONCITONS INTERNES)
void generer_mine(Monde* monde);
void generer_dangers(Monde* monde);

//* Génère le monde
void peupler_monde(Monde* monde);
//* Permet d'ajouter une liste de découvertes à la carte interne de la base
void mettre_a_jour_journal(Monde* monde, Decouverte* nouvelles_decouvertes, int nb_nouvelles);
//* Permet de demander à la base les nouvelles découvertes depuis le dernier passage
void synchroniser_carte_base(Monde* monde, Case** carte, int* index_de_maj);

//* Accesseurs
int get_rayon(Monde* monde);
int get_centre_x(Monde* monde);
int get_centre_y(Monde* monde);
int get_taille_ligne(Monde* monde);
int get_taille_totale(Monde* monde);
int get_nb_cases_explorees(Monde* monde);

Case** get_grille_monde(Monde* monde);
Case** get_carte_base(Monde* monde);

#endif  // !MONDE_H