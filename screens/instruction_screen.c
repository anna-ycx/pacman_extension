#include "screens.h"
#include "raylib.h"
#include "animation.h"

static int framesCounter;
static int finishScreen;

void InitInstructScreen(void) {
    framesCounter = 0;
    finishScreen = 0;
  return;
}

void UpdateInstructScreen(void) {
      framesCounter++;
  if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
      finishScreen = 1;
  } else if(IsKeyPressed(KEY_B)){
      finishScreen = 2;
  }
}

void DrawInstructScreen(void) {
  DrawRectangle(0,0,GetScreenWidth() * 2,GetScreenHeight() * 2,BLACK);
    DrawText("INSTRUCTIONS", GetScreenWidth()/2 - 318,GetScreenHeight()/2 - 160, 80, DARKBLUE);
    DrawText("Use the arrow keys to move PACMAN around. The aim is to collect all of the dots without losing your lives.",
    10, GetScreenHeight()/2 - 40, 15, DARKBLUE);
    DrawText("Make sure to avoid the ghosts! Every time you hit a ghost you lose a life. ",
    10, GetScreenHeight()/2 - 20 , 15, DARKBLUE);
    DrawText("If you eat a big dot you will gain the power to eat ghosts. This only lasts for 30 seconds so be careful.",
    10, GetScreenHeight()/2  , 15, DARKBLUE);
        DrawText("Press ENTER or CLICK to play now, or press B to go back to the title screen",
    10, GetScreenHeight()/2 + 20 , 15, DARKBLUE);
    Vector2 pacmanCenter = {(13 * SQUARE_SIZE) + SQUARE_SIZE/2 + X_OFFSET,
                            (23 * SQUARE_SIZE) + SQUARE_SIZE/2 + Y_OFFSET };
    DrawPacman(pacmanCenter, LEFT, framesCounter);
}

//ignore instruction screen for now
int FinishInstructScreen(void) {
  return finishScreen;
}
