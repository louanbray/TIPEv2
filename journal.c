#include "journal.h"

#include <string.h>

FILE* g_fichier_journal = NULL;

#define CHEMIN_FICHIER_JOURNAL "simulations.log"

void initialiser_journal() {
    g_fichier_journal = fopen(CHEMIN_FICHIER_JOURNAL, "a");

    if (g_fichier_journal == NULL) {
        g_fichier_journal = stdout;
        fprintf(stderr, "[ATTENTION] Impossible d'ouvrir le fichier journal, basculement vers stdout\n");
        return;
    }

    time_t maintenant = time(NULL);
    struct tm* t = localtime(&maintenant);
    char horodatage[64];
    strftime(horodatage, sizeof(horodatage), "%d-%m-%Y %H:%M:%S", t);

    fprintf(g_fichier_journal, "\n");
    fprintf(g_fichier_journal, "================================================================================\n");
    fprintf(g_fichier_journal, "Session de TIPE démarrée le %s\n", horodatage);
    fprintf(g_fichier_journal, "================================================================================\n");
    fflush(g_fichier_journal);
}

void fermer_journal() {
    if (g_fichier_journal != NULL && g_fichier_journal != stdout && g_fichier_journal != stderr) {
        time_t maintenant = time(NULL);
        struct tm* t = localtime(&maintenant);
        char horodatage[64];
        strftime(horodatage, sizeof(horodatage), "%d-%m-%Y %H:%M:%S", t);

        fprintf(g_fichier_journal, "================================================================================\n");
        fprintf(g_fichier_journal, "Session de TIPE terminée le %s\n", horodatage);
        fprintf(g_fichier_journal, "================================================================================\n\n");

        fclose(g_fichier_journal);
        g_fichier_journal = NULL;
    }
}
