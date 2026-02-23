#include "simulation.h"

#include <stdio.h>
#include <stdlib.h>

#include "journal.h"
#include "robot.h"

//* Donne l'état de la simulation
typedef enum EtatSimulation {
    BLANK,
    EN_COURS,
    EN_PAUSE,
    TERMINEE
} EtatSimulation;

//* La matrice. Structure contenant tout un monde simulé
typedef struct Simulation {
    Monde* monde;
    Robot** robots;
    bool* robotsEnVie;
    int nombreDeRobotsEnVie;
    int nombreDeRobots;
    // Stats* stats;
    int idSimulation;
    EtatSimulation etat;
} Simulation;

//* Créé une simulation contenant un monde de rayon 'rayon' contenant une distribution de 'nombreRobots' robots ayant un certain temps_de_cycle et une certaine autonomie
Simulation* creer_simulation(int rayon, int nombreRobots, int temps_de_cycle, int autonomie_initiale) {
    Simulation* simulation = malloc(sizeof(Simulation));

    Monde* monde = creer_monde(rayon);

    Robot** robots = malloc(sizeof(Robot*) * nombreRobots);
    bool* robotsEnVie = malloc(sizeof(bool) * nombreRobots);

    for (int i = 0; i < nombreRobots; i++) {
        robots[i] = creer_robot(monde, temps_de_cycle, autonomie_initiale);
        robotsEnVie[i] = true;
    }

    simulation->monde = monde;
    simulation->robots = robots;
    simulation->robotsEnVie = robotsEnVie;
    simulation->nombreDeRobots = nombreRobots;
    simulation->nombreDeRobotsEnVie = nombreRobots;
    simulation->etat = BLANK;

    simulation->idSimulation = rand();

    JOURNAL_INFO("Nouvelle simulation créée (ID:%d)", simulation->idSimulation);

    return simulation;
}

//* Simule l'avancement du monde à l'instant suivant
void simule_avancement(Simulation* simulation) {
    if (simulation->etat != EN_COURS) return;
    for (int i = 0; i < simulation->nombreDeRobots; i++) {
        if (simulation->robotsEnVie[i]) {
            if (actualiser_robot(simulation->robots[i]) == 0) {
                simulation->robotsEnVie[i] = false;
                simulation->nombreDeRobotsEnVie--;
            }
        }
    }
    //! SI EXPLORATION FINIE : TERMINER LA SIMULATION
    if (simulation->nombreDeRobotsEnVie <= 0) simulation->etat = TERMINEE;
}

//* Boucle de simulation. Renvoie 1 quand elle est terminée
int boucle_principale(Simulation* simulation) {
    while (simulation->etat != TERMINEE) {
        if (simulation->etat == EN_PAUSE) continue;
        simule_avancement(simulation);
    }

    JOURNAL_INFO("Fin de la simulation (%d)", simulation->idSimulation);

    return 1;
}

//* Lance la simulation
int demarre_simulation(Simulation* simulation) {
    if (simulation->etat != BLANK) return -1;

    peupler_monde(simulation->monde);
    simulation->etat = EN_COURS;

    JOURNAL_INFO("Début de la simulation (%d)", simulation->idSimulation);

    return 1;
    return boucle_principale(simulation);
}

//* Libère la mémoire de la simulation et de tous ses composants
void detruire_simulation(Simulation* simulation) {
    if (simulation == NULL) return;

    for (int i = 0; i < simulation->nombreDeRobots; i++) {
        if (simulation->robots[i] != NULL) {
            detruire_robot(simulation->robots[i]);
        }
    }
    free(simulation->robots);
    free(simulation->robotsEnVie);

    detruire_monde(simulation->monde);

    JOURNAL_INFO("Simulation détruite (ID:%d)", simulation->idSimulation);

    free(simulation);
}

//* Affiche les cartes de tout les simulés
void print_simulation(Simulation* simulation) {
    printf("\n\n\n\n-----------------------------------------------------");
    printf("\n              Simulation (ID:%d)", simulation->idSimulation);
    printf("\n-----------------------------------------------------");
    printf("\n\n\n----------------------- Monde -----------------------");
    print_carte(simulation->monde, get_grille_monde(simulation->monde), -1, -1);
    printf("\n\n\n-------------------- Carte(Base) --------------------");
    print_carte(simulation->monde, get_carte_base(simulation->monde), -1, -1);
    for (int i = 0; i < simulation->nombreDeRobots; i++) {
        if (!simulation->robotsEnVie[i]) {
            printf("\n\n\nRobot n°%d est défaillant", i + 1);
            continue;
        }
        printf("\n\n\n----------------- Carte(Robot n°%d) -----------------", i + 1);
        Robot* robot = simulation->robots[i];
        print_carte(simulation->monde, get_carte_robot(robot), get_robot_x(robot), get_robot_y(robot));
    }
}