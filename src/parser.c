#include "parser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Utility functions

void strip_whitespace(char *str)
{
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str))
    {
        str++;
    }

    // Trim trailing space
    if (*str == 0)
    {
        return; // All spaces
    }

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
    {
        end--;
    }
    // Write the new null terminator
    *(end + 1) = '\0';
}

void parse_connection(char *token, int *direction)
{
    strip_whitespace(token);
    *direction = atoi(strchr(token, ':') + 1);
}

char **parse_items(char *token, int *length)
{
    strip_whitespace(token);

    // items start with the word "Items:item1, item2, item3"
    char *items_data = strchr(token, ':');
    if (items_data == NULL)
    {
        return NULL;
    }
    items_data++;
    strip_whitespace(items_data);

    char *temp_data = strdup(items_data); // Make a temporary copy to tokenize
    char *item = strtok(temp_data, ", ");
    int item_count = 0;
    // First pass to count the number of items
    while (item != NULL)
    {
        item_count++;
        item = strtok(NULL, ", ");
    }
    free(temp_data); // Free temporary copy

    // Allocate memory for items array
    // Allocate memory for items array
    char **items = malloc(item_count * sizeof(char *));
    if (items == NULL)
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    // Second pass to copy the items
    temp_data = strdup(items_data); // Make a temporary copy to tokenize
    item = strtok(temp_data, ", ");
    item_count = 0;
    while (item != NULL)
    {
        items[item_count] = strdup(item);
        item_count++;
        item = strtok(NULL, ", ");
    }
    *length = item_count;
    return items;
}

Creature parse_creature(char *token)
{
    Creature creature;
    creature.health = -1;
    creature.strength = -1;

    strip_whitespace(token);
    char *creature_data = strchr(token, ':') + 1;
    char *creature_name = strtok(creature_data, ",");
    char *creature_health = strtok(NULL, ",");
    char *creature_strength = strtok(NULL, ",");
    strip_whitespace(creature_name);
    strcpy(creature.name, creature_name);
    creature.health = atoi(creature_health);
    creature.strength = atoi(creature_strength);

    return creature;
}

Room parse_room_data(char room_data[MAX_ROOM_DATA][MAX_LINE_LENGTH])
{
    /***
     Room Structure:
     * Room:<room_id>
     * Description:<your_room_description>
     * Up:<connected_room_id>
     * Down:<connected_room_id>
     * Left:<connected_room_id>
     * Right:<connected_room_id>
     * Items:<item_name1>, <item_name2>, ...
     * Creature:<creature_name>,<creature_health>,<creature_strength>
     */

    // Few helpers:
    // atoi() in c converts a string to an integer
    // strcpy() in c copies a string to another string

    Room room;
    char *token = strtok(room_data[0], "\n");

    // Parse room ID
    room.id = atoi(token + strlen("Room:"));

    // Parse room description the new line
    token = strtok(room_data[1], "\n");
    strip_whitespace(token);
    strcpy(room.description, token + strlen("Description:"));

    // Parse room connections
    token = strtok(room_data[2], "\n");
    parse_connection(token, &room.up);

    token = strtok(room_data[3], "\n");
    parse_connection(token, &room.down);

    token = strtok(room_data[4], "\n");
    parse_connection(token, &room.left);

    token = strtok(room_data[5], "\n");
    parse_connection(token, &room.right);

    // Parse items
    token = strtok(room_data[6], "\n");
    room.items = parse_items(token, &room.inventory_length);

    // Parse creature
    token = strtok(room_data[7], "\n");
    Creature creature = parse_creature(token);
    room.creature = creature;

    return room;
}

RoomArray parse_rooms(char *filename)
{
    // Open the file
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Failed to open file");
        return (RoomArray){0};
    }
    else
    {
        printf("File opened successfully\n");
    }

    // Dynamically allocate memory for rooms
    Room **rooms = malloc(sizeof(Room *) * MAX_ROOMS);
    char lines[MAX_ROOM_DATA][MAX_LINE_LENGTH];
    char line[MAX_LINE_LENGTH];
    int room_count = 0;
    int line_count = 0;

    while (fgets(line, sizeof(line), file))
    {
        // Add to lines until we reach a new line
        for (int i = 0; i < MAX_ROOM_DATA; i++)
        {
            strcpy(lines[line_count], line);
            line_count++;
            fgets(line, sizeof(line), file);
        }

        rooms[room_count] = malloc(sizeof(Room));
        Room room = parse_room_data(lines);
        *rooms[room_count] = room;
        room_count++;

        line_count = 0;
    }
    fclose(file);
    RoomArray room_array;
    room_array.rooms = rooms;
    room_array.length = room_count;

    return room_array;
}
