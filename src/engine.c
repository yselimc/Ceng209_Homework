#include "engine.h"
#include <dirent.h>
#include <stdlib.h>

GameState *initGame()
{
    // Initialize the game state
    GameState *state = malloc(sizeof(GameState));

    // Initialize the player
    state->player.health = 100;
    state->player.strength = 10;
    state->player.defense = 5;
    state->player.inventory_capacity = 5;
    // Initialize the player's inventory
    state->player.inventory.items = malloc(state->player.inventory_capacity * sizeof(Item));
    state->player.inventory.length = 0;

    // Initialize the room array
    state->rooms = loadRoomData("assets/rooms.txt", "assets/special.txt");

    // Set the current room ID
    state->current_room_id = 0;

    // Welcome message
    printf("Welcome to the game!\n");

    // Ask user to start a new game or load a saved game or list saved games
    printf("Do you want to start a new game or load a saved game?\n");
    // Get the user's choice
    char choice;
    do
    {

        printf("Enter 'N' for a new game or 'L' to load a saved game or 'S' to list saved games: ");
        scanf(" %c", &choice);
        if (choice == 'N' || choice == 'n')
        {
            // Start a new game
            printf("Starting a new game...\n");
            break;
        }
        else if (choice == 'L' || choice == 'l')
        {
            // Load a saved game
            printf("Loading a saved game...\n");
            char saveFile[256];
            printf("Enter the name of the save file: ");
            scanf("%s", saveFile);
            loadGameState(state, saveFile);
            break;
        }
        else if (choice == 'S' || choice == 's')
        {
            // List saved games
            listSavedGames();
        }
        else
        {
            printf("Invalid choice. Please enter 'N' for a new game or 'L' to load a saved game or 'S' to list saved games: ");
            scanf(" %c", &choice);
        }
    } while (choice != 'N' && choice != 'n' && choice != 'L' && choice != 'l');

    return state;
}

void list_all_files()
{
    DIR *dir;
    struct dirent *entry;
    const char *directory = "saves/";
    // Open the directory
    dir = opendir(directory);
    if (dir == NULL)
    {
        perror("opendir");
        return;
    }

    // Read and print each entry in the directory
    while ((entry = readdir(dir)) != NULL)
    {
        printf("%s\n", entry->d_name);
    }

    // Close the directory
    closedir(dir);
}

void listSavedGames()
{
    // List the saved games
    printf("Our default save directory is 'saves/'.\n");
    printf("List of saved games:\n");

    // If the 'saves' directory does not exist, create it
    const char *directory = "saves/";
    DIR *dir = opendir(directory);
    if (dir == NULL)
    {
        system("mkdir saves");
    }
    else
    {
        closedir(dir);
    }

    // List the files in the 'saves' directory
    list_all_files();
}

void saveGameState(GameState *state, const char *saveFile)
{
    // If saveFile is NULL, save it under saves/save.txt
    // but first check if the saves directory exists
    // if not, create it
    struct dirent *entry;

    // Open the directory
    const char *directory = "saves/";
    DIR *dir = opendir(directory);
    if (dir == NULL)
    {
        perror("opendir");
        return;
    }

    if (saveFile == NULL)
    {
        saveFile = "saves/save.txt";
    }

    else
    {
        // Add the saves/ prefix to the save file
        char savePath[256];
        sprintf(savePath, "saves/%s", saveFile);
        saveFile = savePath;
    }
    // Save the game state to a file
    FILE *file = fopen(saveFile, "wb");
    if (file == NULL)
    {
        printf("Failed to save the game state.\n");
        return;
    }

    // Save the player's static fields
    fwrite(&state->player.health, sizeof(int), 1, file);
    fwrite(&state->player.strength, sizeof(int), 1, file);
    fwrite(&state->player.inventory_capacity, sizeof(int), 1, file);

    // Save the player's inventory (ItemArray)
    fwrite(&state->player.inventory.length, sizeof(size_t), 1, file);
    for (size_t i = 0; i < state->player.inventory.length; i++)
    {
        Item *item = &state->player.inventory.items[i];
        size_t name_length = strlen(item->name) + 1;
        fwrite(&name_length, sizeof(size_t), 1, file);
        fwrite(item->name, sizeof(char), name_length, file);
        fwrite(&item->is_special, sizeof(bool), 1, file);
        fwrite(&item->is_key, sizeof(bool), 1, file);
        fwrite(&item->strength, sizeof(int), 1, file);
    }

    // Save the rooms (RoomArray)
    fwrite(&state->rooms.length, sizeof(int), 1, file);
    for (int i = 0; i < state->rooms.length; i++)
    {
        Room *room = &state->rooms.rooms[i];
        fwrite(&room->id, sizeof(int), 1, file);

        size_t description_length = strlen(room->description) + 1;
        fwrite(&description_length, sizeof(size_t), 1, file);
        fwrite(room->description, sizeof(char), description_length, file);

        fwrite(&room->up, sizeof(int), 1, file);
        fwrite(&room->down, sizeof(int), 1, file);
        fwrite(&room->left, sizeof(int), 1, file);
        fwrite(&room->right, sizeof(int), 1, file);

        // Save Room Items
        fwrite(&room->items.length, sizeof(size_t), 1, file);
        for (size_t j = 0; j < room->items.length; j++)
        {
            Item *item = &room->items.items[j];
            size_t name_length = strlen(item->name) + 1;
            fwrite(&name_length, sizeof(size_t), 1, file);
            fwrite(item->name, sizeof(char), name_length, file);
            fwrite(&item->is_special, sizeof(bool), 1, file);
            fwrite(&item->is_key, sizeof(bool), 1, file);
            fwrite(&item->strength, sizeof(int), 1, file);
        }
        // Save Creature
        size_t creature_name_length = strlen(room->creature.name) + 1;
        fwrite(&creature_name_length, sizeof(size_t), 1, file);
        fwrite(room->creature.name, sizeof(char), creature_name_length, file);
        fwrite(&room->creature.health, sizeof(int), 1, file);
        fwrite(&room->creature.strength, sizeof(int), 1, file);
    }

    // Save current room ID
    fwrite(&state->current_room_id, sizeof(int), 1, file);

    printf("Game saved successfully.\n");
    fclose(file);
}

