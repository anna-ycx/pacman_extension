#include "raylib.h"

#ifndef ANIMATION_H
#define ANIMATION_H

typedef enum {STILL, UP, DOWN, RIGHT, LEFT} DirCode;

typedef enum {STATIONARY, CHASE, FLEE, RETURN} GhostMove;

void DrawGhost(Vector2 center, DirCode direction, int framesCounter, Color color, GhostMove behavior);

void DrawPacman(Vector2 center, DirCode direction, int framesCounter);
#endif