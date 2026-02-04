#ifndef ROBOT_H
#define ROBOT_H

#include "monde.h"

typedef struct Robot Robot;

Robot* creer_robot(Monde* monde, int temps_de_cycle, int autonomie_initiale);

#endif  // !ROBOT_H