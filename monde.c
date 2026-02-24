#include "monde.h"

#include <stdio.h>
#include <stdlib.h>

#include "journal.h"
#include "utils.h"

//* Là où tout commence, la structure qui contient tout un monde
typedef struct Monde {
    int rayon;
    Case** grille;
    Case** carte_base;
    int nb_cases_explorees;
    int nb_cases_danger;
    int nb_cases_totales;
    int nb_cases_explorables;
    int centre_x;
    int centre_y;
    Decouverte* journal_global;
    int taille_journal;
    int taille_ligne;
    int taille_totale;
} Monde;

//* Pour une carte de la taille du monde à mettre dans un état vierge
void carte_vide(Monde* monde, Case** carte) {
    for (int i = 0; i < monde->taille_ligne; i++) {
        carte[i] = (Case*)malloc(monde->taille_ligne * sizeof(Case));
        for (int j = 0; j < monde->taille_ligne; j++) {
            carte[i][j].type = VIDE;
            carte[i][j].exploree = false;
        }
    }
}

//* Pour afficher une carte de la taille du monde
void print_carte(Monde* monde, Case** carte, int x, int y) {
    printf("\n\nCarte (%p): \n", (void*)carte);
    for (int i = 0; i < monde->taille_ligne; i++) {
        for (int j = 0; j < monde->taille_ligne; j++) {
            if (j == x && i == y) printf(">");
            switch (carte[i][j].type) {
                case VIDE:
                    printf("- ");
                    break;
                case DANGER:
                    printf("D ");
                    break;
                case MUR:
                    printf("* ");
                    break;
                default:
                    break;
            }
        }
        printf("\n");
    }
}

//* Créé un nouveau Monde
Monde* creer_monde(int rayon) {
    Monde* monde = (Monde*)malloc(sizeof(Monde));

    monde->taille_ligne = 2 * rayon + 1;
    monde->taille_totale = monde->taille_ligne * monde->taille_ligne;

    monde->rayon = rayon;
    monde->centre_x = rayon;
    monde->centre_y = rayon;
    monde->nb_cases_explorees = 0;
    monde->nb_cases_danger = 0;
    monde->nb_cases_totales = monde->taille_totale;
    monde->nb_cases_explorables = monde->taille_totale;
    monde->taille_journal = 0;

    monde->grille = (Case**)malloc(monde->taille_ligne * sizeof(Case*));
    monde->carte_base = (Case**)malloc(monde->taille_ligne * sizeof(Case*));

    monde->journal_global = (Decouverte*)malloc(monde->nb_cases_totales * sizeof(Decouverte));

    carte_vide(monde, monde->grille);
    carte_vide(monde, monde->carte_base);

    JOURNAL_INFO("Monde (%p) cree | rayon:%d | taille:%dx%d | cases:%d", (void*)monde, monde->rayon, monde->taille_ligne, monde->taille_ligne, monde->taille_totale);
    return monde;
}

//* Labyrinthe, ou plutot carte telle que toutes les cases différentes d'un mur soient accessibles
static void generer_labyrinthe(Monde* monde) {}

//* Place des murs aléatoirement sauf autour de la base avec une probabilité PROBA_MUR
static void generer_murs_aleatoires(Monde* monde) {
    int taille = monde->taille_ligne;
    int centre_x = monde->centre_x;
    int centre_y = monde->centre_y;
    int nb_murs = 0;

    for (int i = 0; i < taille; i++) {
        for (int j = 0; j < taille; j++) {
            //? Protéger la base et ses 8 voisins immédiats
            if (abs(i - centre_y) <= 1 && abs(j - centre_x) <= 1) continue;
            if (barriere_probabiliste(PROBA_MUR)) {
                monde->grille[i][j].type = MUR;
                nb_murs++;
            }
        }
    }
    JOURNAL_INFO("Murs aleatoires generes | murs:%d (%.1f%%) (monde: %p)", nb_murs, 100.0 * nb_murs / (taille * taille), (void*)monde);
}

//* Génère les murs de la mine
void generer_mine(Monde* monde, bool labyrinthe) {
    if (labyrinthe)
        generer_labyrinthe(monde);
    else
        generer_murs_aleatoires(monde);

    int nb_explorables = 0;
    for (int i = 0; i < monde->taille_ligne; i++)
        for (int j = 0; j < monde->taille_ligne; j++)
            if (monde->grille[i][j].type != MUR) nb_explorables++;
    monde->nb_cases_explorables = nb_explorables;
}

