#include "raylib.h"
#include "screens.h"
#include <stdio.h>
#include "animation.h"

static int framesCounter;
static int finishScreen;

void InitTitleScreen(void) {
    framesCounter = 0;
    finishScreen = 0;
}

void UpdateTitleScreen(void) {
    framesCounter++;
  if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
      finishScreen = 2;
  } else if(IsKeyPressed(KEY_I)){
      finishScreen = 1;
  } 
  return;
}

void DrawTitleScreen(void){
    DrawRectangle(0,0,GetScreenWidth() * 2,GetScreenHeight() * 2,BLACK);
    DrawText("PACMAN", GetScreenWidth()/2 - 168,GetScreenHeight()/2 - 80, 80, DARKBLUE);
    DrawText("PRESS ENTER to start or I to view instructions", GetScreenWidth()/2 - 240, GetScreenHeight()/2 , 20, DARKBLUE);
    Vector2 pacmanCenter = {(13 * SQUARE_SIZE) + SQUARE_SIZE/2 + X_OFFSET,
                            (23 * SQUARE_SIZE) + SQUARE_SIZE/2 + Y_OFFSET };
    DrawPacman(pacmanCenter, LEFT, framesCounter);
}

int FinishTitleScreen(void){
    return finishScreen;
}