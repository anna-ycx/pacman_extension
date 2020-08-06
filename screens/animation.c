#include "animation.h"

#define SQUARE_SIZE 15

void DrawGhost(Vector2 center, DirCode direction, int framesCounter, Color color, GhostMove behavior){
  //TODO add to animations
  Vector2 eyeAdjust = {0.0f,0.0f};
  switch(direction){
      case LEFT:
                eyeAdjust.x = -0.75f;
            break;
            case RIGHT: 
                eyeAdjust.x = 0.75f;
            break;
            case UP:
                 eyeAdjust.y = -0.75f;
            break;
            case DOWN:
                 eyeAdjust.y = 0.75f;
            break;
            default: break;
  }
  DrawCircle(center.x, center.y, SQUARE_SIZE/2 + 1, color);
  DrawCircle(center.x + 3, center.y - 2.5, 2.75, WHITE);
  DrawCircle(center.x - 3, center.y - 2.5, 2.75, WHITE);
  DrawCircle(center.x - 3 + eyeAdjust.x, center.y - 2.5 + eyeAdjust.y, 2, Fade(DARKBLUE, 0.5f));
  DrawCircle(center.x + 3 + eyeAdjust.x, center.y - 2.5 + eyeAdjust.y, 2, Fade(DARKBLUE, 0.5f));
}

void DrawPacman(Vector2 center, DirCode direction, int framesCounter){
    if (direction == STILL) {
        DrawCircle(center.x, center.y, SQUARE_SIZE/2 + 2, YELLOW);
    } else {
        int mouthDirection = 0;
        switch(direction){
            case LEFT:
                mouthDirection = 300;
            break;
            case RIGHT: 
                mouthDirection = 120;
            break;
            case UP:
                mouthDirection = 210;
            break;
            case DOWN:
                mouthDirection = 30;
            break;
            default:
                framesCounter = 18;
        }
        DrawCircleSector(center, SQUARE_SIZE/2 + 2, mouthDirection,
            ((framesCounter/9) % 2 == 0 ) ? mouthDirection + 300 : mouthDirection + 360, 1, YELLOW);
    }
}

