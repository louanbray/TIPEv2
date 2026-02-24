#ifndef ROBOT_H
#define ROBOT_H

#include "monde.h"

//? false : le timer décrémente à chaque nouvelle exploration de cases (Phase exploration)
//? true : le timer décrémente à chaque déplacement dans la phase EXPLORATION et RECHERCHE_INEXPLORE (pour tester le retour à la base plus rapidement)
//? true -> tout les robots vont revenir à la base en même temps, ce qui n'est pas très interessant.
#define TIMER_ABSOLU false

typedef struct Robot Robot;

typedef enum EtatRobot {
    EN_VIE,
    EN_ATTENTE,
    HORS_SERVICE
} EtatRobot;

//* Permet de faire apparaitre un robot sur un monde
Robot* creer_robot(Monde* monde, int temps_de_cycle, int autonomie_initiale);
//* Permet de libérer la mémoire du robot
void detruire_robot(Robot* robot);

//* Permet à un robot de se synchroniser à la carte de la base du monde qu'il explore
void synchroniser_robot(Robot* robot);

//* Routine du robot (renvoie 1 si le robot est en vie, 0 sinon)
EtatRobot actualiser_robot(Robot* robot);

//* Accesseurs
int get_robot_x(Robot* robot);
int get_robot_y(Robot* robot);

//* Permet d'accéder à la carte interne du robot
//? Utilisé pour débug
Case** get_carte_robot(Robot* robot);

#endif  // !ROBOT_H