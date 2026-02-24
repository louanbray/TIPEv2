#include "stats.h"

#include <stdio.h>

//* Mode --print
void print_stats(const Stats* stats) {
    int brut_total = stats->donnees_brutes_transmises + stats->donnees_brutes_perdues;
    double retention = brut_total > 0 ? 100.0 * stats->donnees_brutes_transmises / brut_total : 0.0;
    double exploration = stats->cases_explorables > 0 ? 100.0 * stats->cases_uniques_connues / stats->cases_explorables : 0.0;

    printf("\n=== Stats (cycle=%d) ===\n", stats->temps_de_cycle);
    printf("  -- Brut (du point de vue des robots) --\n");
    printf("  Donnees brutes transmises : %d\n", stats->donnees_brutes_transmises);
    printf("  Donnees brutes perdues    : %d\n", stats->donnees_brutes_perdues);
    printf("  Taux de retention brut    : %.1f%%\n", retention);
    printf("  -- Net (du point de vue de la base) --\n");
    printf("  Donnees nettes perdues    : %d\n", stats->donnees_nettes_perdues);
    printf("  Cases uniques connues     : %d / %d\n", stats->cases_uniques_connues, stats->cases_explorables);
    printf("  Taux d'exploration        : %.1f%%\n", exploration);
    printf("  -- Robots --\n");
    printf("  Robots morts              : %d\n", stats->robots_morts);
    printf("  Robots en attente         : %d\n", stats->robots_en_attente);
    printf("========================\n");
}

//* En-tete CSV (mode --analyse)
void print_stats_csv_entete() {
    printf("cycle,brutes_transmises,brutes_perdues,retention_brut_pct,nettes_perdues,cases_uniques,cases_explorables,exploration_pct,morts,en_attente\n");
}

//* Ligne CSV (mode --analyse)
void print_stats_csv(const Stats* stats) {
    int brut_total = stats->donnees_brutes_transmises + stats->donnees_brutes_perdues;
    double retention = brut_total > 0 ? 100.0 * stats->donnees_brutes_transmises / brut_total : 0.0;
    double exploration = stats->cases_explorables > 0 ? 100.0 * stats->cases_uniques_connues / stats->cases_explorables : 0.0;

    printf("%d,%d,%d,%.2f,%d,%d,%d,%.2f,%d,%d\n",
           stats->temps_de_cycle,
           stats->donnees_brutes_transmises,
           stats->donnees_brutes_perdues,
           retention,
           stats->donnees_nettes_perdues,
           stats->cases_uniques_connues,
           stats->cases_explorables,
           exploration,
           stats->robots_morts,
           stats->robots_en_attente);
}

//* Ajoute les stats dans l'accumulateur
void accumuler_stats(StatsMoyenne* stats_moyenne, const Stats* stats) {
    stats_moyenne->donnees_brutes_transmises += stats->donnees_brutes_transmises;
    stats_moyenne->donnees_brutes_perdues += stats->donnees_brutes_perdues;
    stats_moyenne->donnees_nettes_perdues += stats->donnees_nettes_perdues;
    stats_moyenne->cases_uniques_connues += stats->cases_uniques_connues;
    stats_moyenne->cases_explorables += stats->cases_explorables;
    stats_moyenne->robots_morts += stats->robots_morts;
    stats_moyenne->robots_en_attente += stats->robots_en_attente;
}

//* Divise tous les champs par le nombre de répétitions pour obtenir la moyenne
void moyenner_stats(StatsMoyenne* stats_moyenne, int nb_repetitions) {
    if (nb_repetitions <= 0) return;
    stats_moyenne->donnees_brutes_transmises /= nb_repetitions;
    stats_moyenne->donnees_brutes_perdues /= nb_repetitions;
    stats_moyenne->donnees_nettes_perdues /= nb_repetitions;
    stats_moyenne->cases_uniques_connues /= nb_repetitions;
    stats_moyenne->cases_explorables /= nb_repetitions;
    stats_moyenne->robots_morts /= nb_repetitions;
    stats_moyenne->robots_en_attente /= nb_repetitions;
    stats_moyenne->nb_repetitions = nb_repetitions;
}

//* En-tete CSV moyenne (mode --analyse avec repetitions)
void print_stats_csv_moyenne_entete() {
    printf("cycle,repetitions,brutes_transmises,brutes_perdues,retention_brut_pct,nettes_perdues,cases_uniques,cases_explorables,exploration_pct,morts,en_attente\n");
}

//* Ligne CSV moyenne
void print_stats_csv_moyenne(const StatsMoyenne* s) {
    double brut_total = s->donnees_brutes_transmises + s->donnees_brutes_perdues;
    double retention = brut_total > 0 ? 100.0 * s->donnees_brutes_transmises / brut_total : 0.0;
    double exploration = s->cases_explorables > 0 ? 100.0 * s->cases_uniques_connues / s->cases_explorables : 0.0;

    printf("%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
           s->temps_de_cycle,
           s->nb_repetitions,
           s->donnees_brutes_transmises,
           s->donnees_brutes_perdues,
           retention,
           s->donnees_nettes_perdues,
           s->cases_uniques_connues,
           s->cases_explorables,
           exploration,
           s->robots_morts,
           s->robots_en_attente);
}
