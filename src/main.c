#include "game.h"
#include <string.h>
#include <stdio.h>


void display_menu()
{
    printf("Available commands, type the number:\n");
    printf(" (1) status           : Displays player status.\n");
    printf(" (2) move <direction> : Move in a direction (up, down, left, right).\n");
    printf(" (3) look             : Look around the current floor.\n");
    printf(" (4) inventory        : Show your inventory.\n");
    printf(" (5) pickup <item>    : Pick up an item in the floor.\n");
    printf(" (6) drop <item>      : Drop an item in the floor.\n");
    printf(" (7) attack           : Attack a creature in the floor.\n");
    printf(" (8) save <filepath>  : Save the game state to a file.\n");
    printf(" (9) load <filepath> : Load the game state from a file.\n");
    printf(" (e) exit            : Exit the game.\n");
}

int main()
{
    // Initialize the game
    GameState *state = init_game();

    // Display the welcome message
    printf("Welcome to the Dungeon Adventure Game!\n");
    char command[50];
    // Main game loop
    while (1)
    {
        printf("\n");
        display_menu();
        printf("\n> ");
        scanf("%s", command);

        if (strcmp(command, "status") == 0 || strcmp(command, "1") == 0)
        {
            display_player_status(state);
        }
        else if (strcmp(command, "move") == 0 || strcmp(command, "2") == 0)
        {
            move_player(state);
        }
        else if (strcmp(command, "look") == 0 || strcmp(command, "3") == 0)
        {
            look_around(state);
        }
        else if (strcmp(command, "inventory") == 0 || strcmp(command, "4") == 0)
        {
            show_inventory(&state->player);
        }
        else if (strcmp(command, "pickup") == 0 || strcmp(command, "5") == 0)
        {
            pick_item(state);
        }
        else if (strcmp(command, "drop") == 0 || strcmp(command, "6") == 0)
        {
            drop_item(state);
        }
        else if (strcmp(command, "attack") == 0 || strcmp(command, "7") == 0)
        {
            attack_creature(state);
        }
        else if (strcmp(command, "save") == 0 || strcmp(command, "8") == 0)
        {
            char filepath[MAX_DESC];
            printf("Enter the file path to save the game: ");
            scanf("%s", filepath);
            save_game_state(state, filepath);
        }
        else if (strcmp(command, "load") == 0 || strcmp(command, "9") == 0)
        {
            char filepath[MAX_DESC];
            printf("Enter the file path to load the game: ");
            scanf("%s", filepath);
            load_game_state(state, filepath);
        }
        else if (strcmp(command, "exit") == 0 || strcmp(command, "e") == 0)
        {
            printf("Exiting the game.\n");
            break;
        }
        else
        {
            printf("Unknown command. Type 'help' for a list of commands.\n");
        }
    }
    
}
