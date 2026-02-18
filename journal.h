#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <time.h>

// Fichier de journalisation - défini dans journal.c
extern FILE* g_fichier_journal;

// Macros de journalisation qui écrivent dans le fichier journal
#define JOURNAL_INFO(fmt, ...)                                                                                        \
    do {                                                                                                              \
        if (g_fichier_journal) {                                                                                      \
            time_t maintenant = time(NULL);                                                                           \
            struct tm* info_tm = localtime(&maintenant);                                                              \
            char horodatage[20];                                                                                      \
            strftime(horodatage, sizeof(horodatage), "%H:%M:%S", info_tm);                                            \
            fprintf(g_fichier_journal, "[%s][INFO][%s:%d] " fmt "\n", horodatage, __FILE__, __LINE__, ##__VA_ARGS__); \
            fflush(g_fichier_journal);                                                                                \
        }                                                                                                             \
    } while (0)
#define JOURNAL_AVERT(fmt, ...)                                                                                        \
    do {                                                                                                               \
        if (g_fichier_journal) {                                                                                       \
            time_t maintenant = time(NULL);                                                                            \
            struct tm* info_tm = localtime(&maintenant);                                                               \
            char horodatage[20];                                                                                       \
            strftime(horodatage, sizeof(horodatage), "%H:%M:%S", info_tm);                                             \
            fprintf(g_fichier_journal, "[%s][AVERT][%s:%d] " fmt "\n", horodatage, __FILE__, __LINE__, ##__VA_ARGS__); \
            fflush(g_fichier_journal);                                                                                 \
        }                                                                                                              \
    } while (0)
#define JOURNAL_ERREUR(fmt, ...)                                                                                        \
    do {                                                                                                                \
        if (g_fichier_journal) {                                                                                        \
            time_t maintenant = time(NULL);                                                                             \
            struct tm* info_tm = localtime(&maintenant);                                                                \
            char horodatage[20];                                                                                        \
            strftime(horodatage, sizeof(horodatage), "%H:%M:%S", info_tm);                                              \
            fprintf(g_fichier_journal, "[%s][ERREUR][%s:%d] " fmt "\n", horodatage, __FILE__, __LINE__, ##__VA_ARGS__); \
            fflush(g_fichier_journal);                                                                                  \
        }                                                                                                               \
    } while (0)

void initialiser_journal();
void fermer_journal();

#endif