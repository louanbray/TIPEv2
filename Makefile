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

$(BUILD)/main.o:       main.c       journal.h simulation.h
$(BUILD)/journal.o:    journal.c    journal.h
$(BUILD)/monde.o:      monde.c      monde.h utils.h
$(BUILD)/robot.o:      robot.c      robot.h monde.h
$(BUILD)/simulation.o: simulation.c simulation.h robot.h monde.h journal.h
$(BUILD)/stats.o:      stats.c      stats.h
$(BUILD)/utils.o:      utils.c      utils.h

# ============================================================
#  Utilitaires
# ============================================================

# Arguments disponibles (voir main.c) :
#   --log           : Activer la journalisation
#   --xlog          : Activer la journalisation détaillée
#   --print         : Afficher les cartes à la fin de la simulation
#   --xprint        : Afficher les cartes détaillées à la fin de la simulation
#   --debug         : Activer le mode debug (log + print)
#   --xdebug        : Activer le mode debug détaillé (xlog + xprint)
#   --seed <number> : Définir la graine pour la génération aléatoire
#
# Utilisation : make run ARGS="--log --seed 42"
ARGS ?=

run: $(TARGET)
	./$(TARGET) $(ARGS)

clean:
	rm -rf $(BUILD)
	@echo Nettoyage effectue.

rebuild: clean all

.PHONY: all run clean rebuild
