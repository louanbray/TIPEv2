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
void print_stats_csv_entete(void) {
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
