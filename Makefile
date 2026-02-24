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
#   --robots <n>        : Nombre de robots (defaut: 200)
#   --rayon <n>         : Rayon du monde (defaut: 20)
#   --autonomie <n>     : Autonomie initiale (defaut: 500)
#   --analyse           : Analyse CSV sur differents temps de cycle
#   --pas <n>           : Pas du cycle pour --analyse (defaut: 5)
#
# Exemples :
#   make run ARGS="--print"
#   make run ARGS="--seed 42 --cycle 80 --print"
#   make analyse SEED=42
ARGS ?=
SEED ?= 0

analyse: $(TARGET)
	./$(TARGET) --analyse --seed $(SEED) 2>nul

run: $(TARGET)
	./$(TARGET) $(ARGS)

clean:
	rm -rf $(BUILD)
	@echo Nettoyage effectue.

rebuild: clean all

.PHONY: all run clean rebuild
