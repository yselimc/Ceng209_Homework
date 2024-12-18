## Folder Structure

```
|-- src/
|   |-- game.c          # Core game logic
|   |-- main.c          # Entry point of the application
|   |-- parser.c        # File parsing functionality
|   |-- game.h          # Header file for game.c
|   |-- parser.h        # Header file for parser.c
|   |-- main.h          # (Optional) Header file for main.c, if used
|
|-- assets/
|   |-- rooms.txt       # Input file that defines the game rooms
|
|-- build/              # Compiled binaries and object files
|
|-- README.md           # Project documentation
```

## How to Build

To build the game, navigate to the root project folder and run the following command:

```sh
make
```

This will compile the source files and generate the executable in the `build/` directory.

## How to Play

1. Run the game executable from the `build/` directory:

    ```sh
    ./build/main.exe
    ```

2. Follow the on-screen instructions to navigate through the dungeon, interact with objects, and progress through the game.

## Dependencies

- A C compiler (e.g., `gcc`)