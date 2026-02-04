#include "monde.h"

#include <stdbool.h>
#include <stdlib.h>

typedef struct Monde {
    int rayon;
    Case** grille;
    Case** carte_base;
    int nb_cases_explorees;
    int nb_cases_danger;
    int nb_cases_totales;
    int nb_cases_a_explorer;
    int centre_x;
    int centre_y;
    Decouverte* journal_global;
    int taille_journal;
    int taille_ligne;
    int taille_totale;
} Monde;

void carte_vide(Monde* monde, Case** carte) {
    for (int i = 0; i < monde->taille_ligne; i++) {
        carte[i] = (Case*)malloc(monde->taille_ligne * sizeof(Case));
        for (int j = 0; j < monde->taille_ligne; j++) {
            carte[i][j].type = VIDE;
            carte[i][j].exploree = false;
        }
    }
}

Monde* creer_monde(int rayon) {
    Monde* monde = (Monde*)malloc(sizeof(Monde));

    monde->taille_ligne = 2 * rayon + 1;
    monde->taille_totale = monde->taille_ligne * monde->taille_ligne;

    monde->rayon = rayon;
    monde->centre_x = rayon;
    monde->centre_y = rayon;
    monde->nb_cases_explorees = 0;
    monde->nb_cases_danger = 0;
    monde->nb_cases_totales = monde->taille_totale;  //! soustraire les murs (cf peupler_monde)
    monde->nb_cases_a_explorer = monde->taille_totale;
    monde->taille_journal = 0;

    monde->grille = (Case**)malloc(monde->taille_ligne * sizeof(Case*));
    monde->carte_base = (Case**)malloc(monde->taille_ligne * sizeof(Case*));

    monde->journal_global = (Decouverte*)malloc(monde->nb_cases_totales * sizeof(Decouverte));

    carte_vide(monde, monde->grille);
    carte_vide(monde, monde->carte_base);

    return monde;
}
//! TODO
void generer_mine(Monde* monde);
void generer_dangers(Monde* monde);

void peupler_monde(Monde* monde) {
    generer_mine(monde);
    generer_dangers(monde);
}

void ajouter_decouverte(Monde* monde, const Decouverte* decouverte) {
    if (monde->carte_base[decouverte->x][decouverte->y].exploree) return;

    monde->journal_global[monde->taille_journal] = *decouverte;
    monde->carte_base[decouverte->x][decouverte->y].exploree = true;
    monde->carte_base[decouverte->x][decouverte->y].type = decouverte->type;
    monde->taille_journal++;
}

void mettre_a_jour_journal(Monde* monde, Decouverte* nouvelles_decouvertes, int nb_nouvelles) {
    for (int i = 0; i < nb_nouvelles; i++) {
        ajouter_decouverte(monde, &nouvelles_decouvertes[i]);
    }
}

void synchroniser_carte_base(Monde* monde, Case** carte, int* index_de_maj) {
    for (int i = *index_de_maj; i < monde->taille_journal; i++) {
        Decouverte decouverte = monde->journal_global[i];
        carte[decouverte.x][decouverte.y].type = decouverte.type;
        carte[decouverte.x][decouverte.y].exploree = true;
    }
    *index_de_maj = monde->taille_journal;
}

int get_rayon(Monde* monde) {
    return monde->rayon;
}

int get_centre_x(Monde* monde) {
    return monde->centre_x;
}

int get_centre_y(Monde* monde) {
    return monde->centre_y;
}

int get_taille_ligne(Monde* monde) {
    return monde->taille_ligne;
}

int get_taille_totale(Monde* monde) {
    return monde->taille_totale;
}

int get_nb_cases_explorees(Monde* monde) {
    return monde->nb_cases_explorees;
}

void detruire_monde(Monde* monde) {
    for (int i = 0; i < monde->taille_ligne; i++) {
        free(monde->grille[i]);
        free(monde->carte_base[i]);
    }
    free(monde->grille);
    free(monde->carte_base);
    free(monde->journal_global);
    free(monde);
}