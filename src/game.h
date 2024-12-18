#ifndef GAME_H
#define GAME_H

#include "parser.h"

typedef struct
{
    int health;
    int strength;
    int inventory_capacity;
    char **inventory;    // Player's inventory
    int inventory_count; // Number of items the player has
} Player;

typedef struct
{
    Player player;
    RoomArray rooms;
    int current_room_id;
} GameState;

// Function prototypes
GameState *init_game();
void display_player_status(GameState *state);
void pick_item(GameState *state);
void drop_item(GameState *state);
void attack_creature(GameState *state);
void move_player(GameState *state);
void look_around(GameState *state);
void show_inventory(Player *player);
void save_game_state(GameState *state, const char *save_file);
void load_game_state(GameState *state, const char *save_file);
#endif