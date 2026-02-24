#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdbool.h>

typedef struct Simulation Simulation;

//* Créé une simulation contenant un monde de rayon 'rayon' contenant une distribution de 'nombreRobots' robots ayant un certain temps_de_cycle et une certaine autonomie
Simulation* creer_simulation(int rayon, int nombreRobots, int temps_de_cycle, int autonomie_initiale);

//* Lance la simulation (labyrinthe=true : labyrinthe DFS, false : murs aléatoires)
int demarre_simulation(Simulation* simulation, bool labyrinthe);

//* Libère la mémoire de la simulation et de tous ses composants
void detruire_simulation(Simulation* simulation);

//* Affiche les cartes de tout les simulés
void print_simulation(Simulation* simulation, bool details);

#endif  // !SIMULATION_H