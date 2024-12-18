#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GameState *init_game()
{
    // Initialize the game state
    GameState *state = malloc(sizeof(GameState));
    // Initialize the player
    state->player.health = 100;
    state->player.strength = 5;
    state->player.inventory_capacity = 5;

    // Initialize the player's inventory
    state->player.inventory = malloc(state->player.inventory_capacity * sizeof(char *));
    state->player.inventory_count = 0;

    // Initialize the room array
    state->rooms = parse_rooms("assets/rooms.txt");

    // Set the current room ID
    state->current_room_id = 1;

    return state;
}

// Helper functions for game actions (not exposed in the header file)

void __pick_item(Player *player, Room *room, const char *item_name)
{
    player->inventory[player->inventory_count] = strdup(item_name);
    player->inventory_count++;

    // Remove the item from the room
    for (int i = 0; i < room->inventory_length; i++)
    {
        if (strcmp(room->items[i], item_name) == 0)
        {
            free(room->items[i]);
            for (int j = i; j < room->inventory_length - 1; j++)
            {
                room->items[j] = room->items[j + 1];
            }
            room->inventory_length--;
            break;
        }
    }

}

void __drop_item(Player *player, Room *room, const char *item_name)
{
    // Check if the item is in the player's inventory
    int item_index = -1;
    for (int i = 0; i < player->inventory_count; i++)
    {
        if (strcmp(player->inventory[i], item_name) == 0)
        {
            item_index = i;
            break;
        }
    }

    if (item_index == -1)
    {
        printf("You don't have %s in your inventory.\n", item_name);
        return;
    }

    // Add the item to the room
    item_index = -1;
    for (int i = 0; i < MAX_ITEM_COUNT; i++)
    {
        if (room->items[i] == NULL)
        {
            room->items[i] = strdup(item_name);
            break;
        }
    }

    if (item_index == MAX_ITEM_COUNT)
    {
        printf("The room is full of items. You can't drop %s here.\n", item_name);
        return;
    }

    // Remove the item from the player's inventory
    for (int i = 0; i < player->inventory_count; i++)
    {
        if (strcmp(player->inventory[i], item_name) == 0)
        {
            free(player->inventory[i]);
            for (int j = i; j < player->inventory_count - 1; j++)
            {
                player->inventory[j] = player->inventory[j + 1];
            }
            player->inventory_count--;
            break;
        }
    }
}

void __attack_creature(Player *player, Creature *creature)
{
    // Check if the creature is present
    if (creature->health <= 0)
    {
        printf("There's no creature here to attack.\n");
        return;
    }

    // Attack the creature
    printf("You attacked the creature!\n");
    creature->health -= player->strength;
    // Check if the creature is defeated
    if (creature->health <= 0)
    {
        printf("You defeated the creature!\n");
    }
    else
    {
        // The creature attacks back
        printf("The creature attacks back!\n");
        player->health -= creature->strength;
        if (player->health <= 0)
        {
            printf("You have been defeated. Game Over!\n");
            exit(0);
        }
    }
}

void pick_item(GameState *state)
{
    // Get the current room
    Room *current_room = state->rooms.rooms[state->current_room_id-1];

    // Check if the room has any items
    if (current_room->items == NULL)
    {
        printf("No items in the room.\n");
        return;
    }

    // Print the items in the room
    printf("Items in the room:\n");
    for (int i = 0; i < current_room->inventory_length; i++)
    {
        if (current_room->items[i] != NULL)
        {
            printf("%s\n", current_room->items[i]);
        }
    }

    // Get the item name from the user
    char item_name[MAX_ITEM_NAME];
    printf("Enter the item name to pick: ");
    scanf("%s", item_name);

    // Pick the item
    __pick_item(&state->player, current_room, item_name);
}

void drop_item(GameState *state)
{
    // Get the current room
    Room *current_room = state->rooms.rooms[state->current_room_id-1];

    // Check if the player has any items
    if (state->player.inventory_count == 0)
    {
        printf("Your inventory is empty.\n");
        return;
    }

    // Print the items in the player's inventory
    printf("Items in your inventory:\n");
    for (int i = 0; i < state->player.inventory_count; i++)
    {
        printf("%s\n", state->player.inventory[i]);
    }

    // Get the item name from the user
    char item_name[MAX_ITEM_NAME];
    printf("Enter the item name to drop: ");
    scanf("%s", item_name);

    // Drop the item
    __drop_item(&state->player, current_room, item_name);
}

void attack_creature(GameState *state)
{
    // Get the current room
    Room *current_room = state->rooms.rooms[state->current_room_id-1];

    // Check if the room has a creature
    if (current_room->creature.health <= 0)
    {
        printf("There's no creature here to attack.\n");
        return;
    }

    // Attack the creature
    __attack_creature(&state->player, &current_room->creature);
}

