#ifndef UTILS_H
#define UTILS_H
#include <stdbool.h>

//* Retourne vrai à la probabilité p
bool barriere_probabiliste(double p);

//* Renvoie un nombre entre m-d et m+d
double nombre_autour_de(double m, double d);

#endif  // !UTILS_H