#ifndef MAIN_H
#define MAIN_H

#include "gba.h"

void updateOpponentCar(const unsigned short* image, int* opponentIndex);
int handleCollisions(void);
void drawText(char* driverNames[], unsigned short colors[]);
void swap(int index1, int index2, char* driverNames[], unsigned short colors[]);

// TODO: Create any necessary structs

struct Car {
    double x;
    double y;
    const u16* image;
    int passed;
};

/*
* For example, for a Snake game, one could be:
*
* struct snake {
*   int heading;
*   int length;
*   int row;
*   int col;
* };
*
* Example of a struct to hold state machine data:
*
* struct state {
*   int currentState;
*   int nextState;
* };
*
*/

#endif
