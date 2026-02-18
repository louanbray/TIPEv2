#ifndef SIMULATION_H
#define SIMULATION_H

typedef struct Simulation Simulation;

//* Créé une simulation contenant un monde de rayon 'rayon' contenant une distribution de 'nombreRobots' robots ayant un certain temps_de_cycle et une certaine autonomie
Simulation* creer_simulation(int rayon, int nombreRobots, int temps_de_cycle, int autonomie_initiale);

//* Lance la simulation
int demarre_simulation(Simulation* simulation);

//* Affiche les cartes de tout les simulés
void print_simulation(Simulation* simulation);

#endif  // !SIMULATION_H