void move_player(GameState *state)
{
    // Get the current room
    Room *current_room = state->rooms.rooms[state->current_room_id-1];

    // Ask the user for the direction
    char direction[MAX_DESC];

    printf("Enter the direction to move (up, down, left, right): ");
    scanf("%s", direction);

    while (strcmp(direction, "up") != 0 && strcmp(direction, "down") != 0 &&
           strcmp(direction, "left") != 0 && strcmp(direction, "right") != 0)
    {
        printf("Invalid direction. Please enter a valid direction (up, down, left, right): ");
        scanf("%s", direction);
    }

    // If the given direction leads to a room with connection 0, then give warning,
    // else move the player to the new room

    int new_room_id = -1;

    if (strcmp(direction, "up") == 0)
        new_room_id = current_room->up;
    else if (strcmp(direction, "down") == 0)
        new_room_id = current_room->down;
    else if (strcmp(direction, "left") == 0)
        new_room_id = current_room->left;
    else if (strcmp(direction, "right") == 0)
        new_room_id = current_room->right;

    if (new_room_id == 0)
    {
        printf("There's no room connected in this direction.\n");
    }
    else
    {
        state->current_room_id = new_room_id;
        printf("Player moved to room %d.\n", new_room_id);
    }
}

void look_around(GameState *state)
{
    // Display the description of the current room and items or creatures in it
    Room *current_room = state->rooms.rooms[state->current_room_id-1];

    printf("Room ID: %d\n", current_room->id);
    printf("Description: %s\n", current_room->description);

    printf("Connections:\n");
    printf("  Up: %d, Down: %d, Left: %d, Right: %d\n", current_room->up, current_room->down, current_room->left, current_room->right);

    printf("Items in the room:\n");

    // Check if items array is NULL
    if (current_room->items == NULL)
    {
        printf("  No items in the room.\n");
    }
    else
    {
        for (int i = 0; i < current_room->inventory_length; i++)
        {
            if (current_room->items[i] != NULL)
            {
                printf("  - %s\n", current_room->items[i]);
            }
        }
    }

    if (current_room->creature.health > 0)
    {
        printf("A creature is here with %d health and %d strength.\n", current_room->creature.health, current_room->creature.strength);
    }
    else
    {
        printf("No creature in this room.\n");
    }
}

void show_inventory(Player *player)
{
    // Display the items in the player's inventory
    if (player->inventory_count == 0)
    {
        printf("Player inventory is empty.\n");
        return;
    }
    else
    {
        printf("Player inventory:\n");
        for (int i = 0; i < player->inventory_count; i++)
        {
            printf("  - %s\n", player->inventory[i]);
        }
    }
}

void save_game_state(GameState *state, const char *save_file)
{
    // Save the game state to a file
    FILE *file = fopen(save_file, "wb");
    if (file == NULL)
    {
        printf("Failed to save the game state.\n");
        return;
    }

    // Write the player data
    fwrite(&state->player, sizeof(Player), 1, file);

    // Write the room array data
    fwrite(&state->rooms.length, sizeof(size_t), 1, file);
    for (size_t i = 0; i < state->rooms.length; i++)
    {
        Room *room = state->rooms.rooms[i];
        fwrite(room, sizeof(Room), 1, file);
    }

    // Write the current room ID
    fwrite(&state->current_room_id, sizeof(int), 1, file);

    fclose(file);
    printf("Game state saved to %s.\n", save_file);
}

void load_game_state(GameState *state, const char *save_file)
{
    // Load the game state from a file
    FILE *file = fopen(save_file, "rb");
    if (file == NULL)
    {
        printf("Failed to load the game state.\n");
        return;
    }

    // Read the player data
    fread(&state->player, sizeof(Player), 1, file);

    // Read the room array data
    size_t room_count;
    fread(&room_count, sizeof(size_t), 1, file);
    state->rooms.length = room_count;
    state->rooms.rooms = malloc(room_count * sizeof(Room *));
    for (size_t i = 0; i < room_count; i++)
    {
        Room *room = malloc(sizeof(Room));
        fread(room, sizeof(Room), 1, file);
        state->rooms.rooms[i] = room;
    }

    // Read the current room ID
    fread(&state->current_room_id, sizeof(int), 1, file);

    fclose(file);
    printf("Game state loaded from %s.\n", save_file);
}

void display_player_status(GameState *state)
{
    printf("Player Health: %d\n", state->player.health);
    printf("Player Strength: %d\n", state->player.strength);

    printf("Current Room ID: %d\n", state->current_room_id);
}