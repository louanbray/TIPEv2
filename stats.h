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

//? Inutile [
/////* Affichage CSV d'une simulation unique
// void print_stats_csv_entete();
// void print_stats_csv(const Stats* stats);
//? ]

//* Permet d'accumuler des stats pour en faire la moyenne
typedef struct StatsMoyenne {
    int temps_de_cycle;
    int nb_repetitions;
    double donnees_brutes_transmises;
    double donnees_brutes_perdues;
    double donnees_nettes_perdues;
    double cases_uniques_connues;
    double cases_explorables;
    double robots_morts;
    double robots_en_attente;
} StatsMoyenne;

void accumuler_stats(StatsMoyenne* stats_moyenne, const Stats* stats);
void moyenner_stats(StatsMoyenne* stats_moyenne, int nb_repetitions);

void print_stats_csv_moyenne_entete();
void print_stats_csv_moyenne(const StatsMoyenne* stats_moyenne);

#endif  // !STATS_H
