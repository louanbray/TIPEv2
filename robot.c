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
    int cible_x;  // Coordonnées de la case cible courante (-1 si non définie)
    int cible_y;
    int* cible_chemin_x;  // Chemin précalculé vers la cible (positions x)
    int* cible_chemin_y;  // Chemin précalculé vers la cible (positions y)
    int cible_longueur;   // Nombre de pas dans le chemin
    int cible_etape;      // Indice du prochain pas à effectuer
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
    robot->cible_x = -1;
    robot->cible_y = -1;
    robot->cible_chemin_x = NULL;
    robot->cible_chemin_y = NULL;
    robot->cible_longueur = 0;
    robot->cible_etape = 0;

    robot->autonomie = autonomie_initiale;
    robot->timer_de_retour = temps_de_cycle;  //! TODO : à gérer correctement
    robot->temps_de_cycle = temps_de_cycle;
    robot->alive = true;

    JOURNAL_INFO("Robot (%p) cree en (%d,%d) | autonomie:%d | cycle:%d", (void*)robot, robot->x, robot->y, robot->autonomie, robot->temps_de_cycle);
    return robot;
}

//* Forward declaration (définie plus bas, après les BFS)
static void entrer_dans_phase(Robot* robot, EtatRobot etat_cible);

//* Permet à un robot de se synchroniser à la carte de la base du monde qu'il explore
void synchroniser_robot(Robot* robot) {
    JOURNAL_INFO("Robot (%p) synchronisation : %d decouverte(s) transferee(s)", (void*)robot, robot->nombre_decouvertes);

    synchroniser_carte_base(robot->monde, robot->carte, &robot->dernier_index_de_maj);
    mettre_a_jour_journal(robot->monde, robot->journal_local, robot->nombre_decouvertes);

    robot->nombre_decouvertes = 0;
    entrer_dans_phase(robot, RECHERCHE_INEXPLORE);
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
        entrer_dans_phase(robot, RETOUR);
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

//* Libère le chemin précalculé stocké dans le robot
static void liberer_chemin(Robot* robot) {
    free(robot->cible_chemin_x);
    free(robot->cible_chemin_y);
    robot->cible_chemin_x = NULL;
    robot->cible_chemin_y = NULL;
    robot->cible_longueur = 0;
    robot->cible_etape = 0;
}

//* BFS calculant le chemin complet vers (dest_x, dest_y) sur le sous-graphe exploré non-MUR
//* Stocke le chemin dans robot->cible_chemin_x/y.
//* Renvoie la longueur du chemin (0 = déjà sur place, -1 = non atteignable)
static int bfs_calculer_chemin(Robot* robot, int dest_x, int dest_y) {
    liberer_chemin(robot);
    if (robot->x == dest_x && robot->y == dest_y) return 0;

    int taille = get_taille_ligne(robot->monde);
    int** parent_x = malloc(taille * sizeof(int*));
    int** parent_y = malloc(taille * sizeof(int*));
    bool** visite = malloc(taille * sizeof(bool*));
    for (int i = 0; i < taille; i++) {
        parent_x[i] = malloc(taille * sizeof(int));
        parent_y[i] = malloc(taille * sizeof(int));
        visite[i] = calloc(taille, sizeof(bool));
        for (int j = 0; j < taille; j++) {
            parent_x[i][j] = -1;
            parent_y[i][j] = -1;
        }
    }
    int* file_x = malloc(taille * taille * sizeof(int));
    int* file_y = malloc(taille * taille * sizeof(int));
    int debut = 0, fin = 0;

    const int delta_x[4] = {-1, 0, 1, 0};
    const int delta_y[4] = {0, 1, 0, -1};

    visite[robot->y][robot->x] = true;
    file_x[fin] = robot->x;
    file_y[fin] = robot->y;
    fin++;

    bool trouve = false;
    while (debut < fin && !trouve) {
        int courant_x = file_x[debut], courant_y = file_y[debut];
        debut++;
        for (int d = 0; d < 4; d++) {
            int voisin_x = courant_x + delta_x[d];
            int voisin_y = courant_y + delta_y[d];

            if (voisin_x < 0 || voisin_x >= taille || voisin_y < 0 || voisin_y >= taille) continue;
            if (visite[voisin_y][voisin_x]) continue;

            bool est_dest = (voisin_x == dest_x && voisin_y == dest_y);
            bool traversable = robot->carte[voisin_y][voisin_x].exploree && robot->carte[voisin_y][voisin_x].type != MUR;

            if (!traversable && !est_dest) continue;

            visite[voisin_y][voisin_x] = true;
            parent_x[voisin_y][voisin_x] = courant_x;
            parent_y[voisin_y][voisin_x] = courant_y;

            file_x[fin] = voisin_x;
            file_y[fin] = voisin_y;
            fin++;
            if (est_dest) {
                trouve = true;
                break;
            }
        }
    }
    free(file_x);
    free(file_y);

    int longueur = -1;
    if (trouve) {
        // Calcul de la longueur du chemin
        int cx = dest_x, cy = dest_y;
        longueur = 0;
        while (cx != robot->x || cy != robot->y) {
            longueur++;
            int px = parent_x[cy][cx];
            int py = parent_y[cy][cx];
            cx = px;
            cy = py;
        }

        robot->cible_chemin_x = malloc(longueur * sizeof(int));
        robot->cible_chemin_y = malloc(longueur * sizeof(int));
        robot->cible_longueur = longueur;
        robot->cible_etape = 0;

        cx = dest_x;
        cy = dest_y;
        // Reconstruction du chemin
        for (int i = longueur - 1; i >= 0; i--) {
            robot->cible_chemin_x[i] = cx;
            robot->cible_chemin_y[i] = cy;
            int px = parent_x[cy][cx], py = parent_y[cy][cx];
            cx = px;
            cy = py;
        }
    }

    for (int i = 0; i < taille; i++) {
        free(parent_x[i]);
        free(parent_y[i]);
        free(visite[i]);
    }
    free(parent_x);
    free(parent_y);
    free(visite);
    return longueur;
}

//* BFS trouvant la case inexploree la plus proche et calculant le chemin complet vers elle
//* Traverse le sous-graphe exploré non-MUR. Renvoie la longueur du chemin (-1 si aucune atteignable)
static int bfs_vers_inexploree(Robot* robot) {
    liberer_chemin(robot);
    int taille = get_taille_ligne(robot->monde);
    int** parent_x = malloc(taille * sizeof(int*));
    int** parent_y = malloc(taille * sizeof(int*));
    bool** visite = malloc(taille * sizeof(bool*));
    for (int i = 0; i < taille; i++) {
        parent_x[i] = malloc(taille * sizeof(int));
        parent_y[i] = malloc(taille * sizeof(int));
        visite[i] = calloc(taille, sizeof(bool));
        for (int j = 0; j < taille; j++) {
            parent_x[i][j] = -1;
            parent_y[i][j] = -1;
        }
    }
    int* file_x = malloc(taille * taille * sizeof(int));
    int* file_y = malloc(taille * taille * sizeof(int));
    int debut = 0, fin = 0;

    const int delta_x[4] = {-1, 0, 1, 0};
    const int delta_y[4] = {0, 1, 0, -1};

    visite[robot->y][robot->x] = true;
    file_x[fin] = robot->x;
    file_y[fin] = robot->y;
    fin++;

    int inexp_x = -1, inexp_y = -1;
    bool trouve = false;
    while (debut < fin && !trouve) {
        int courant_x = file_x[debut], courant_y = file_y[debut];
        debut++;
        for (int d = 0; d < 4; d++) {
            int voisin_x = courant_x + delta_x[d], voisin_y = courant_y + delta_y[d];
            if (voisin_x < 0 || voisin_x >= taille || voisin_y < 0 || voisin_y >= taille) continue;
            if (visite[voisin_y][voisin_x]) continue;
            // Si destination atteinte, tout arrêter et préparer le chemin
            if (!robot->carte[voisin_y][voisin_x].exploree) {
                inexp_x = voisin_x;
                inexp_y = voisin_y;
                parent_x[inexp_y][inexp_x] = courant_x;
                parent_y[inexp_y][inexp_x] = courant_y;
                trouve = true;
                break;
            }
            if (robot->carte[voisin_y][voisin_x].type == MUR) continue;
            visite[voisin_y][voisin_x] = true;
            parent_x[voisin_y][voisin_x] = courant_x;
            parent_y[voisin_y][voisin_x] = courant_y;
            file_x[fin] = voisin_x;
            file_y[fin] = voisin_y;
            fin++;
        }
    }
    free(file_x);
    free(file_y);

    int longueur = -1;
    if (trouve) {
        robot->cible_x = inexp_x;
        robot->cible_y = inexp_y;
        int cx = inexp_x, cy = inexp_y;

        // Calcule la longueur du chemin
        longueur = 0;
        while (cx != robot->x || cy != robot->y) {
            longueur++;
            int px = parent_x[cy][cx];
            int py = parent_y[cy][cx];
            cx = px;
            cy = py;
        }

        robot->cible_chemin_x = malloc(longueur * sizeof(int));
        robot->cible_chemin_y = malloc(longueur * sizeof(int));
        robot->cible_longueur = longueur;
        robot->cible_etape = 0;

        cx = inexp_x;
        cy = inexp_y;
        // Reconstruis le chemin
        for (int i = longueur - 1; i >= 0; i--) {
            robot->cible_chemin_x[i] = cx;
            robot->cible_chemin_y[i] = cy;
            int px = parent_x[cy][cx];
            int py = parent_y[cy][cx];
            cx = px;
            cy = py;
        }
    }

    for (int i = 0; i < taille; i++) {
        free(parent_x[i]);
        free(parent_y[i]);
        free(visite[i]);
    }
    free(parent_x);
    free(parent_y);
    free(visite);
    return longueur;
}

//* Prépare le robot pour une nouvelle phase (RETOUR ou RECHERCHE_INEXPLORE)
//* Calcule et stocke le chemin complet une seule fois, puis met à jour l'état du robot
static void entrer_dans_phase(Robot* robot, EtatRobot etat_cible) {
    if (etat_cible == RETOUR) {
        int centre_x = get_centre_x(robot->monde);
        int centre_y = get_centre_y(robot->monde);
        int longueur = bfs_calculer_chemin(robot, centre_x, centre_y);
        if (longueur == 0) {
            robot->etat = TRANSFERT_DE_DONNEE;  // Cas étrange mais bon
            JOURNAL_INFO("Robot (%p) deja a la base, transfert immediat", (void*)robot);
        } else if (longueur > 0) {
            robot->etat = RETOUR;
            JOURNAL_INFO("Robot (%p) passe en RETOUR | chemin : %d pas", (void*)robot, longueur);
        } else {
            robot->etat = RETOUR;  // Cas anormal, ne devrait pas arriver
            JOURNAL_AVERT("Robot (%p) : aucun chemin vers la base !", (void*)robot);
        }
    } else if (etat_cible == RECHERCHE_INEXPLORE) {
        int longueur = bfs_vers_frontier(robot);
        if (longueur > 0) {
            robot->etat = RECHERCHE_INEXPLORE;
            JOURNAL_INFO("Robot (%p) cible inexplorée (%d,%d) | chemin : %d pas", (void*)robot, robot->cible_x, robot->cible_y, longueur);
        } else if (longueur == 0) {
            // Case inexplorée adjacente (cas theorique)
            robot->etat = EXPLORATION;
        } else {
            // Aucune case inexplorée accessible dans la zone explorée connue -> retour à la base pour synchronisation et espérer que d'autres robots aient exploré de nouvelles zones
            entrer_dans_phase(robot, RETOUR);
        }
    }
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
        entrer_dans_phase(robot, RECHERCHE_INEXPLORE);
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
int actualiser_robot(Robot* robot) {
    EtatRobot etat = robot->etat;
    switch (etat) {
        case EXPLORATION:
            exploration_robot(robot);
            break;
        case RETOUR: {
            if (robot->cible_etape < robot->cible_longueur) {
                robot->x = robot->cible_chemin_x[robot->cible_etape];
                robot->y = robot->cible_chemin_y[robot->cible_etape];
                robot->cible_etape++;
            }
            if (robot->cible_etape >= robot->cible_longueur) {
                robot->etat = TRANSFERT_DE_DONNEE;
                JOURNAL_INFO("Robot (%p) arrive a la base en (%d,%d)", (void*)robot, robot->x, robot->y);
            }
            break;
        }
        case RECHERCHE_INEXPLORE: {
            if (robot->cible_etape < robot->cible_longueur) {
                robot->x = robot->cible_chemin_x[robot->cible_etape];
                robot->y = robot->cible_chemin_y[robot->cible_etape];
                robot->cible_etape++;
            }
            if (robot->cible_etape + 1 >= robot->cible_longueur) {
                // Case non explorée cible adjacente -> reprise du modèle d'exploration classique pour découvrir la case et ses alentours
                robot->etat = EXPLORATION;
                JOURNAL_INFO("Robot (%p) arrive en frontier (%d,%d), reprise exploration", (void*)robot, robot->x, robot->y);
            }
            break;
        }
        case TRANSFERT_DE_DONNEE:
            synchroniser_robot(robot);
            robot->timer_de_retour = robot->temps_de_cycle;
            break;
        default:
            break;
    }

    if (robot->alive) {
        if (etat == EXPLORATION) decremente_timer_robot(robot);
        if (etat != TRANSFERT_DE_DONNEE) degrade_robot(robot);
    }
    if (!robot->alive)
        return 0;  //! TODO : Implémenter la mort du robot + les stats de données perdues etc...
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
    free(robot->cible_chemin_x);
    free(robot->cible_chemin_y);
    free(robot);
}