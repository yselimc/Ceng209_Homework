#ifndef ENGINE_H
#define ENGINE_H

#include "rooms.h"

typedef struct
{
    int health;
    int strength;
    int defense;
    int inventory_capacity;
    ItemArray inventory;
} Player;

typedef struct
{
    Player player;
    RoomArray rooms;
    int current_room_id;
} GameState;


GameState *initGame();
void loadGameState(GameState *state, const char *saveFile);
void listSavedGames();
void lookAround(GameState *state);
void pickItem(GameState *state);
void dropItem(GameState *state);
void attackCreature(GameState *state);
void useItem(GameState *state);
void movePlayer(GameState *state);
void displayStatus(GameState *state);
void showInventory(GameState *state);
void saveGameState(GameState *state, const char *saveFile);
#endif