#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define FIELD_WIDTH 12 
#define FIELD_HEIGHT 18
#define SCREEN_WIDTH 80 
#define SCREEN_HEIGHT 30 

char tetromino[7][17] = {
    "..X...X...X...X.",
    "..X..XX..X......",
    ".X...XX...X.....",
    ".....XX..XX.....",
    "..X..XX...X.....",
    ".....XX...X...X.",
    ".....XX..X...X.."
};

unsigned char *field = NULL;

int rotate(int px, int py, int r) {
    return 0;
}

bool doesPieceFit(int tetromino, int rotation, int posX, int posY) {
    return true;
}

void dispplayFrame(char *screen) {
    printf("\033[H");

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        fwrite(screen + y * SCREEN_WIDTH, sizeof(char), SCREEN_WIDTH, stdout);
        putchar('\n');
    }

    fflush(stdout);
}

int main() {

    // Clear terminal and hide cursor
    printf("\033[2J\033[?25l");

    // Assets

    bool gameOver = false;
    int currentPiece = 0;
    int currentRotation = 0;
    int currentX = FIELD_WIDTH / 2;
    int currentY = 0;
    bool Keys[4] = {};
    bool rotateHold = true;

    // Play field buffer
    field = malloc(FIELD_WIDTH * FIELD_HEIGHT * sizeof(unsigned char));
    // Board Boundaries
    for (int x = 0; x < FIELD_WIDTH; x++)
        for (int y = 0; y < FIELD_HEIGHT; y++)
            field[y * FIELD_WIDTH + x] = (x == 0 || x == FIELD_WIDTH - 1 || y == FIELD_HEIGHT - 1 ) ? 9 : 0;

    char *screen = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(char));
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) screen[i] = ' ';

    while (!gameOver) {

        // GAME TIMING

        // INPUT

        // GAME LOGIC
            // Lock the current piece in the field
            // Chech have we got any lines
                // Remove Line, set to =
            // Chose next piece
            // If piece does not fit

        // RENDER OUTPUT
        // Draw Field
        for (int x = 0; x < FIELD_WIDTH; x++)
            for (int y = 0; y < FIELD_HEIGHT; y++)
                screen[(y + 2) * SCREEN_WIDTH + (x + 2)] = " ABCDEFG=#"[field[y * FIELD_WIDTH + x]];
        // Draw Piece
        // Draw score
            // Display Frame before clearing lines
            // Display Frame after clearing lines

        dispplayFrame(screen);
    }

    // GAME OVER
    return 0;
}

