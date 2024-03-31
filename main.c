#include "main.h"

#include <stdio.h>
#include <string.h>
#include "images/redbull.h"
#include "images/leftCurb.h"
#include "images/rightCurb.h"
#include "images/startScreen.h"
#include "images/explosion.h"
#include "images/mclaren.h"
#include "images/mercedes.h"
#include "images/ferrari.h"
#include "images/aston.h"
#include "images/scoreBackground.h"
#include "images/loseScreen.h"
#include "images/winScreen.h"
#include "images/startScreen2.h"
#include "images/redbullLeft.h"
#include "images/redbullRight.h"
#include "images/finishLine.h"

#include "gba.h"

enum gba_state {
    START,
    PLAY,
    EXPLOSION,
    WIN,
    LOSE,
};

struct Car playerCar;
struct Car opponentCar;
int explosionStartTime;



int main(void) {
    /* TODO: */
    // Manipulate REG_DISPCNT here to set Mode 3. //
    REG_DISPCNT = BG2_ENABLE | MODE3;
    // Save current and previous state of button input.
    u32 previousButtons = BUTTONS;
    u32 currentButtons = BUTTONS;

    // Load initial application state
    enum gba_state state = START; //START;
    int offset = 0;

    while (1) {
        currentButtons = BUTTONS; // Load the current state of the buttons

        // Manipulate the state machine below as needed //
        // NOTE: Call waitForVBlank() before you draw
        waitForVBlank();
        switch (state) {
            case START:

                //initializing variables
                playerCar.x = 60;
                playerCar.y = 105;
                playerCar.image = redbull;

                opponentCar.x = -100;
                opponentCar.y = -100;
                opponentCar.passed = 0;

                const unsigned short* carArray[5][1155] = {{redbull}, {aston}, {mclaren}, {ferrari}, {mercedes}};
                char* driverNames[5] ={"VER", "ALO", "NOR", "LEC", "HAM"};
                unsigned short colors[] = {
                        BLUE, GREEN, ORANGE, RED, TURQUOISE
                };

                int opponentIndex = 1;
                int finishLineY = 0;

                if (vBlankCounter%120 < 60) {
                    drawFullScreenImageDMA(startScreen);
                }
                else {
                    drawFullScreenImageDMA(startScreen2);
                }
                if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
                    fillScreenDMA(GRAY);
                    drawText(driverNames, colors);
                    state = PLAY;
                }
                break;
            case PLAY:
                if (opponentIndex >= 5 ) { //hacky timer
                    if (opponentIndex > 75) {
                        if (finishLineY < HEIGHT) {
                            drawRectDMA(finishLineY, LEFTCURB_WIDTH, FINISHLINE_WIDTH, FINISHLINE_HEIGHT, GRAY);
                        }
                        finishLineY += 5;
                        if (finishLineY < HEIGHT) {
                        drawImageDMA(finishLineY, LEFTCURB_WIDTH, FINISHLINE_WIDTH, FINISHLINE_HEIGHT, finishLine);
                    }   }

                    opponentIndex++;
                }
                if (finishLineY > HEIGHT + 200) {
                    state = WIN;
                }
                drawImageDMA(playerCar.y, playerCar.x, REDBULL_WIDTH, REDBULL_HEIGHT, playerCar.image);
                updateOpponentCar(*carArray[opponentIndex], &opponentIndex);
                if (opponentCar.passed == 1) { //if pass opponent, update leaderboard
                    swap (opponentIndex-1, opponentIndex, driverNames, colors);
                    drawRectDMA(50, 157, 50, 100, GRAY);
                    drawText(driverNames, colors);
                }
                drawWrappingImage(offset, 0, LEFTCURB_WIDTH, LEFTCURB_HEIGHT, leftCurb);
                drawWrappingImage(offset, 110, RIGHTCURB_WIDTH, RIGHTCURB_HEIGHT, rightCurb);
                if (handleCollisions()) {
                    explosionStartTime = vBlankCounter;
                    state = EXPLOSION;
                }
                //controls
                if (KEY_DOWN(BUTTON_LEFT, currentButtons) && playerCar.x > 0 + LEFTCURB_WIDTH) {
                    playerCar.x -= 1;
                    playerCar.image = redbullLeft;
                }
                else if (KEY_DOWN(BUTTON_RIGHT, currentButtons) && playerCar.x + REDBULL_WIDTH < 110) {
                    playerCar.x += 1;
                    playerCar.image = redbullRight;
                }
                else {
                    playerCar.image = redbull;
                }
                if (KEY_DOWN(BUTTON_DOWN, currentButtons) && playerCar.y + REDBULL_HEIGHT - 40 < HEIGHT) {
                    playerCar.y += 1;
                }
                if (KEY_DOWN(BUTTON_UP, currentButtons) && playerCar.y > 0) {
                    playerCar.y -= 1;
                }
                /*
                else {
                    if (playerCar.y < HEIGHT-REDBULL_HEIGHT) {
                        playerCar.y += 0.5;
                    }
                }
                 */

                offset += 7;
                if (offset > HEIGHT) {
                    offset = 0;
                }



                break;
            case EXPLOSION:
                drawExplosionSlow(playerCar.y-60, playerCar.x - 50, EXPLOSION_WIDTH, EXPLOSION_HEIGHT, explosion);
                if (vBlankCounter - explosionStartTime > 5) {
                    drawFullScreenImageDMA(loseScreen);
                    state = LOSE;
                }
                break;
            case WIN:
                drawFullScreenImageDMA(winScreen);
                break;
            case LOSE:
                setPixel(0,0,BLUE);
                char line1[] = "Crashed while";
                drawString(65, 148, line1, WHITE);
                char line2[] = "overtaking ";
                if (opponentCar.passed != 0) {
                    strcat(line2, driverNames[opponentIndex-1]);
                }
                else {
                    strcat(line2, driverNames[opponentIndex]);
                }
                char line3[] ="for P";
                char place[2];
                sprintf(place, "%d", -opponentIndex+5);
                strcat(line3, place);

                drawString(75, 145, line2, WHITE);
                drawString(85, 163, line3, WHITE);
                break;
        }
        if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
            state = START;
        }
        previousButtons = currentButtons; // Store the current state of the buttons
    }

    UNUSED(previousButtons);

    return 0;
}

