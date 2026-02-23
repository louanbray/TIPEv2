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

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD)
	@echo Nettoyage effectue.

rebuild: clean all

.PHONY: all run clean rebuild