void loadGameState(GameState *state, const char *saveFile)
{
    char savePath[256];
    sprintf(savePath, "saves/%s", saveFile);
    saveFile = savePath;
    // Load the game state from a file
    FILE *file = fopen(saveFile, "rb");
    if (file == NULL)
    {
        printf("Failed to load the game state.\n");
        exit(EXIT_FAILURE);
    }

    fread(&state->player.health, sizeof(int), 1, file);
    fread(&state->player.strength, sizeof(int), 1, file);
    fread(&state->player.inventory_capacity, sizeof(int), 1, file);
    // Load the player's inventory (ItemArray)
    fread(&state->player.inventory.length, sizeof(size_t), 1, file);
    state->player.inventory.items = malloc(state->player.inventory.length * sizeof(Item));
    for (size_t i = 0; i < state->player.inventory.length; i++)
    {
        Item *item = &state->player.inventory.items[i];
        size_t name_length;
        fread(&name_length, sizeof(size_t), 1, file);
        item->name = malloc(name_length);
        fread((char *)item->name, sizeof(char), name_length, file);
        fread(&item->is_special, sizeof(bool), 1, file);
        fread(&item->is_key, sizeof(bool), 1, file);
        fread(&item->strength, sizeof(int), 1, file);
    }

    // Load the rooms (RoomArray)
    fread(&state->rooms.length, sizeof(int), 1, file);
    state->rooms.rooms = malloc(state->rooms.length * sizeof(Room));
    for (int i = 0; i < state->rooms.length; i++)
    {
        Room *room = &state->rooms.rooms[i];
        fread(&room->id, sizeof(int), 1, file);

        size_t description_length;
        fread(&description_length, sizeof(size_t), 1, file);
        room->description = malloc(description_length);
        fread((char *)room->description, sizeof(char), description_length, file);

        fread(&room->up, sizeof(int), 1, file);
        fread(&room->down, sizeof(int), 1, file);
        fread(&room->left, sizeof(int), 1, file);
        fread(&room->right, sizeof(int), 1, file);

        // Load Room Items
        fread(&room->items.length, sizeof(size_t), 1, file);
        room->items.items = malloc(room->items.length * sizeof(Item));
        for (size_t j = 0; j < room->items.length; j++)
        {
            Item *item = &room->items.items[j];
            size_t name_length;
            fread(&name_length, sizeof(size_t), 1, file);
            item->name = malloc(name_length);
            fread((char *)item->name, sizeof(char), name_length, file);
            fread(&item->is_special, sizeof(bool), 1, file);
            fread(&item->is_key, sizeof(bool), 1, file);
            fread(&item->strength, sizeof(int), 1, file);
        }

        // Load Creature
        size_t creature_name_length;
        fread(&creature_name_length, sizeof(size_t), 1, file);
        room->creature.name = malloc(creature_name_length);
        fread((char *)room->creature.name, sizeof(char), creature_name_length, file);
        fread(&room->creature.health, sizeof(int), 1, file);
        fread(&room->creature.strength, sizeof(int), 1, file);
    }

    // Load current room ID
    fread(&state->current_room_id, sizeof(int), 1, file);

    displayStatus(state);
    lookAround(state);
    fclose(file);
}

