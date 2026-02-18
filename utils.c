#include "utils.h"

#include <stdlib.h>
#include <time.h>

//* Retourne vrai à la probabilité p
bool barriere_probabiliste(double probabilite) {
    return rand() < RAND_MAX * probabilite;
}

//* Renvoie un nombre entre m-d et m+d
double nombre_autour_de(double m, double d) {
    return m + ((rand() / (double)RAND_MAX) * 2.0 - 1.0) * d;
}