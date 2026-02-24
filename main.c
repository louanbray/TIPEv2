#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "journal.h"
#include "simulation.h"
#include "stats.h"

void arguments(int argc, char* argv[], bool* log, bool* xlog, bool* print, bool* xprint, bool* analyse, int* seed, int* cycle, int* robots, int* rayon, int* autonomie, int* pas) {
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
        } else if (strcmp(argv[i], "--cycle") == 0 && i + 1 < argc) {
            *cycle = atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--robots") == 0 && i + 1 < argc) {
            *robots = atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--rayon") == 0 && i + 1 < argc) {
            *rayon = atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--autonomie") == 0 && i + 1 < argc) {
            *autonomie = atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--pas") == 0 && i + 1 < argc) {
            *pas = atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--analyse") == 0) {
            *analyse = true;
        } else if (strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s [options]\n", argv[0]);
            printf("Options:\n");
            printf("  --log               : Activer la journalisation\n");
            printf("  --xlog              : Activer la journalisation detaillee\n");
            printf("  --print             : Afficher les cartes et stats a la fin\n");
            printf("  --xprint            : Afficher les cartes detaillees a la fin\n");
            printf("  --debug             : log + print\n");
            printf("  --xdebug            : xlog + xprint\n");
            printf("  --seed <n>          : Graine aleatoire (defaut: time)\n");
            printf("  --cycle <n>         : Temps de cycle (defaut: 50)\n");
            printf("  --robots <n>        : Nombre de robots (defaut: 200)\n");
            printf("  --rayon <n>         : Rayon du monde (defaut: 20)\n");
            printf("  --autonomie <n>     : Autonomie initiale (defaut: 500)\n");
            printf("  --analyse           : Analyse CSV sur differents temps de cycle\n");
            printf("  --pas <n>           : Pas du cycle pour --analyse (defaut: 5)\n");
            printf("\nExemples :\n");
            printf("  %s --print\n", argv[0]);
            printf("  %s --seed 42 --cycle 80 --print\n", argv[0]);
            printf("  %s --analyse --seed 42 > resultats.csv\n", argv[0]);
            exit(EXIT_SUCCESS);
        }
    }
}

//* Lance une serie de simulations en faisant varier le temps de cycle.
//* Affiche les resultats au format CSV sur stdout (redirigeable vers fichier .csv).
void lance_analyse(int seed, int rayon, int nb_robots, int autonomie, int pas) {
    fprintf(stderr, "[Analyse] rayon=%d  robots=%d  autonomie=%d  pas=%d  seed=%d\n", rayon, nb_robots, autonomie, pas, seed);
    print_stats_csv_entete();
    for (int cycle = pas; cycle <= autonomie; cycle += pas) {
        srand(seed);

        Simulation* sim = creer_simulation(rayon, nb_robots, cycle, autonomie);
        demarre_simulation(sim, false);

        Stats stats = get_stats_simulation(sim);
        print_stats_csv(&stats);

        fflush(stdout);
        detruire_simulation(sim);

        fprintf(stderr, "  cycle=%4d termine\n", cycle);
    }
    fprintf(stderr, "[Analyse] Termine.\n");
}

int main(int argc, char* argv[]) {
    bool log = false, xlog = false, print = false, xprint = false, analyse = false;
    int seed = time(NULL);
    int cycle = 50, robots = 200, rayon = 20, autonomie = 500, pas = 5;
    arguments(argc, argv, &log, &xlog, &print, &xprint, &analyse, &seed, &cycle, &robots, &rayon, &autonomie, &pas);
    srand(seed);

    if (log) initialiser_journal();

    if (analyse) {
        lance_analyse(seed, rayon, robots, autonomie, pas);
    } else {
        Simulation* sim = creer_simulation(rayon, robots, cycle, autonomie);
        demarre_simulation(sim, false);
        if (print) {
            print_simulation(sim, xprint);
            Stats stats = get_stats_simulation(sim);
            print_stats(&stats);
        }
        detruire_simulation(sim);
    }

    if (log) fermer_journal();

    return EXIT_SUCCESS;
}