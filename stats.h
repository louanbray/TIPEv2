#ifndef STATS_H
#define STATS_H

//* Résultats d'une simulation pour un temps de cycle donné
typedef struct Stats {
    int temps_de_cycle;
    // Du point de vue des robots
    int donnees_brutes_transmises;  // Cases que les robots ont remis à la base (avec doublons inter-robots)
    int donnees_brutes_perdues;     // Cases locales non transmises lors de la mort d'un robot (avec doublons)
    int donnees_nettes_perdues;     // Cases perdues et vraiment inconnues de la base (sans doublons)
    // Du point de vue de la base
    int cases_uniques_connues;  // Cases uniques effectivement enregistrées par la base
    int cases_explorables;      // Cases non-mur au total dans le monde
    // État final des robots
    int robots_morts;       // Robots hors-service (autonomie épuisée ou danger)
    int robots_en_attente;  // Robots ayant terminé normalement
} Stats;

//* Affichage lisible (pour --print)
void print_stats(const Stats* stats);

//* Affichage CSV — en-tête puis lignes (pour --analyse)
void print_stats_csv_entete(void);
void print_stats_csv(const Stats* stats);

#endif  // !STATS_H