void updateOpponentCar(const unsigned short *image, int* opponentIndex2) {
    if (*opponentIndex2 < 5) {
        if (opponentCar.x == -100 && opponentCar.y == -100) {
            if (randint(0, 100) == 1) {
                opponentCar.x = playerCar.x;
                opponentCar.y = -52;
                opponentCar.passed = 0;
            }
        } else {
            opponentCar.y += 1.5;
            drawImageDMA(opponentCar.y, opponentCar.x, REDBULL_WIDTH, REDBULL_HEIGHT, image);
            if (opponentCar.y >= HEIGHT) {
                opponentCar.y = -100;
                opponentCar.x = -100;
                (*opponentIndex2)++;
            }
            if (opponentCar.y > playerCar.y) {
                if (opponentCar.passed == 0) {
                    opponentCar.passed = 1;
                } else {
                    opponentCar.passed = -1;
                }
            }
        }
    }
}

int handleCollisions(void) {
    if (playerCar.x + REDBULL_WIDTH-3 > opponentCar.x && // r1 right edge past r2 left
        playerCar.x < opponentCar.x + REDBULL_WIDTH-3 // r1 left edge past r2 right
        && playerCar.y < opponentCar.y + REDBULL_HEIGHT-3 // r1 top edge past r2 bottom
        && playerCar.y + REDBULL_HEIGHT-3 > opponentCar.y  // r1 bottom edge past r2 top
            ) {
        return 1;
    }
    return 0;
}

void drawText(char* driverNames[], unsigned short colors[]) {
    drawImageDMA(40, 150,SCOREBACKGROUND_WIDTH, SCOREBACKGROUND_HEIGHT, scoreBackground);
    for (int i = 0; i < 5; i++){
        char place[2];
        sprintf(place, "%d", (-i+5));
        drawString(50+10*(-i+5), 160, place, WHITE);
        drawString(50+10*(-i+5), 175, driverNames[i], WHITE);
        drawRectDMA(50+10*(-i+5), 157, 1, 8, colors[i]);
    };
}

void swap(int index1, int index2, char* driverNames[], unsigned short colors[]) {
    char* tempName = driverNames[index1];
    driverNames[index1] = driverNames[index2];
    driverNames[index2] = tempName;

    unsigned short tempColor = colors[index1];
    colors[index1] = colors[index2];
    colors[index2] = tempColor;
}
