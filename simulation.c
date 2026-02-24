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
    bool* robotsEnAttente;
    int nombreDeRobotsEnAttente;
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
    bool* robotsEnAttente = calloc(sizeof(bool), nombreRobots);

    for (int i = 0; i < nombreRobots; i++) {
        robots[i] = creer_robot(monde, temps_de_cycle, autonomie_initiale);
        robotsEnVie[i] = true;
    }

    simulation->monde = monde;
    simulation->robots = robots;
    simulation->robotsEnVie = robotsEnVie;
    simulation->robotsEnAttente = robotsEnAttente;
    simulation->nombreDeRobots = nombreRobots;
    simulation->nombreDeRobotsEnAttente = 0;
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
        if (!simulation->robotsEnVie[i]) continue;
        if (simulation->robotsEnAttente[i]) continue;
        EtatRobot etat_robot = actualiser_robot(simulation->robots[i]);
        if (etat_robot == HORS_SERVICE) {
            simulation->robotsEnVie[i] = false;
            simulation->nombreDeRobotsEnVie--;
            JOURNAL_AVERT("Robot (%p) hors service | robots restants : %d/%d", (void*)simulation->robots[i], simulation->nombreDeRobotsEnVie, simulation->nombreDeRobots);
        } else if (etat_robot == EN_ATTENTE) {
            simulation->robotsEnAttente[i] = true;
            simulation->nombreDeRobotsEnAttente++;
            JOURNAL_INFO("Robot (%p) a fini son exploration | robots en veille : %d/%d", (void*)simulation->robots[i], simulation->nombreDeRobotsEnAttente, simulation->nombreDeRobots);
        }
    }

    if (simulation->nombreDeRobotsEnVie <= 0) {
        simulation->etat = TERMINEE;
        JOURNAL_INFO("Simulation terminée (ID:%d) | tous les robots sont hors service", simulation->idSimulation);
    } else if (simulation->nombreDeRobotsEnAttente >= simulation->nombreDeRobotsEnVie) {
        simulation->etat = TERMINEE;
        JOURNAL_INFO("Simulation terminée (ID:%d) | toutes les zones accessibles ont été explorées", simulation->idSimulation);
    }
}

//* Boucle de simulation. Renvoie 1 quand elle est terminée
int boucle_principale(Simulation* simulation) {
    while (simulation->etat != TERMINEE) {
        if (simulation->etat == EN_PAUSE) continue;
        simule_avancement(simulation);
    }

    JOURNAL_INFO("Fin de la simulation (ID:%d)", simulation->idSimulation);

    return 1;
}

//* Lance la simulation
int demarre_simulation(Simulation* simulation, bool labyrinthe) {
    if (simulation->etat != BLANK) return -1;

    peupler_monde(simulation->monde, labyrinthe);
    simulation->etat = EN_COURS;

    JOURNAL_INFO("Début de la simulation (ID:%d)", simulation->idSimulation);

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
void print_simulation(Simulation* simulation, bool details) {
    Monde* monde = simulation->monde;
    printf("\n\n\n\n-----------------------------------------------------");
    printf("\n              Simulation (ID:%d)", simulation->idSimulation);
    printf("\n-----------------------------------------------------");
    printf("\n\n\n----------------------- Monde -----------------------");
    print_carte(monde, get_grille_monde(monde), get_centre_x(monde), get_centre_y(monde));
    printf("\n\n\n-------------------- Carte(Base) --------------------");
    print_carte(monde, get_carte_base(monde), -1, -1);
    if (!details) return;
    for (int i = 0; i < simulation->nombreDeRobots; i++) {
        Robot* robot = simulation->robots[i];
        if (!simulation->robotsEnVie[i])
            printf("\n\n\n----------------- Carte(Robot défaillant %p) -----------------", (void*)robot);
        else
            printf("\n\n\n----------------- Carte(Robot %p) -----------------", (void*)robot);
        print_carte(monde, get_carte_robot(robot), get_robot_x(robot), get_robot_y(robot));
    }
}