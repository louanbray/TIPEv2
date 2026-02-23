#include "robot.h"

#include <stdbool.h>
#include <stdlib.h>

#include "journal.h"

//* Etat du robot dans sa routine
typedef enum EtatRobot {
    EXPLORATION,
    RETOUR,
    RECHERCHE_INEXPLORE,
    TRANSFERT_DE_DONNEE
} EtatRobot;

//* La structure contenant les premiers habitants de ce monde: les robots
typedef struct Robot {
    Monde* monde;
    Case** carte;
    Decouverte* journal_local;
    EtatRobot etat;
    int nombre_decouvertes;
    int dernier_index_de_maj;
    int x;
    int y;
    int autonomie;
    int timer_de_retour;
    int temps_de_cycle;
    bool alive;
} Robot;

//* Permet de faire apparaitre un robot sur un monde
Robot* creer_robot(Monde* monde, int temps_de_cycle, int autonomie_initiale) {
    Robot* robot = (Robot*)malloc(sizeof(Robot));

    robot->monde = monde;
    robot->carte = (Case**)malloc(get_taille_ligne(monde) * sizeof(Case*));
    carte_vide(monde, robot->carte);

    robot->journal_local = (Decouverte*)malloc(temps_de_cycle * sizeof(Decouverte));

    robot->etat = EXPLORATION;
    robot->nombre_decouvertes = 0;
    robot->dernier_index_de_maj = 0;

    robot->x = get_centre_x(monde);
    robot->y = get_centre_y(monde);

    robot->autonomie = autonomie_initiale;
    robot->timer_de_retour = temps_de_cycle;  //! à gérer correctement TODO
    robot->temps_de_cycle = temps_de_cycle;
    robot->alive = true;

    JOURNAL_INFO("Robot (%p) cree en (%d,%d) | autonomie:%d | cycle:%d", (void*)robot, robot->x, robot->y, robot->autonomie, robot->temps_de_cycle);
    return robot;
}

//* Permet à un robot de se synchroniser à la carte de la base du monde qu'il explore
void synchroniser_robot(Robot* robot) {
    JOURNAL_INFO("Robot (%p) synchronisation : %d decouverte(s) transferee(s)", (void*)robot, robot->nombre_decouvertes);
    mettre_a_jour_journal(robot->monde, robot->journal_local, robot->nombre_decouvertes);
    synchroniser_carte_base(robot->monde, robot->carte, &robot->dernier_index_de_maj);
    robot->nombre_decouvertes = 0;
    robot->etat = EXPLORATION;
}

//* Réduis l'autonomie du robot et modifie son état s'il est à cours de batterie
void degrade_robot(Robot* robot) {
    robot->autonomie--;
    if (robot->autonomie <= 0) {
        robot->alive = false;
        JOURNAL_AVERT("Robot (%p) hors service en (%d,%d) | autonomie epuisee", (void*)robot, robot->x, robot->y);
    }
}

//* A appeler après chaque déplacement en phase d'exploration, gère le changement de phase vers retour
void decremente_timer_robot(Robot* robot) {
    robot->timer_de_retour--;
    if (robot->timer_de_retour <= 0) {
        robot->timer_de_retour = robot->temps_de_cycle;
        robot->etat = RETOUR;
        JOURNAL_INFO("Robot (%p) passe en phase RETOUR depuis (%d,%d)", (void*)robot, robot->x, robot->y);
    }
}

//* Routine du robot (renvoie 1 si le robot est en vie, 0 sinon)
//! TODO : implémenter l'exploration et le retour
int actualiser_robot(Robot* robot) {
    EtatRobot etat = robot->etat;
    switch (etat) {
        case EXPLORATION:
            //! TODO : implémenter l'exploration
            break;
        case RETOUR:
            //! TODO : implémenter le retour vers la base
            break;
        case RECHERCHE_INEXPLORE:
            //! TODO : implémenter la recherche d'une zone inexplorée pour reprendre l'exploration
            break;
        case TRANSFERT_DE_DONNEE:
            //! TODO : transférer les données à la base et switch vers RECHERCHE_INEXPLORE
            break;
        default:
            break;
    }

    if (robot->alive) {
        if (etat == EXPLORATION) decremente_timer_robot(robot);
        if (etat != TRANSFERT_DE_DONNEE) degrade_robot(robot);
    }
    if (!robot->alive)
        return 0;  //! Implémenter la mort du robot + les stats de données perdues etc...
    return 1;
}

//* Accesseurs
int get_robot_x(Robot* robot) {
    return robot->x;
}

int get_robot_y(Robot* robot) {
    return robot->y;
}

//* Permet d'accéder à la carte interne du robot
//? Utilisé pour débug
Case** get_carte_robot(Robot* robot) {
    return robot->carte;
}

//* Permet de libérer la mémoire du robot
void detruire_robot(Robot* robot) {
    JOURNAL_INFO("Robot (%p) detruit | autonomie restante:%d", (void*)robot, robot->autonomie);
    for (int i = 0; i < get_taille_ligne(robot->monde); i++) {
        free(robot->carte[i]);
    }
    free(robot->carte);
    free(robot->journal_local);
    free(robot);
}