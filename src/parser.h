#ifndef UTILS_H
#define UTILS_H

#include <ctype.h>
#include <stddef.h>

#define MAX_DESC 256
#define MAX_ITEM_NAME 50
#define MAX_CREATURE_NAME 50
#define MAX_ITEM_COUNT 10
#define MAX_ROOMS 10
#define MAX_LINE_LENGTH 256
#define MAX_ROOM_DATA 8

typedef struct
{
    char name[MAX_CREATURE_NAME];
    int health;
    int strength;
} Creature;

typedef struct
{
    int id;                     // Unique room ID
    char description[MAX_DESC]; // Room description
    int up, down, left, right;  // Connected room IDs
    char **items;               // Items present in the room
    int inventory_length;       // Number of items in the room
    Creature creature;          // Creature present in the room (if any)
} Room;

typedef struct
{
    Room **rooms;
    size_t length;
} RoomArray;

#endif

// Utility functions
void strip_whitespace(char *str);                   // Function to strip leading/trailing whitespace from a string
void parse_connection(char *token, int *direction); // Function to parse room connections
char **parse_items(char *token, int *length);       // Function to parse room items

// Parsing Functions
RoomArray parse_rooms(char *filename);                                // Function to parse rooms and return array of rooms
Room parse_room_data(char room_data[MAX_ROOM_DATA][MAX_LINE_LENGTH]); // Helper function to parse individual room data