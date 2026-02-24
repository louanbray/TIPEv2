#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "journal.h"
#include "simulation.h"
#include "stats.h"

void arguments(int argc, char* argv[], bool* log, bool* xlog, bool* print, bool* xprint, bool* analyse, int* seed, int* cycle, int* robots, int* rayon, int* autonomie, int* pas, int* repetitions) {
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
        } else if (strcmp(argv[i], "--repetitions") == 0 && i + 1 < argc) {
            *repetitions = atoi(argv[i + 1]);
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
            printf("  --repetitions <n>   : Simulations par pas pour --analyse (defaut: 100)\n");
            printf("\nExemples :\n");
            printf("  %s --print\n", argv[0]);
            printf("  %s --seed 42 --cycle 80 --print\n", argv[0]);
            printf("  %s --analyse --seed 42 > resultats.csv\n", argv[0]);
            exit(EXIT_SUCCESS);
        }
    }
}

//* Lance une serie de simulations en faisant varier le temps de cycle.
//* Pour chaque cycle, moyenne sur 'repetitions' simulations (seeds : seed+0 .. seed+n-1).
//* Affiche les resultats au format CSV sur stdout (redirigeable vers fichier .csv).
void lance_analyse(int seed, int rayon, int nb_robots, int autonomie, int pas, int repetitions) {
    fprintf(stderr, "[Analyse] rayon=%d  robots=%d  autonomie=%d  pas=%d  repetitions=%d  seed=%d\n", rayon, nb_robots, autonomie, pas, repetitions, seed);
    print_stats_csv_moyenne_entete();
    for (int cycle = pas; cycle <= autonomie; cycle += pas) {
        StatsMoyenne stats_moyenne = {0};
        stats_moyenne.temps_de_cycle = cycle;
        for (int rep = 0; rep < repetitions; rep++) {
            srand(seed + rep);
            Simulation* sim = creer_simulation(rayon, nb_robots, cycle, autonomie);
            demarre_simulation(sim, false);
            Stats stats = get_stats_simulation(sim);
            accumuler_stats(&stats_moyenne, &stats);
            detruire_simulation(sim);
        }
        moyenner_stats(&stats_moyenne, repetitions);
        print_stats_csv_moyenne(&stats_moyenne);
        fflush(stdout);
        fprintf(stderr, "  cycle=%4d termine\n", cycle);
    }
    fprintf(stderr, "[Analyse] Termine.\n");
}

int main(int argc, char* argv[]) {
    bool log = false, xlog = false, print = false, xprint = false, analyse = false;
    int seed = time(NULL);
    int cycle = 50, robots = 5, rayon = 20, autonomie = 500, pas = 5, repetitions = 100;
    arguments(argc, argv, &log, &xlog, &print, &xprint, &analyse, &seed, &cycle, &robots, &rayon, &autonomie, &pas, &repetitions);
    srand(seed);

    if (log) initialiser_journal();

    if (analyse) {
        lance_analyse(seed, rayon, robots, autonomie, pas, repetitions);
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