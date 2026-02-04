#include "robot.h"

#include <stdbool.h>

typedef enum EtatRobot {
    EXPLORATION,
    RETOUR
} EtatRobot;

typedef struct Robot {
    Case** carte;
    Decouverte* journal_local;
    EtatRobot etat;
    int nombre_decouvertes;
    int dernier_index_de_maj;
    int x;
    int y;
    int autonomie;
    int timer_de_retour;
    bool alive;
} Robot;

Robot* creer_robot(Monde* monde, int temps_de_cycle, int autonomie_initiale) {
    Robot* robot = (Robot*)malloc(sizeof(Robot));

    robot->carte = (Case**)malloc(get_taille_ligne(monde) * sizeof(Case*));
    carte_vide(monde, robot->carte);

    robot->journal_local = (Decouverte*)malloc(temps_de_cycle * sizeof(Decouverte));

    robot->etat = EXPLORATION;
    robot->nombre_decouvertes = 0;
    robot->dernier_index_de_maj = 0;

    robot->x = get_centre_x(monde);
    robot->y = get_centre_y(monde);

    robot->autonomie = autonomie_initiale;
    robot->timer_de_retour = 0;  //! à gérer correctement
    robot->alive = true;

    return robot;
}

void synchroniser_robot(Robot* robot, Monde* monde) {
    mettre_a_jour_journal(monde, robot->journal_local, robot->nombre_decouvertes);
    synchroniser_carte_base(monde, robot->carte, &robot->dernier_index_de_maj);
    robot->nombre_decouvertes = 0;
    robot->etat = EXPLORATION;
}

//! TODO : implémenter l'exploration et le retour
void actualiser_robot(Robot* robot, Monde* monde) {
    if (robot->etat == EXPLORATION) {
        //! TODO : implémenter l'exploration
    } else if (robot->etat == RETOUR) {
        //! TODO : implémenter le retour
    }
}

void detruire_robot(Robot* robot, Monde* monde) {
    for (int i = 0; i < get_taille_ligne(monde); i++) {
        free(robot->carte[i]);
    }
    free(robot->carte);
    free(robot->journal_local);
    free(robot);
}