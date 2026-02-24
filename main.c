#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "journal.h"
#include "monde.h"
#include "simulation.h"
#include "stats.h"

void arguments(int argc, char* argv[],
               bool* log, bool* xlog, bool* print, bool* xprint,
               bool* analyse, bool* topt,
               int* seed, int* cycle, int* robots, int* rayon, int* autonomie, int* pas, int* repetitions,
               double* proba_danger, double* milieu, double* deviation,
               int* nb_analyses, double* danger_min, double* danger_max, double* danger_pas_d,
               double* milieu_ratio, double* deviation_ratio) {
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
        } else if (strcmp(argv[i], "--topt") == 0) {
            *topt = true;
            // --- Params danger (pour --analyse et simulation simple) ---
        } else if (strcmp(argv[i], "--proba-danger") == 0 && i + 1 < argc) {
            *proba_danger = atof(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--milieu") == 0 && i + 1 < argc) {
            *milieu = atof(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--deviation") == 0 && i + 1 < argc) {
            *deviation = atof(argv[i + 1]);
            i++;
            // --- Params specifiques a --topt ---
        } else if (strcmp(argv[i], "--analyses") == 0 && i + 1 < argc) {
            *nb_analyses = atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--danger-min") == 0 && i + 1 < argc) {
            *danger_min = atof(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--danger-max") == 0 && i + 1 < argc) {
            *danger_max = atof(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--danger-pas") == 0 && i + 1 < argc) {
            *danger_pas_d = atof(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--milieu-ratio") == 0 && i + 1 < argc) {
            *milieu_ratio = atof(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--deviation-ratio") == 0 && i + 1 < argc) {
            *deviation_ratio = atof(argv[i + 1]);
            i++;
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
            printf("  --robots <n>        : Nombre de robots (defaut: 5)\n");
            printf("  --rayon <n>         : Rayon du monde (defaut: 20)\n");
            printf("  --autonomie <n>     : Autonomie initiale (defaut: 500)\n");
            printf("  --analyse           : Analyse CSV sur differents temps de cycle\n");
            printf("  --pas <n>           : Pas du cycle pour --analyse et --topt (defaut: 5)\n");
            printf("  --repetitions <n>   : Simulations par pas (defaut: 100)\n");
            printf("  --proba-danger <f>  : Densite de cases danger (defaut: %.2f)\n", PROBA_DANGER);
            printf("  --milieu <f>        : Letalite moyenne des cases danger (defaut: %.2f)\n", MILIEU);
            printf("  --deviation <f>     : Dispersion de la letalite (defaut: %.2f)\n", DEVIATION);
            printf("  --topt              : Calcule T*(danger) sur plusieurs niveaux de danger\n");
            printf("  --analyses <n>      : Nb d'analyses independantes pour --topt (defaut: 100)\n");
            printf("  --danger-min <f>    : Danger minimum pour --topt (defaut: 0.0)\n");
            printf("  --danger-max <f>    : Danger maximum pour --topt (defaut: 0.30)\n");
            printf("  --danger-pas <f>    : Pas de danger pour --topt (defaut: 0.05)\n");
            printf("  --milieu-ratio <f>  : (topt) milieu = ratio * d pour chaque d ; ecrase --milieu\n");
            printf("  --deviation-ratio <f>: (topt) deviation = ratio * d pour chaque d ; ecrase --deviation\n");
            printf("\nExemples :\n");
            printf("  %s --print\n", argv[0]);
            printf("  %s --seed 42 --cycle 80 --print\n", argv[0]);
            printf("  %s --analyse --seed 42 --proba-danger 0.1 --milieu 0.2 --deviation 0.1 > resultats.csv\n", argv[0]);
            printf("  %s --topt --seed 42 --analyses 100 --repetitions 50 > topt.csv\n", argv[0]);
            printf("  %s --topt --analyses 100 --repetitions 50 --danger-min 0.0 --danger-max 0.30 --danger-pas 0.005 > tstar.csv\n", argv[0]);
            exit(EXIT_SUCCESS);
        }
    }
}

//* Lance une serie de simulations en faisant varier le temps de cycle.
//* Pour chaque cycle, moyenne sur 'repetitions' simulations (seeds : seed+0 .. seed+n-1).
//* Affiche les resultats au format CSV sur stdout (redirigeable vers fichier .csv).
void lance_analyse(int seed, int rayon, int nb_robots, int autonomie, int pas, int repetitions, double proba_danger, double milieu, double deviation) {
    fprintf(stderr, "[Analyse] rayon=%d  robots=%d  autonomie=%d  pas=%d  repetitions=%d  seed=%d\n",
            rayon, nb_robots, autonomie, pas, repetitions, seed);
    fprintf(stderr, "[Analyse] danger=%.3f  milieu=%.3f  deviation=%.3f\n", proba_danger, milieu, deviation);
    print_stats_csv_moyenne_entete();
    for (int cycle = pas; cycle <= autonomie; cycle += pas) {
        StatsMoyenne stats_moyenne = {0};
        stats_moyenne.temps_de_cycle = cycle;
        for (int rep = 0; rep < repetitions; rep++) {
            srand(seed + rep);
            Simulation* sim = creer_simulation(rayon, nb_robots, cycle, autonomie);
            demarre_simulation(sim, false, proba_danger, milieu, deviation);
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

//* Pour chaque niveau de danger, calcule T* = le temps de cycle qui maximise exploration_pct.
//* T* est obtenu en moyennant le pic de 'nb_analyses' courbes independantes (seeds distincts).
//* milieu/deviation sont fixes sauf si milieu_ratio/deviation_ratio >= 0 (alors ratio * d).
//* Affiche un CSV : proba_danger, milieu, deviation, topt_moyen
void lance_topt(int seed, int rayon, int nb_robots, int autonomie, int pas, int repetitions,
                int nb_analyses, double danger_min, double danger_max, double danger_pas_d,
                double milieu_fixe, double deviation_fixe,
                double milieu_ratio, double deviation_ratio) {
    fprintf(stderr, "[T-opt] rayon=%d  robots=%d  autonomie=%d  pas=%d  rep=%d  analyses=%d\n", rayon, nb_robots, autonomie, pas, repetitions, nb_analyses);
    if (milieu_ratio >= 0 || deviation_ratio >= 0)
        fprintf(stderr, "[T-opt] danger %.3f->%.3f (step=%.3f)  milieu=%s  deviation=%s\n",
                danger_min, danger_max, danger_pas_d,
                milieu_ratio >= 0 ? "ratio*d" : "fixe",
                deviation_ratio >= 0 ? "ratio*d" : "fixe");
    else
        fprintf(stderr, "[T-opt] danger %.3f->%.3f (step=%.3f)  milieu=%.3f (fixe)  deviation=%.3f (fixe)\n", danger_min, danger_max, danger_pas_d, milieu_fixe, deviation_fixe);
    printf("proba_danger,milieu,deviation,topt_moyen\n");

    for (double d = danger_min; d <= danger_max + 1e-9; d += danger_pas_d) {
        double milieu = (milieu_ratio >= 0) ? milieu_ratio * d : milieu_fixe;
        double deviation = (deviation_ratio >= 0) ? deviation_ratio * d : deviation_fixe;
        double somme_topt = 0.0;

        for (int a = 0; a < nb_analyses; a++) {
            //* Chaque analyse utilise une famille de seeds distincte
            int seed_a = seed + a * repetitions;
            double best_exploration = -1.0;
            int best_cycle = pas;

            for (int cycle = pas; cycle <= autonomie; cycle += pas) {
                StatsMoyenne sm = {0};
                sm.temps_de_cycle = cycle;
                for (int rep = 0; rep < repetitions; rep++) {
                    srand(seed_a + rep);
                    Simulation* sim = creer_simulation(rayon, nb_robots, cycle, autonomie);
                    demarre_simulation(sim, false, d, milieu, deviation);
                    Stats stats = get_stats_simulation(sim);
                    accumuler_stats(&sm, &stats);
                    detruire_simulation(sim);
                }
                moyenner_stats(&sm, repetitions);
                double exploration = sm.cases_explorables > 0 ? 100.0 * sm.cases_uniques_connues / sm.cases_explorables : 0.0;
                if (exploration > best_exploration) {
                    best_exploration = exploration;
                    best_cycle = cycle;
                }
            }
            somme_topt += best_cycle;
        }

        double topt_moyen = somme_topt / nb_analyses;
        printf("%.4f,%.4f,%.4f,%.2f\n", d, milieu, deviation, topt_moyen);
        fflush(stdout);
        fprintf(stderr, "  danger=%.3f -> T*=%.1f\n", d, topt_moyen);
    }
    fprintf(stderr, "[T-opt] Termine.\n");
}

int main(int argc, char* argv[]) {
    bool log = false, xlog = false, print = false, xprint = false, analyse = false, topt = false;
    int seed = time(NULL);
    int cycle = 50, robots = 5, rayon = 20, autonomie = 500, pas = 5, repetitions = 100;
    double proba_danger = PROBA_DANGER, milieu = MILIEU, deviation = DEVIATION;
    int nb_analyses = 100;
    double danger_min = 0.0, danger_max = 0.30, danger_pas_d = 0.05;
    double milieu_ratio = -1.0, deviation_ratio = -1.0;  // -1 = non active, utilise --milieu/--deviation fixes

    arguments(argc, argv,
              &log, &xlog, &print, &xprint,
              &analyse, &topt,
              &seed, &cycle, &robots, &rayon, &autonomie, &pas, &repetitions,
              &proba_danger, &milieu, &deviation,
              &nb_analyses, &danger_min, &danger_max, &danger_pas_d,
              &milieu_ratio, &deviation_ratio);
    srand(seed);

    if (log) initialiser_journal();

    if (topt) {
        lance_topt(seed, rayon, robots, autonomie, pas, repetitions,
                   nb_analyses, danger_min, danger_max, danger_pas_d,
                   milieu, deviation,
                   milieu_ratio, deviation_ratio);
    } else if (analyse) {
        lance_analyse(seed, rayon, robots, autonomie, pas, repetitions, proba_danger, milieu, deviation);
    } else {
        Simulation* sim = creer_simulation(rayon, robots, cycle, autonomie);
        demarre_simulation(sim, false, proba_danger, milieu, deviation);
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