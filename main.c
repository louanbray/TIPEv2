#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "journal.h"
#include "simulation.h"

int main() {
    srand(time(NULL));

    initialiser_journal();

    Simulation* sim = creer_simulation(20, 3, 50, 500);

    demarre_simulation(sim, false);
    print_simulation(sim);

    detruire_simulation(sim);

    fermer_journal();

    return EXIT_SUCCESS;
}