void lookAround(GameState *state)
{
    // Get the current room
    Room *currentRoom = &state->rooms.rooms[state->current_room_id];

    // Print the room id
    printf("Room ID: %d\n", currentRoom->id);

    // Print the room description
    printf("%s\n", currentRoom->description);

    // Print the items in the room
    if (currentRoom->items.length > 0)
    {
        printf("Items in the room:\n");
        for (int i = 0; i < currentRoom->items.length; i++)
        {
            Item item = currentRoom->items.items[i];
            printf("%s\n", item.name);
        }
    }

    // Print the creature in the room
    if (currentRoom->creature.name != NULL)
    {
        printf("Creature in the room: %s\n", currentRoom->creature.name);
        printf("Creature health: %d\n", currentRoom->creature.health);
        printf("Creature strength: %d\n", currentRoom->creature.strength);
    }

    // Print connections of the room
    printf("Connections:\n");
    if (currentRoom->up != -1)
    {
        printf("Up: %d\n", currentRoom->up);
    }
    if (currentRoom->down != -1)
    {
        printf("Down: %d\n", currentRoom->down);
    }
    if (currentRoom->left != -1)
    {
        printf("Left: %d\n", currentRoom->left);
    }
    if (currentRoom->right != -1)
    {
        printf("Right: %d\n", currentRoom->right);
    }
}

void addItem(ItemArray *items, Item item)
{
    items->items = realloc(items->items, (items->length + 1) * sizeof(Item));
    items->items[items->length] = item;
    items->length++;
}

void removeItem(ItemArray *items, Item item)
{
    int index = -1;
    for (int i = 0; i < items->length; i++)
    {
        if (strcmp(items->items[i].name, item.name) == 0)
        {
            index = i;
            break;
        }
    }

    if (index != -1)
    {
        for (int i = index; i < items->length - 1; i++)
        {
            items->items[i] = items->items[i + 1];
        }
        items->length--;
        items->items = realloc(items->items, items->length * sizeof(Item));
    }

    else
    {
        printf("Item not found.\n");
    }
}

void pickItem(GameState *state)
{
    // List the items in the current room
    Room *currentRoom = &state->rooms.rooms[state->current_room_id];
    if (currentRoom->items.length == 0)
    {
        printf("There are no items in this room.\n");
        return;
    }

    // Check if the player's inventory is full
    if (state->player.inventory.length == state->player.inventory_capacity)
    {
        printf("Your inventory is full. You cannot pick any more items.\n");
        return;
    }
    else
    {
        printf("Your inventory capacity is %d.\n", state->player.inventory_capacity);
    }

    if (currentRoom->items.length > 0)
    {
        printf("%d\n", currentRoom->items.length);
        printf("Items in the room:\n");
        for (int i = 0; i < currentRoom->items.length; i++)
        {
            Item item = currentRoom->items.items[i];
            printf("%d. %s\n", i + 1, item.name);
        }
    }

    // Ask the player to pick an item
    int choice;
    printf("Enter the number of the item you want to pick: ");
    scanf("%d", &choice);

    // Check if the choice is valid
    if (choice < 1 || choice > currentRoom->items.length)
    {
        printf("Invalid choice.\n");
        return;
    }

    // Add the item to the player's inventory
    Item item = currentRoom->items.items[choice - 1];
    printf("You picked up %s.\n", item.name);
    addItem(&state->player.inventory, item);

    // Remove the item from the room
    removeItem(&currentRoom->items, item);
}

void dropItem(GameState *state)
{
    Room *currentRoom = &state->rooms.rooms[state->current_room_id];
    if (state->player.inventory.length == 0)
    {
        printf("Your inventory is empty.\n");
        return;
    }

    printf("Items in your inventory:\n");
    for (int i = 0; i < state->player.inventory.length; i++)
    {
        Item item = state->player.inventory.items[i];
        printf("%d. %s\n", i + 1, item.name);
    }

    int choice;
    printf("Enter the number of the item you want to drop: ");
    scanf("%d", &choice);

    if (choice < 1 || choice > state->player.inventory.length)
    {
        printf("Invalid choice.\n");
        return;
    }

    Item item = state->player.inventory.items[choice - 1];
    addItem(&currentRoom->items, item);

    removeItem(&state->player.inventory, item);
}

