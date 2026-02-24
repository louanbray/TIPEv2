#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "journal.h"
#include "simulation.h"

void arguments(int argc, char* argv[], bool* log, bool* xlog, bool* print, bool* xprint, int* seed) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--log") == 0) {
            *log = true;
        } else if (strcmp(argv[i], "--xlog") == 0) {
            *log = true;
            *xlog = true;
        } else if (strcmp(argv[i], "--print") == 0) {
            *print = true;
        } else if (strcmp(argv[i], "--xprint") == 0) {
            *print = true;
            *xprint = true;
        } else if (strcmp(argv[i], "--debug") == 0) {
            *log = true;
            *print = true;
        } else if (strcmp(argv[i], "--xdebug") == 0) {
            *log = true;
            *xlog = true;
            *print = true;
            *xprint = true;
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            *seed = atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s [options]\n", argv[0]);
            printf("Options:\n");
            printf("  --log           : Activer la journalisation\n");
            printf("  --xlog          : Activer la journalisation détaillée\n");
            printf("  --print         : Afficher les cartes à la fin de la simulation\n");
            printf("  --xprint        : Afficher les cartes détaillées à la fin de la simulation\n");
            printf("  --debug         : Activer le mode debug (log + print)\n");
            printf("  --xdebug        : Activer le mode debug détaillé (xlog + xprint)\n");
            printf("  --seed <number> : Définir la graine pour la génération aléatoire\n");
            exit(EXIT_SUCCESS);
        }
    }
}

int main(int argc, char* argv[]) {
    bool log = false, xlog = false, print = false, xprint = false;
    int seed = time(NULL);
    arguments(argc, argv, &log, &xlog, &print, &xprint, &seed);
    srand(seed);

    if (log) initialiser_journal();

    Simulation* sim = creer_simulation(20, 200, 50, 500);

    demarre_simulation(sim, false);
    if (print) print_simulation(sim, xprint);

    detruire_simulation(sim);

    if (log) fermer_journal();

    return EXIT_SUCCESS;
}