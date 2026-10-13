#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>

#define FIELD_WIDTH 12 
#define FIELD_HEIGHT 18
#define SCREEN_WIDTH 80 
#define SCREEN_HEIGHT 30 

struct timespec delay = {
    .tv_sec = 0,
    .tv_nsec = 50000000L
};

char tetrominoShapes[7][17] = {
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
    
    switch (r % 4)
    {
        case 0: return py * 4 + px; // 0 deg
        case 1: return 12 + py - (px * 4); // 90 deg
        case 2: return 15 - (py * 4) - px; // 180 deg
        case 3: return 3 - py + (px * 4); // 270 deg
    }
    return 0;
}

bool doesPieceFit(int tetromino, int rotation, int posX, int posY) {

    for (int px = 0; px < 4; px++)
        for (int py = 0; py < 4; py++) {
            // Get index into piece
            int pi = rotate(px, py, rotation);
            // Get index into field
            int fi = (posY + py) * FIELD_WIDTH + (posX + px);

            if (posX + px >= 0 && posX + px < FIELD_WIDTH)
                if (posY + py >= 0 && posY + py < FIELD_HEIGHT) {
                    if(tetrominoShapes[tetromino][pi] == 'X' && field[fi] !=0) return false;
                }
        }

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

void updateKeys(bool keys[4]) {
    for (int i = 0; i < 4; i++)
        keys[i] = false;

    struct pollfd pfd = {
        .fd = STDIN_FILENO,
        .events = POLLIN,
    };

    while (poll(&pfd, 1, 0) > 0 && (pfd.revents & POLLIN)) {
        char c;
        if (read(STDIN_FILENO, &c, 1) != 1) break;
        switch (c) {
            case 'd':
                keys[0] = true;
                break;
            case 'a':
                keys[1] = true;
                break;
            case 's':
                keys[2] = true;
                break;
            case 'z':
                keys[3] = true;
                break;
        }

        pfd.revents = 0;
    }
}
        


int main() {

    // some linux bullshit
    struct termios oldt, newt;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_cc[VMIN] = 0;
    newt.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Clear terminal and hide cursor
    printf("\033[2J\033[?25l");

    // Assets

    bool gameOver = false;
    int currentPiece = 0;
    int currentRotation = 0;
    int currentX = FIELD_WIDTH / 2;
    int currentY = 0;
    bool keys[4] = {false};
    bool rotateHold = true;

    int lines[4];
    int lineCount = 0;

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
        nanosleep(&delay, NULL);
        

        // INPUT
        updateKeys(keys);

        printf("\rkeys: [%d, %d, %d, %d]   ",
            keys[0], keys[1], keys[2], keys[3]);
        fflush(stdout);

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
        for (int px = 0; px < 4; px++)
            for (int py = 0; py < 4; py++)
                if (tetrominoShapes[currentPiece][rotate(px,py,currentRotation)] == 'X')
                    screen[(currentY + py + 2)*SCREEN_WIDTH + (currentX + px + 2)] = currentPiece + 65;

        // Draw score
            // Display Frame before clearing lines
            // Display Frame after clearing lines

        dispplayFrame(screen);
    }

    // GAME OVER
    return 0;
}