void attackCreature(GameState *state)
{
    Room *currentRoom = &state->rooms.rooms[state->current_room_id];
    if (currentRoom->creature.name == NULL)
    {
        printf("There is no creature in this room.\n");
        return;
    }

    printf("Creature in the room: %s\n", currentRoom->creature.name);
    printf("Creature health: %d\n", currentRoom->creature.health);
    printf("Creature strength: %d\n", currentRoom->creature.strength);

    // Attack the creature
    currentRoom->creature.health -= state->player.strength;
    printf("You attacked the creature.\n");

    // Check if the creature is defeated
    if (currentRoom->creature.health <= 0)
    {
        printf("The creature is defeated.\n");
        currentRoom->creature.name = NULL;
    }
    else
    {
        // Creature attacks the player
        state->player.health -= (currentRoom->creature.strength - state->player.defense * 0.5);
        printf("The creature attacked you.\n");

        // Check if the player is defeated
        if (state->player.health <= 0)
        {
            printf("You are defeated.\n");
            printf("Game over.\n");
            exit(EXIT_SUCCESS);
        }
    }
}

void useItem(GameState *state)
{
    if (state->player.inventory.length == 0)
    {
        printf("Your inventory is empty.\n");
        return;
    }

    printf("Items in your inventory:\n");
    for (int i = 0; i < state->player.inventory.length; i++)
    {
        Item item = state->player.inventory.items[i];
        printf("%d. %s\n", i + 1, item.name);
    }

    int choice;
    printf("Enter the number of the item you want to use: ");
    scanf("%d", &choice);

    if (choice < 1 || choice > state->player.inventory.length)
    {
        printf("Invalid choice.\n");
        return;
    }

    Item item = state->player.inventory.items[choice - 1];
    if (item.is_special)
    {
        // Check if item name is
        // BlackSword, GreatSword, Dagger, IceSpell , FireSpell or Excalibur
        // If so, increase player's strength by item.strength only for 1 turn

        if (strcmp(item.name, "BlackSword") == 0 || strcmp(item.name, "GreatSword") == 0 || strcmp(item.name, "Dagger") == 0 || strcmp(item.name, "IceSpell") == 0 || strcmp(item.name, "FireSpell") == 0 || strcmp(item.name, "Excalibur") == 0)
        {
            state->player.strength += item.strength;
            printf("You used %s.\n", item.name);
            printf("Your strength is increased by %d for one turn.\n", item.strength);

            // Remove the item from the player's inventory
            removeItem(&state->player.inventory, item);

            // Attack the creature
            attackCreature(state);

            // Decrease the player's strength back to normal
            state->player.strength -= item.strength;
        }
        // If item name is LeatherArmour or Helmet increase the player health by item.strength
        else if (strcmp(item.name, "LeatherArmour") == 0 || strcmp(item.name, "Helmet") == 0)
        {
            state->player.defense += item.strength;
            printf("You used %s.\n", item.name);
            printf("Your defence is increased permenantly by %d.\n", item.strength);

            // Remove the item from the player's inventory
            removeItem(&state->player.inventory, item);
        }
        else if (strcmp(item.name, "HealthPotion") == 0 || strcmp(item.name, "GreatHealthPotion") == 0)
        {
            if (state->player.health = 100)
            {
                printf("Your HP is full. You cannot use health potion!!\n");
                return;
            }

            state->player.health += item.strength;
            if (state->player.health > 100)
                ;
            {
                state->player.health = 100;
            }

            printf("You used %s.\n", item.name);
            printf("Your health is increased by %d.\n", item.strength);

            // Remove the item from the player's inventory
            removeItem(&state->player.inventory, item);
        }
        else
        {
            printf("No Effect.\n");
        }
    }
    else
    {
        printf("You cannot use this item.\n");
    }
}

void movePlayer(GameState *state)
{
    Room *currentRoom = &state->rooms.rooms[state->current_room_id];

    int direction;
    printf("Enter the direction you want to move:\n");
    printf("1. Up\n");
    printf("2. Down\n");
    printf("3. Left\n");
    printf("4. Right\n");
    printf("Enter your choice: ");
    scanf("%d", &direction);

    int nextRoomId = -1;
    switch (direction)
    {
    case 1:
        nextRoomId = currentRoom->up;
        break;
    case 2:
        nextRoomId = currentRoom->down;
        break;
    case 3:
        nextRoomId = currentRoom->left;
        break;
    case 4:
        nextRoomId = currentRoom->right;
        break;
    default:
        printf("Invalid choice.\n");
        return;
    }

    if (nextRoomId == -1)
    {
        printf("You cannot move in that direction.\n");
        return;
    }

    state->current_room_id = nextRoomId;
}

void displayStatus(GameState *state)
{
    printf("Player status:\n");
    printf("Health: %d\n", state->player.health);
    printf("Strength: %d\n", state->player.strength);
    printf("Defense: %d\n", state->player.defense);
    printf("Current room ID: %d\n", state->current_room_id);
}

void showInventory(GameState *state)
{
    printf("Player inventory:\n");
    for (int i = 0; i < state->player.inventory.length; i++)
    {
        Item item = state->player.inventory.items[i];
        printf("%s\n", item.name);
    }
}
