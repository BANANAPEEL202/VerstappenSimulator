#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include "images/background.h"
#include "images/redbull.h"
#include "images/leftCurb.h"
#include "images/rightCurb.h"
#include "images/startScreen.h"

#include "gba.h"
/* TODO: */
// Include any header files for title screen or exit
// screen images generated by nin10kit. Example for the provided garbage
// image:
// #include "images/garbage.h"

/* TODO: */
// Add any additional states you need for your app. You are not requried to use
// these specific provided states.
enum gba_state {
  START,
  PLAY,
  WIN,
  LOSE,
};

int main(void) {
  /* TODO: */
  // Manipulate REG_DISPCNT here to set Mode 3. //
  REG_DISPCNT = BG2_ENABLE | MODE3;
  // Save current and previous state of button input.
  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;

  struct Car playerCar;
  playerCar.x = 90;
  playerCar.y = 105;
  playerCar.image = redbull;

  // Load initial application state
  enum gba_state state = START; //START;
  int offset = 0;

  while (1) {
    currentButtons = BUTTONS; // Load the current state of the buttons

    /* TODO: */
    // Manipulate the state machine below as needed //
    // NOTE: Call waitForVBlank() before you draw
    waitForVBlank();
    switch (state) {
      case START:
          drawFullScreenImageDMA(startScreen);
          if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
              fillScreenDMA(GRAY);
              state = PLAY;
          }
        break;
        case PLAY:
            drawImageDMA(playerCar.y, playerCar.x, REDBULL_WIDTH, REDBULL_HEIGHT, playerCar.image);
            drawWrappingImage(offset, 0, LEFTCURB_WIDTH, LEFTCURB_HEIGHT, leftCurb);
            drawWrappingImage(offset, 110, RIGHTCURB_WIDTH, RIGHTCURB_HEIGHT, rightCurb);
            //controls
            if (KEY_DOWN(BUTTON_LEFT, currentButtons) && playerCar.x > 0+LEFTCURB_WIDTH) {
                playerCar.x -= 1;
            }
            if (KEY_DOWN(BUTTON_RIGHT, currentButtons) && playerCar.x+REDBULL_WIDTH < 110) {
                playerCar.x += 1;
            }
            if (KEY_DOWN(BUTTON_DOWN, currentButtons) && playerCar.y+REDBULL_HEIGHT-40<HEIGHT) {
                playerCar.y += 1;
            }
            if (KEY_DOWN(BUTTON_UP, currentButtons) && playerCar.y > 0) {
                playerCar.y -= 1;
            }

        // state = ?
        break;
      case WIN:
        // state = ?
        break;
      case LOSE:
        // state = ?
        break;
    }
    offset+=5;
    if (offset > HEIGHT) {
        offset=0;
    }
    previousButtons = currentButtons; // Store the current state of the buttons
  }

  UNUSED(previousButtons); // You can remove this once previousButtons is used

  return 0;
}
