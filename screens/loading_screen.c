#include "screens.h"
#include "raylib.h"
#include <stdlib.h>
#include "animation.h"

static int framesCounter;
static int finishScreen;
Vector2 pacmanCenter = {(17 * SQUARE_SIZE) + SQUARE_SIZE/2 + X_OFFSET,
                            (15 * SQUARE_SIZE) + SQUARE_SIZE/2 + Y_OFFSET };
DirCode *mouthDirection;
bool *isDotVisible;

void InitLoadingScreen(void) {
    isDotVisible = malloc(sizeof(bool) * 16);
    for(int i = 0; i < 16; i++){
        isDotVisible[i] = true;
    }
    mouthDirection = malloc(sizeof(DirCode));
    *mouthDirection = RIGHT;
    framesCounter = 0;
    finishScreen = 0;
}

void UpdateLoadingScreen(void) {
    framesCounter++;
    if(framesCounter == 500){
        finishScreen = 1;
    }
}

void DrawLoadingScreen(void) {
    int x = 10;
    int y = 15;
    char *loading[7] = {"L", "O", "A", "D", "I", "N", "G"};
    DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(),BLACK);
    for (int i = 0; i < 7; i++){
        DrawText(loading[i],(x * SQUARE_SIZE) + X_OFFSET,
        (y * SQUARE_SIZE) + Y_OFFSET, SQUARE_SIZE, WHITE);
        x++;
    }
    //DrawText("LOADING", x, y, SQUARE_SIZE * 1.666, WHITE);
    //x += SQUARE_SIZE * 5;
    for(int i = 0; i < framesCounter / 15; i++){
        if(isDotVisible[i] == true){
        DrawRectangle((x * SQUARE_SIZE) + SQUARE_SIZE/2 + X_OFFSET,
        (y * SQUARE_SIZE) + SQUARE_SIZE/2 + Y_OFFSET, 3, 3, WHITE);}
        if(i < 2) {
            x++;
        }else if (i < 10 ) {
            y++;
        }else if (i < 16 ){
            x--;
        }
    }
    
    if(framesCounter > 270 && framesCounter < 300){
        DrawCircleSector(pacmanCenter, SQUARE_SIZE/2+2, 90, (90 + 360/30 * (framesCounter - 270)) , 1, YELLOW);
    }else if (framesCounter > 300){
        float speed = (float)SQUARE_SIZE/ (float) MOVE_FRAMES;//make this match later
        DrawPacman(pacmanCenter, *mouthDirection, framesCounter);
          isDotVisible[(framesCounter - 300) / MOVE_FRAMES] = false;
          if(framesCounter <= 320){
              *mouthDirection = RIGHT;
              pacmanCenter.x += speed;
          } else if(framesCounter <= 400){
              *mouthDirection = DOWN;
              pacmanCenter.y += speed;
          } else if (framesCounter <= 460) {
              *mouthDirection = LEFT;
              pacmanCenter.x -= speed;
          }
        
        //pacmanCenter.y += speed;
    }
}

//ignore loading screen for now
int FinishLoadingScreen(void) {
  return finishScreen;
}

void UnloadLoadingScreen(void) {
    free(isDotVisible);
    free(mouthDirection);
}

