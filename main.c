/*
 * Tetris in C
 * Port based on OneLoneCoder's Command Line Tetris.
 *
 * Original work:
 * Copyright (C) 2018 Javidx9
 *
 * Adapted to C for Linux.
 * Copyright (C) 2026 Klaus Afton
 *
 * Licensed under the GNU General Public License v3.0.
 * See LICENSE for details.
 */

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
        case 0: return py * 4 + px;
        case 1: return 12 + py - (px * 4);
        case 2: return 15 - (py * 4) - px;
        case 3: return 3 - py + (px * 4);
    }
    return 0;
}

bool doesPieceFit(int tetromino, int rotation, int posX, int posY) {

    for (int px = 0; px < 4; px++)
        for (int py = 0; py < 4; py++) {
            int pi = rotate(px, py, rotation);
            int fi = (posY + py) * FIELD_WIDTH + (posX + px);

            if (posX + px >= 0 && posX + px < FIELD_WIDTH)
                if (posY + py >= 0 && posY + py < FIELD_HEIGHT) {
                    if(tetrominoShapes[tetromino][pi] == 'X' && field[fi] !=0) return false;
                }
        }

    return true;
}

void displayFrame(char *screen) {
    printf("\033[H");

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        fwrite(screen + y * SCREEN_WIDTH, sizeof(char), SCREEN_WIDTH, stdout);
        putchar('\n');
    }

    fflush(stdout);
}

void drawScreen(char *screen, int currentPiece, int currentRotation, int currentX, int currentY, int score) {
        for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
            screen[i] = ' ';

        for (int x = 0; x < FIELD_WIDTH; x++)
            for (int y = 0; y < FIELD_HEIGHT; y++)
                screen[(y + 2) * SCREEN_WIDTH + (x + 2)] = " ABCDEFG=#"[field[y * FIELD_WIDTH + x]];

        for (int px = 0; px < 4; px++)
            for (int py = 0; py < 4; py++)
                if (tetrominoShapes[currentPiece][rotate(px,py,currentRotation)] == 'X')
                    screen[(currentY + py + 2)*SCREEN_WIDTH + (currentX + px + 2)] = currentPiece + 65;
        
        char scoreText[16];
        snprintf(scoreText, sizeof(scoreText), "SCORE: %8d", score);

        for (int i = 0; i < 15; i++)
            screen[2 * SCREEN_WIDTH + FIELD_WIDTH + 6 + i] = scoreText[i];
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

    srand(time(NULL));
    struct termios oldt, newt;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_cc[VMIN] = 0;
    newt.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    printf("\033[2J\033[?25l");

    bool gameOver = false;
    int currentPiece = 0;
    int currentRotation = 0;
    int currentX = FIELD_WIDTH / 2;
    int currentY = 0;
    bool keys[4] = {false};
    bool rotateHold = true;

    int speed = 20;
    int speedCounter = 0;
    bool forceDown = false;
    int pieceCount = 0;
    int score = 0;

    int lines[4];
    int lineCount = 0;

    field = malloc(FIELD_WIDTH * FIELD_HEIGHT * sizeof(unsigned char));

    for (int x = 0; x < FIELD_WIDTH; x++)
        for (int y = 0; y < FIELD_HEIGHT; y++)
            field[y * FIELD_WIDTH + x] = (x == 0 || x == FIELD_WIDTH - 1 || y == FIELD_HEIGHT - 1 ) ? 9 : 0;

    char *screen = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(char));
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) screen[i] = ' ';

    while (!gameOver) {

        nanosleep(&delay, NULL);
        speedCounter++;
        forceDown = (speedCounter == speed);
        
        updateKeys(keys);

        currentX += (keys[0] && doesPieceFit(currentPiece, currentRotation, currentX + 1, currentY)) ? 1 : 0;
        currentX -= (keys[1] && doesPieceFit(currentPiece, currentRotation, currentX - 1, currentY)) ? 1 : 0;
        currentY += (keys[2] && doesPieceFit(currentPiece, currentRotation, currentX, currentY + 1)) ? 1 : 0;
        if (keys[3]) {
            currentRotation += (rotateHold && doesPieceFit(currentPiece, currentRotation + 1, currentX, currentY)) ? 1 : 0;
            rotateHold = false;
        } else {
            rotateHold = true;
        }

        if (forceDown) {
            if (doesPieceFit(currentPiece, currentRotation, currentX, currentY + 1))
                currentY++;
            else {
                for (int px = 0; px < 4; px++)
                    for (int py = 0; py < 4; py++)
                        if (tetrominoShapes[currentPiece][rotate(px,py,currentRotation)] == 'X')
                            field[(currentY + py) * FIELD_WIDTH + (currentX + px)] = currentPiece + 1;

                pieceCount++;
                if (pieceCount % 50 == 0 && speed >= 10) speed--;
    
                for (int py = 0; py < 4; py++)
                    if (currentY + py < FIELD_HEIGHT - 1) {
                        bool line = true;
                        for (int px = 1; px < FIELD_WIDTH - 1; px++)
                            line &= (field[(currentY + py) * FIELD_WIDTH + px]) != 0;
                        if (line) {
                            for (int px = 1; px < FIELD_WIDTH - 1; px++)
                                field[(currentY + py) * FIELD_WIDTH + px] = 8;
                            lines[lineCount++] = (currentY + py);
                        }
                    }
                
                    score += 25;
                    if(lineCount > 0) score += (1 << lineCount) * 100;

                    currentX = FIELD_WIDTH / 2;
                    currentY = 0;
                    currentRotation = 0;
                    currentPiece = rand() % 7;

                    gameOver = !doesPieceFit(currentPiece, currentRotation, currentX, currentY);
            }

            speedCounter = 0;
        }

        drawScreen(screen, currentPiece, currentRotation, currentX, currentY, score);

        if (lineCount > 0) {
            displayFrame(screen);
            nanosleep(&(struct timespec){
                .tv_sec = 0,
                .tv_nsec = 400000000L
            }, NULL);

            for (int i = 0; i < lineCount; i++) {
                int v = lines[i];

                for (int x = 1; x < FIELD_WIDTH - 1; x++){
                    for (int y = v; y > 0; y--)
                        field[y * FIELD_WIDTH + x] = field[(y - 1) * FIELD_WIDTH + x];
                
                    field[x] = 0;
                }
            }

            lineCount = 0;
        }
        drawScreen(screen, currentPiece, currentRotation, currentX, currentY, score);
        displayFrame(screen);
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    printf("\033[?25h\n");
    printf("Game Over!! Score: %d\n", score);
    free(field);
    free(screen);

    return 0;
}

