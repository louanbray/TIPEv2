#include "robot.h"

#include <stdbool.h>
#include <stdlib.h>

#include "journal.h"
#include "utils.h"

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

    robot->journal_local = (Decouverte*)malloc(5 * temps_de_cycle * sizeof(Decouverte));

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

    synchroniser_carte_base(robot->monde, robot->carte, &robot->dernier_index_de_maj);
    mettre_a_jour_journal(robot->monde, robot->journal_local, robot->nombre_decouvertes);

    robot->nombre_decouvertes = 0;
    robot->etat = RECHERCHE_INEXPLORE;
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

//* Enregistre une nouvelle découverte dans la carte locale et le journal du robot
void enregistre_decouverte_robot(Robot* robot, int x, int y, CaseType type) {
    if (robot->carte[y][x].exploree) return;
    Decouverte nouvelle_decouverte = {.x = x, .y = y, .type = type};
    robot->journal_local[robot->nombre_decouvertes++] = nouvelle_decouverte;

    robot->carte[y][x].type = type;
    robot->carte[y][x].exploree = true;
}

//* Regarde autour de la case et enregistre les murs
void robot_regarde_case(Robot* robot, int x, int y, CaseType type) {
    if (type == MUR) enregistre_decouverte_robot(robot, x, y, MUR);
}

//* Tue le robot avec la bonne probabilité (case danger)
void marche_sur_case_danger(Robot* robot) {
    Case** grille_monde = get_grille_monde(robot->monde);
    if (barriere_probabiliste(grille_monde[robot->y][robot->x].proba_danger)) {
        robot->alive = false;
        JOURNAL_AVERT("Robot (%p) tue par un danger en (%d,%d)", (void*)robot, robot->x, robot->y);
    }

    //? Sécurisation de la case après survie du robot
    grille_monde[robot->y][robot->x].type = VIDE;
}

//* Regarde autour du robot (OUEST->NORD->EST->SUD) enregistre les murs puis se déplace vers une case non mur
void exploration_robot(Robot* robot) {
    int dx[4] = {-1, 0, 1, 0};
    int dy[4] = {0, 1, 0, -1};

    Case** grille_monde = get_grille_monde(robot->monde);

    int possibilite[4] = {-1, -1, -1, -1};
    int possibilites = 0;

    for (int dir = 0; dir < 4; dir++) {
        int n_x = robot->x + dx[dir];
        int n_y = robot->y + dy[dir];

        // Ignorer les cases hors de la grille
        if (n_x < 0 || n_x >= get_taille_ligne(robot->monde)) continue;
        if (n_y < 0 || n_y >= get_taille_ligne(robot->monde)) continue;

        robot_regarde_case(robot, n_x, n_y, grille_monde[n_y][n_x].type);

        if (!robot->carte[n_y][n_x].exploree) possibilite[possibilites++] = dir;
    }

    if (possibilites == 0) {
        //! TODO : RECHERCHE INEXPLORE
        return;
    }

    int p = rand() % possibilites;

    robot->x += dx[possibilite[p]];
    robot->y += dy[possibilite[p]];

    //? Si le robot s'est déplacé c'est que la case est soit DANGER soit VIDE, si elle est DANGER alors après avoir survécu et la sécurise en VIDE, dans le cas contraire, le danger est désamorcé pour les autres robots.
    enregistre_decouverte_robot(robot, robot->x, robot->y, VIDE);
    if (grille_monde[robot->y][robot->x].type == DANGER) marche_sur_case_danger(robot);
}

//* Routine du robot (renvoie 1 si le robot est en vie, 0 sinon)
//! TODO : implémenter l'exploration et le retour
int actualiser_robot(Robot* robot) {
    EtatRobot etat = robot->etat;
    switch (etat) {
        case EXPLORATION:
            exploration_robot(robot);
            break;
        case RETOUR:
            //! TODO : implémenter le retour vers la base
            break;
        case RECHERCHE_INEXPLORE:
            //! TODO : implémenter la recherche d'une zone inexplorée pour reprendre l'exploration
            break;
        case TRANSFERT_DE_DONNEE:
            synchroniser_robot(robot);
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