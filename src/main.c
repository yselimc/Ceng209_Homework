#include "engine.h"

int main()
{
    GameState *state = initGame();

    char choice;
    do
    {
        printf("\nChoose an action:\n");
        printf("0. Quit\n");
        printf("1. Look around\n");
        printf("2. Pick an item\n");
        printf("3. Drop an item\n");
        printf("4. Attack a creature\n");
        printf("5. Use an item in combat\n");
        printf("6. Move to another room\n");
        printf("7. Display player status\n");
        printf("8. Show inventory\n");
        printf("9. Save game\n");
        printf("Enter your choice: ");
        scanf(" %c", &choice);

        switch (choice)
        {
        case '1':
            lookAround(state);
            break;
        case '2':
            pickItem(state);
            break;
        case '3':
            dropItem(state);
            break;
        case '4':
            attackCreature(state);
            break;
        case '5':
            useItem(state);
            break;
        case '6':
            movePlayer(state);
            break;
        case '7':
            displayStatus(state);
            break;
        case '8':
            showInventory(state);
            break;
        case '9':
            printf("Enter the name of the save file: ");
            char saveFile[256];
            scanf("%s", saveFile);
            saveGameState(state, saveFile);
            break;
        }

    } while (choice != '0');
    return 0;    
}