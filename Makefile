# ============================================================
#  Makefile — TIPEv2
# ============================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c17 -g
LDFLAGS =

BUILD   = build
TARGET  = $(BUILD)/tipe.exe
SRC     = main.c journal.c monde.c robot.c simulation.c stats.c utils.c
OBJ     = $(addprefix $(BUILD)/, $(SRC:.c=.o))

# ============================================================
#  Règles
# ============================================================

all: $(BUILD) $(TARGET)

$(BUILD):
	mkdir $(BUILD)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^
	@echo Compilation terminee : $(TARGET)

$(BUILD)/%.o: %.c | $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $<

# ============================================================
#  Dépendances des headers
# ============================================================

$(BUILD)/main.o:       main.c       journal.h simulation.h stats.h
$(BUILD)/journal.o:    journal.c    journal.h
$(BUILD)/monde.o:      monde.c      monde.h utils.h
$(BUILD)/robot.o:      robot.c      robot.h monde.h
$(BUILD)/simulation.o: simulation.c simulation.h robot.h monde.h journal.h stats.h
$(BUILD)/stats.o:      stats.c      stats.h
$(BUILD)/utils.o:      utils.c      utils.h

# ============================================================
#  Utilitaires
# ============================================================

# Arguments disponibles (voir main.c) :
#   --log               : Activer la journalisation
#   --xlog              : Activer la journalisation detaillee
#   --print             : Afficher les cartes et stats a la fin
#   --xprint            : Afficher les cartes detaillees a la fin
#   --debug             : log + print
#   --xdebug            : xlog + xprint
#   --seed <n>          : Graine aleatoire
#   --cycle <n>         : Temps de cycle (defaut: 50)
#   --robots <n>        : Nombre de robots (defaut: 5)
#   --rayon <n>         : Rayon du monde (defaut: 20)
#   --autonomie <n>     : Autonomie initiale (defaut: 500)
#   --proba-danger <f>  : Densite de cases danger (defaut: 0.10)
#   --milieu <f>        : Letalite moyenne des cases danger (defaut: 0.20)
#   --deviation <f>     : Dispersion de la letalite (defaut: 0.10)
#   --analyse           : Analyse CSV sur differents temps de cycle
#   --pas <n>           : Pas du cycle pour --analyse et --topt (defaut: 5)
#   --repetitions <n>   : Simulations par pas (defaut: 100)
#   --topt              : Calcule T*(danger) sur plusieurs niveaux de danger
#   --analyses <n>      : Nb d'analyses independantes pour --topt (defaut: 100)
#   --danger-min <f>    : Danger minimum pour --topt (defaut: 0.0)
#   --danger-max <f>    : Danger maximum pour --topt (defaut: 0.30)
#   --danger-pas <f>    : Pas de danger pour --topt (defaut: 0.05)
#   --milieu-ratio <f>  : (topt) milieu = ratio * d pour chaque d ; ecrase --milieu
#   --deviation-ratio <f>: (topt) deviation = ratio * d pour chaque d ; ecrase --deviation
#
# Exemples :
#   make run ARGS="--print"
#   make run ARGS="--seed 42 --cycle 80 --print"
#   make analyse SEED=42
#   make analyse SEED=42 ARGS="--repetitions 500 --proba-danger 0.1 --milieu 0.2 --deviation 0.1"
#   make topt SEED=42
#   make topt SEED=42 ARGS="--analyses 100 --repetitions 50 --danger-min 0.0 --danger-max 0.30 --danger-pas 0.025"
ARGS ?=
SEED ?= 0

analyse: $(TARGET)
	./$(TARGET) --analyse --seed $(SEED) $(ARGS) 2>derniere_analyse.log

topt: $(TARGET)
	./$(TARGET) --topt --seed $(SEED) $(ARGS) > tstar.csv 2>derniere_topt.log

run: $(TARGET)
	./$(TARGET) $(ARGS)

clean:
	rm -rf $(BUILD)
	@echo Nettoyage effectue.

rebuild: clean all

.PHONY: all run clean rebuild analyse topt