//* Génère les cases "danger" avec une probabilité PROBA_DANGER
void generer_dangers(Monde* monde) {
    int nb_dangers = 0;

    for (int i = 0; i < monde->taille_ligne; i++) {
        for (int j = 0; j < monde->taille_ligne; j++) {
            if (i == monde->centre_y && j == monde->centre_x) continue;
            if (monde->grille[i][j].type == VIDE && barriere_probabiliste(PROBA_DANGER)) {
                monde->grille[i][j].type = DANGER;
                monde->grille[i][j].proba_danger = nombre_autour_de(MILIEU, DEVIATION);
                nb_dangers++;
            }
        }
    }
    monde->nb_cases_danger = nb_dangers;
    JOURNAL_INFO("Dangers generes : %d case(s) sur %d vides (%.1f%%) (monde: %p)", nb_dangers, monde->nb_cases_explorables, 100.0 * nb_dangers / monde->nb_cases_explorables, (void*)monde);
}

//* Génère le monde
void peupler_monde(Monde* monde, bool labyrinthe) {
    JOURNAL_INFO("Début de la génération du terrain - Labyrinthe : %d (monde: %p)", labyrinthe, (void*)monde);
    generer_mine(monde, labyrinthe);
    generer_dangers(monde);
    JOURNAL_INFO("Terrain entièrement généré (monde: %p)", (void*)monde);
}

//* Permet d'enregistrer une découverte dans la carte interne de la base
void ajouter_decouverte(Monde* monde, const Decouverte* decouverte) {
    if (monde->carte_base[decouverte->y][decouverte->x].exploree) return;

    monde->journal_global[monde->taille_journal] = *decouverte;
    monde->carte_base[decouverte->y][decouverte->x].exploree = true;
    monde->carte_base[decouverte->y][decouverte->x].type = decouverte->type;
    monde->nb_cases_explorees++;
    monde->taille_journal++;
}

//* Permet d'ajouter une liste de découvertes à la carte interne de la base
void mettre_a_jour_journal(Monde* monde, Decouverte* nouvelles_decouvertes, int nb_nouvelles) {
    for (int i = 0; i < nb_nouvelles; i++) ajouter_decouverte(monde, &nouvelles_decouvertes[i]);
    if (nb_nouvelles > 0) JOURNAL_INFO("Ajout de %d nouvelles découvertes à la base (monde: %p)", nb_nouvelles, (void*)monde);
}

//* Permet de demander à la base les nouvelles découvertes depuis le dernier passage
void synchroniser_carte_base(Monde* monde, Case** carte, int* index_de_maj) {
    int nb_sync = monde->taille_journal - *index_de_maj;
    for (int i = *index_de_maj; i < monde->taille_journal; i++) {
        Decouverte decouverte = monde->journal_global[i];
        carte[decouverte.y][decouverte.x].type = decouverte.type;
        carte[decouverte.y][decouverte.x].exploree = true;
    }
    *index_de_maj = monde->taille_journal;
    if (nb_sync > 0) JOURNAL_INFO("Carte (%p) synchronisee avec la base: %d nouvelle(s) case(s) (monde: %p)", (void*)carte, nb_sync, (void*)monde);
}

//* Accesseurs
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

int get_nb_cases_explorables(Monde* monde) {
    return monde->nb_cases_explorables;
}

bool est_synchronise(Monde* monde, int dernier_index_de_maj) {
    return dernier_index_de_maj == monde->taille_journal;
}

Case** get_grille_monde(Monde* monde) {
    return monde->grille;
}

Case** get_carte_base(Monde* monde) {
    return monde->carte_base;
}

//* Libère la mémoire du Monde
void detruire_monde(Monde* monde) {
    JOURNAL_INFO("Monde (%p) detruit | rayon:%d | journal:%d entree(s)", (void*)monde, monde->rayon, monde->taille_journal);
    for (int i = 0; i < monde->taille_ligne; i++) {
        free(monde->grille[i]);
        free(monde->carte_base[i]);
    }
    free(monde->grille);
    free(monde->carte_base);
    free(monde->journal_global);
    free(monde);
}