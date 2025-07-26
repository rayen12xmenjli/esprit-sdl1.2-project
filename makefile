
CC = gcc
CFLAGS = -Wall -g -Iinclude -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
LDFLAGS = -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -lSDL_gfx -lm
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include

SRC = $(SRC_DIR)/main.c $(SRC_DIR)/menu.c $(SRC_DIR)/player_menu.c $(SRC_DIR)/player.c \
      $(SRC_DIR)/player2.c $(SRC_DIR)/events.c $(SRC_DIR)/events2.c $(SRC_DIR)/utils.c \
      $(SRC_DIR)/inventory.c $(SRC_DIR)/ui.c $(SRC_DIR)/collision.c $(SRC_DIR)/level.c \
      $(SRC_DIR)/mouvement.c $(SRC_DIR)/jet.c $(SRC_DIR)/soldier.c $(SRC_DIR)/soldier2.c \
      $(SRC_DIR)/enemy.c $(SRC_DIR)/robot.c $(SRC_DIR)/boss.c $(SRC_DIR)/portal.c \
      $(SRC_DIR)/enigme.c $(SRC_DIR)/npc.c $(SRC_DIR)/npc2.c $(SRC_DIR)/enemylvl2.c

OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))
EXEC = game

$(shell mkdir -p $(OBJ_DIR))

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $< into $@"
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(EXEC)
	@echo "Cleaned build artifacts."

update: clean all
	@echo "Project updated and rebuilt."

.PHONY: all clean update

