#include "rooms.h"
#include <stdio.h>

#define ROOM_INFO_SIZE 8

Room processBatch(char **batch, int special_count, char **special_words, int *special_numbers)
{
    /*
    The batch structure is
    Room: <room_id>
    Description: <description>
    Up: <connection_room_id> (-1 represents no connection)
    Down: #
    Left: #
    Right: #
    Items: <itemName1>, <itemName2>, ...
    Creature: <name>, <health>, <strength>
    */

    Room room;
    room.id = atoi(batch[0] + 6);     // Skip "Room:"
    room.description = batch[1] + 12; // Skip "Description:"
    room.up = atoi(batch[2] + 3);     // Skip "Up:"
    room.down = atoi(batch[3] + 5);   // Skip "Down:"
    room.left = atoi(batch[4] + 5);   // Skip "Left:"
    room.right = atoi(batch[5] + 6);  // Skip "Right:"
    room.items.items = NULL;
    room.items.length = 0;

    // Process items
    int item_count;
    char **items = parseString(batch[6] + 6, ',', &item_count);


    for (int i = 0; i < item_count; i++)
    {
        Item item;
        item.name = items[i];
        item.is_special = false;
        item.is_key = false;
        item.strength = 0;

        for (int j = 0; j < special_count; j++)
        {
            if (strcmp(items[i], special_words[j]) == 0)
            {
                item.is_special = true;
                item.strength = special_numbers[j];
                break;
            }
        }

        room.items.items = realloc(room.items.items, (room.items.length + 1) * sizeof(Item));
        room.items.items[room.items.length] = item;
        room.items.length++;
    }

    // Process creature information
    // Creature:<name>, <health>, <strength>
    int creature_count;
    char **creature = parseString(batch[7] + 9, ',', &creature_count);
    room.creature.name = creature[0];
    room.creature.health = atoi(creature[1]);
    room.creature.strength = atoi(creature[2]);

    return room;
}

RoomArray processBatches(char ***batches, int roomCount, int special_count, char **special_words, int *special_numbers)
{
    RoomArray rooms;
    rooms.rooms = NULL;
    rooms.length = 0;

    // Print the first line of first batch
    for (int i = 0; i < roomCount; i++)
    {
        Room room = processBatch(batches[i], special_count, special_words, special_numbers);
        rooms.rooms = realloc(rooms.rooms, (rooms.length + 1) * sizeof(Room));
        rooms.rooms[rooms.length] = room;
        rooms.length++;
    }

    return rooms;
}

RoomArray loadRoomData(const char *room_path, const char *special_path)
{
    FILE *rooms = openFile(room_path);
    FILE *special_items = openFile(special_path);

    int batchCount;
    int roomCount;
    int special_count;
    int *special_numbers;
    char **special_words;

    char ***batches = readBatches(rooms, ROOM_INFO_SIZE, &roomCount, &batchCount);
    readWordsAndIntegers(special_items, &special_words, &special_numbers, &special_count);

    RoomArray room_data = processBatches(batches, roomCount, special_count, special_words, special_numbers);

    return room_data;
}