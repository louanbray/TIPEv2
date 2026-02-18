#include "utils.h"

#include <stdlib.h>
#include <time.h>

//* Retourne vrai à la probabilité p
bool barriere_probabiliste(double probabilite) {
    return rand() < RAND_MAX * probabilite;
}

//* Renvoie un nombre entre m-d et m+d
double nombre_autour_de(double m, double d) {
    return m + ((rand() * 2 * d) / (double)RAND_MAX);
}