#ifndef ROOMS_H
#define ROOMS_H

#include <ctype.h>
#include <stdbool.h>
#include "utils.h"

typedef struct
{
    const char *name;
    int health;
    int strength;
} Creature;

typedef struct
{
    const char *name;
    bool is_special;
    bool is_key;
    int strength;
} Item;

typedef struct
{
    Item *items;
    size_t length;
} ItemArray;

typedef struct
{
    int id;
    const char *description;
    int up, down, left, right;
    ItemArray items;
    Creature creature;
} Room;

typedef struct
{
    Room *rooms;
    int length;
} RoomArray;

RoomArray loadRoomData(const char *room_path, const char *special_path);
#endif