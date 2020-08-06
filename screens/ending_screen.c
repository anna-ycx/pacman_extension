#include "screens.h"
#include "raylib.h"

static int finishScreen;
static int framesCounter;

void InitEndingScreen(void) {
    finishScreen = 0;
    framesCounter =0;
  return;
}

void UpdateEndingScreen(void) {
  framesCounter++;
  if (framesCounter >= 300) {
      finishScreen = 1;
  }
  return;
}

void DrawEndingScreen(void) {
    DrawRectangle(0,0,GetScreenWidth() * 2,GetScreenHeight() * 2,BLACK);
    DrawText("THANKS FOR PLAYING", GetScreenWidth()/2 - 318,GetScreenHeight()/2 - 160, 40, DARKBLUE);
    DrawText("This game was made by Yuan Chen, Sophie Elliott, Vincent Wang and Hannah Watson.",
    30, GetScreenHeight()/2 - 40, 15, DARKBLUE);
    DrawText("Utilizing the raylib graphics library for our C project extension.",
    30, GetScreenHeight()/2 - 20 , 15, DARKBLUE);
  return;
}

int FinishEndingScreen(void) {
  return finishScreen;
}