#include <assert.h>
#include "raylib.h"
#include "screens.h"
#include "../graphs.h"
#include "animation.h"
//figure out how many there actually are
#define NO_CONSUMABLES 248
#define NO_GHOSTS 3

#define GHOST_SPEED MOVE_FRAMES

Vector2 Direction[5] = {{0, 0}, {0, -1}, {0, 1}, {1, 0}, {-1, 0}};



typedef struct Consumable {
  node_t *position;
  bool powerUp;
  bool used;
  //image and size?
} Consumable;

typedef struct Entity {
  node_t *position;
  node_t *starting;
  DirCode direction;
  GhostMove behavior;
  int framesUntilActive;
  int moveProgress;
  bool player;
} Entity;

static bool gameOver;
static bool pause;
static bool won;

static DirCode queuedDir;

//init with correct sizes and images here
static Entity pacman;
static Entity ghost1;
static Entity ghost2;
static Entity ghost3;
static Entity *ghosts[NO_GHOSTS];
static Consumable *consumables[NO_CONSUMABLES];
static bool allowMove;
static bool poweredUp;
static int powerFramesRemaining;

static int score;
static int lives;

static int framesCounter;
static int endScreen;

static char **grid;
static graph_t *graph;
static nid_t **routing_table;

//x is a wall
//- is a pac dot space
//. is an empty space
//p is a powerup
//g for ghost spawn
//a for pacman
char **GetGrid() {
  //TODO: make this a file writing/general thing to support multiple maps
  char **grid = malloc(HEIGHT * sizeof(char *));
  grid[0] = strdup("xxxxxxxxxxxxxxxxxxxxxxxxxxxx");
  grid[1] = strdup("x------------xx------------x");
  grid[2] = strdup("x-xxxx-xxxxx-xx-xxxxx-xxxx-x");
  grid[3] = strdup("xpxxxx-xxxxx-xx-xxxxx-xxxxpx");
  grid[4] = strdup("x-xxxx-xxxxx-xx-xxxxx-xxxx-x");
  grid[5] = strdup("x--------------------------x");
  grid[6] = strdup("x-xxxx-xx-xxxxxxxx-xx-xxxx-x");
  grid[7] = strdup("x-xxxx-xx-xxxxxxxx-xx-xxxx-x");
  grid[8] = strdup("x------xx----xx----xx------x");
  grid[9] = strdup("xxxxxx-xxxxx-xx-xxxxx-xxxxxx");
  grid[10] = strdup("xxxxxx-xxxxx-xx-xxxxx-xxxxxx");
  grid[11] = strdup("xxxxxx-xx..........xx-xxxxxx");
  grid[12] = strdup("xxxxxx-xx.xxxddxxx.xx-xxxxxx");
  grid[13] = strdup("xxxxxx-xx.x....g.x.xx-xxxxxx");
  grid[14] = strdup("......-...x.g....x...-......");
  grid[15] = strdup("xxxxxx-xx.x...g..x.xx-xxxxxx");
  grid[16] = strdup("xxxxxx-xx.xxxxxxxx.xx-xxxxxx");
  grid[17] = strdup("xxxxxx-xx..........xx-xxxxxx");
  grid[18] = strdup("xxxxxx-xx.xxxxxxxx.xx-xxxxxx");
  grid[19] = strdup("xxxxxx-xx.xxxxxxxx.xx-xxxxxx");
  grid[20] = strdup("x------------xx------------x");
  grid[21] = strdup("x-xxxx-xxxxx-xx-xxxxx-xxxx-x");
  grid[22] = strdup("x-xxxx-xxxxx-xx-xxxxx-xxxx-x");
  grid[23] = strdup("xp--xx-------a.-------xx--px");
  grid[24] = strdup("xxx-xx-xx-xxxxxxxx-xx-xx-xxx");
  grid[25] = strdup("xxx-xx-xx-xxxxxxxx-xx-xx-xxx");
  grid[26] = strdup("x------xx----xx----xx------x");
  grid[27] = strdup("x-xxxxxxxxxx-xx-xxxxxxxxxx-x");
  grid[28] = strdup("x-xxxxxxxxxx-xx-xxxxxxxxxx-x");
  grid[29] = strdup("x--------------------------x");
  grid[30] = strdup("xxxxxxxxxxxxxxxxxxxxxxxxxxxx");

  return grid;
}


void AddGhost(int number, int x, int y) {
  Entity *ghost = NULL;
  switch (number) {
    case 0:
      ghost = &ghost1;
      break;
    case 1:
      ghost = &ghost2;
      break;
    case 2:
      ghost = &ghost3;
      break;
  }
  ghost->player = false;
  ghost->position = search(graph, x, y);
  ghost->starting = ghost->position;
  ghost->direction = STILL;
  ghost->behavior = STATIONARY;
  ghost->framesUntilActive = (300 * number) + 50;
  ghosts[number] = ghost;
}


void AddConsumablesAndEntities() {
  int GhostCount = 0;
  int ConsumableCount = 0;
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < HEIGHT; x++) {
      switch(grid[y][x]) {
        case '-':
          {
          Consumable *dot = malloc(sizeof(Consumable));
          dot->position = search(graph, x, y);
          assert(dot->position);
          dot->powerUp = false;
          dot->used = false;
          consumables[ConsumableCount++] = dot;
          break;
          }
        case 'p':
          {
          Consumable *pup = malloc(sizeof(Consumable));
          pup->position = search(graph, x, y);
          assert(pup->position);
          pup->powerUp = true;
          pup->used = false;
          consumables[ConsumableCount++] = pup;
          break;
          }
        case 'g':
          if (GhostCount < NO_GHOSTS) {
            AddGhost(GhostCount++, x, y);
          }
        case 'a':
          pacman.position = search(graph, x, y);
          pacman.starting = pacman.position;
          pacman.player = true;
          assert(pacman.position);
          break;
        default:
          break;
      }
    }
  }
}

void InitGameplayScreen(int lifeCount) {
  gameOver = false;
  pause = false;
  won = false;
  
  queuedDir = 0;
  
  allowMove = true;
  poweredUp = false;
  powerFramesRemaining = 0;
  
  score = 0;
  lives = lifeCount;
  
  framesCounter = 0;
  endScreen = -1;
  
  grid = (char **) GetGrid();
  graph = populate(grid, count_nodes(grid));
  routing_table = generate(graph);
  AddConsumablesAndEntities();
}

//reset positions but keep state of consumables when pacman loses a life
void InitLoseLife() {
  gameOver = false;
  allowMove = true;
  endScreen = -1;
  for (int i = 0; i < NO_GHOSTS; i++) {
    ghosts[i]->position = ghosts[i]->starting;
    ghosts[i]->direction = STILL;
    ghosts[i]->behavior = STATIONARY;
    ghosts[i]->moveProgress = 0;
    ghosts[i]->framesUntilActive = (300 * i) + 50;
  }
  pacman.position = pacman.starting;
  pacman.direction = STILL;
  pacman.moveProgress = 0;
}

void FreeAll() {
  //free consumables array
  for (int i = 0; i < NO_CONSUMABLES; i++) {
    free(consumables[i]);
  }
  //free board representations
  free_graph(graph);
  free(grid);
}

//return true iff all pac dots have been eaten
bool CheckWin(void) {
  for (int i = 0; i < NO_CONSUMABLES; i++) {
    if (!consumables[i]->powerUp && consumables[i]->used == false) {
      return false;
    }
  }
  return true;
}

//compare two nodes
bool nodeEquals(node_t *fst, node_t *snd) {
    return (fst->x == snd->x) && (fst->y == snd->y);
}


//queue direction if moving, set if not 
bool trySetMovement(DirCode direction) {  
  //if still moving from one square to another, queue direction
  if (!allowMove) {
    queuedDir = direction;
    return false;
  }
  //else try to get next node in new direction
  node_t *next = try_move(graph, pacman.position, Direction[direction]);
  if (next) {
    //if not continuing in straight line then keep the last queued
    if (pacman.direction == STILL) {
      queuedDir = 0;
    }
    pacman.position = next;
    pacman.direction = direction;
    pacman.moveProgress = 0;
    allowMove = false;
    return true;
  }
  return false;
}

//check for key presses adn try to set movement
void getMovement() {
  //if stationary 
  if (IsKeyPressed(KEY_RIGHT)) {
    trySetMovement(RIGHT);
  } else if (IsKeyPressed(KEY_LEFT)) {
    trySetMovement(LEFT);
  } else if (IsKeyPressed(KEY_UP)) {
    trySetMovement(UP);
  } else if (IsKeyPressed(KEY_DOWN)) {
    trySetMovement(DOWN);
  }
}

void setGhostBehavior(GhostMove behavior) {
    for (int i = 0; i < NO_GHOSTS; i++) {
        ghosts[i]->behavior = behavior;
    }
}


//return the direction code for the direction from the current to the target node
DirCode getDirection(node_t *current, node_t *target) {
  if (target->x - current->x > 0) {
    return RIGHT;
  } else if (target->x - current->x < 0) {
    return LEFT;
  } else if (target->y - current->y > 0) {
    return DOWN;
  } else if (target->y - current->y < 0) {
    return UP;
  }
  //otherwise return STILL
  return UP;
}

//set the position and direction of ghost
void moveGhost(int i, node_t *target) {
  ghosts[i]->moveProgress = 0;
  node_t *next = next_in_path(graph, routing_table, ghosts[i]->position, target);
  ghosts[i]->direction = getDirection(ghosts[i]->position, next);
  ghosts[i]->position = next;
}


void UpdateGameplayScreen(void) {
    if (!gameOver) {
        if (IsKeyPressed('P')) {
            pause = !pause;
        }
        if (!pause) {
            if (framesCounter % 60 == 0) {
              printf("Ghost 0 is now %d\n", ghosts[0]->behavior);
              printf("Ghost 1 is now %d\n", ghosts[1]->behavior);
              printf("Ghost 2 is now %d\n\n", ghosts[2]->behavior);
            }
            framesCounter++;
            pacman.moveProgress++;
            //player control
            getMovement();
            
            //if player is on a grid square i.e. is allowed to change direction
            if (pacman.direction && pacman.moveProgress == MOVE_FRAMES) {
              pacman.moveProgress = 0;
              allowMove = true;
              //if no queued movement or can't move in queued direction
              if (!(queuedDir != STILL && trySetMovement(queuedDir))) {
                //try to continue in current direction
                if(!trySetMovement(pacman.direction)) {
                  //else stop pacman
                  pacman.direction = STILL;
                }
              }
            }

            //move and revive ghosts
            for (int i = 0; i < NO_GHOSTS; i++) {
                switch (ghosts[i]->behavior) {
                    case CHASE:
                        ghosts[i]->moveProgress++;
                        if (ghosts[i]->moveProgress == GHOST_SPEED) {
                          moveGhost(i, pacman.position);
                        }
                        break;
                    case FLEE:
                        ghosts[i]->moveProgress++;
                        //if ghost has reached a node
                        if (ghosts[i]->moveProgress == GHOST_SPEED) {
                          srand(framesCounter);
                          //25% chance to pick new direction
                          if (rand() % 4 == 0) {
                            ghosts[i]->direction = (rand() % 4) + 1;
                          }
                          //try to get the next node 
                          node_t *next = try_move(graph, ghosts[i]->position, Direction[ghosts[i]->direction]);
                          //if can't move further
                          while(!next) {
                            ghosts[i]->direction = (rand() % 4) + 1;
                            next = try_move(graph, ghosts[i]->position, Direction[ghosts[i]->direction]);
                          }
                          ghosts[i]->position = next;
                          ghosts[i]->moveProgress = 0;
                        }
                        break;
                    case RETURN:
                        //if a ghost is returning when the powerup time ends, it will start chasing 
                        ghosts[i]->moveProgress++;
                        //if reached the starting node, set inactive until end of powerup or for 5 seconds
                        if (nodeEquals(ghosts[i]->position, ghosts[i]->starting)) {
                          ghosts[i]->behavior = STATIONARY;
                          ghosts[i]->framesUntilActive = 5 * 60;
                        }
                        
                        //otherwise move ghost back to starting node
                        if (ghosts[i]->moveProgress == GHOST_SPEED) {
                          moveGhost(i, ghosts[i]->starting);
                        }
                        break;
                    default:
                        break;
                }
                
                if (ghosts[i]->framesUntilActive > 0) {
                    ghosts[i]->framesUntilActive--;
                    if (ghosts[i]->framesUntilActive == 0) {
                        ghosts[i]->behavior = CHASE;
                    }
                }                    
            }
            
            //control powerups
            if (poweredUp) {
                powerFramesRemaining--;
                if (powerFramesRemaining == 0) {
                    poweredUp = false;
                    setGhostBehavior(CHASE);
                    
                }
            }
            
            //check collision with consumables
            for (int i = 0; i < NO_CONSUMABLES; i++) {
                if (nodeEquals(pacman.position, consumables[i]->position) && !consumables[i]->used) {
                    consumables[i]->used = true;
                    score++;
                    if (consumables[i]->powerUp) {
                        score += 4;
                        poweredUp = true;
                        //1 minute powerup?
                        powerFramesRemaining = 10 * 60;
                        setGhostBehavior(FLEE);
                    }
                }
            }
            //check collision with ghosts
            for (int i = 0; i < NO_GHOSTS; i++) {
                if (nodeEquals(pacman.position, ghosts[i]->position)) {
                    //if a ghost if fleeing and powerup is enabled
                    if (poweredUp && ghosts[i]->behavior == FLEE) {
                        score += 10;
                        ghosts[i]->behavior = RETURN;
                    //if a ghost is returning to spawn ignore collision
                    } else if (ghosts[i]->behavior != RETURN){
                        //player death
                        gameOver = true;
                        won = false;
                    }
                }
            }
            //player wins
            if (CheckWin()) {
                gameOver = true;
                won = true;
            }
            
        } else {
            //pause screen 
           
        }
    //if a life is lost 
    } else {
      
    }
}

void DrawWalls() {
  DrawRectangle(X_OFFSET,
                Y_OFFSET,
                SQUARE_SIZE * WIDTH, 
                SQUARE_SIZE * HEIGHT, 
                DARKBLUE);
}

void DrawDoor() {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      if (grid[y][x] == 'd') {
          DrawRectangle(X_OFFSET + x * SQUARE_SIZE,
                Y_OFFSET + y * SQUARE_SIZE,
                SQUARE_SIZE, 
                SQUARE_SIZE, 
                WHITE);
      }
    }
  }
}

void DrawNodes() {
  for (int i = 0; i < graph->size; i++) {
    if (grid[graph->nodes[i]->y][graph->nodes[i]->x] != 'd') {
      DrawRectangle(graph->nodes[i]->x * SQUARE_SIZE + X_OFFSET - 6,
                    graph->nodes[i]->y * SQUARE_SIZE + Y_OFFSET - 6,
                    SQUARE_SIZE + 12, 
                    SQUARE_SIZE + 12, 
                    BLACK);
    }
  }
}

void DrawConsumables() {
  for (int i = 0; i < NO_CONSUMABLES; i++) {
    if (consumables[i] != NULL && !consumables[i]->used) {
      int x = consumables[i]->position->x;
      int y = consumables[i]->position->y;
      if (consumables[i]->powerUp) {
        DrawCircle((x * SQUARE_SIZE) + (SQUARE_SIZE / 2) + X_OFFSET,
                  (y * SQUARE_SIZE) + (SQUARE_SIZE / 2) + Y_OFFSET,
                  SQUARE_SIZE/2 - 2,
                  WHITE);
      } else {
        DrawRectangle(x * SQUARE_SIZE + (SQUARE_SIZE / 2) + X_OFFSET - 1,
                      y * SQUARE_SIZE + (SQUARE_SIZE / 2) + Y_OFFSET - 1,
                      3, 
                      3, 
                      WHITE);
      }
    }
  }
}


Vector2 getCenter(Entity entity) {
  return (Vector2) {(entity.position->x * SQUARE_SIZE) + (SQUARE_SIZE / 2) + X_OFFSET - 
                      (Direction[entity.direction].x * SQUARE_SIZE * (1 - ((float) entity.moveProgress / MOVE_FRAMES))),
                    (pacman.position->y * SQUARE_SIZE) + (SQUARE_SIZE / 2) + Y_OFFSET - 
                      (Direction[entity.direction].y * SQUARE_SIZE * (1 - ((float) entity.moveProgress / MOVE_FRAMES)))};
}


void DrawGhosts() {
  Color color1 = RED;
  Color color2 = PINK;
  Color color3 = SKYBLUE;
  if (ghosts[0]->behavior == FLEE) {
      color1 = DARKBLUE;
  }
  if (ghosts[1]->behavior == FLEE) {
      color2 = DARKBLUE;
  }
  if (ghosts[2]->behavior == FLEE) {
      color3 = DARKBLUE;
  }
    Vector2 ghost1Center = (Vector2) {(ghost1.position->x * SQUARE_SIZE) + (SQUARE_SIZE / 2) + X_OFFSET - 
                      (Direction[ghost1.direction].x * SQUARE_SIZE * (1 - ((float) ghost1.moveProgress / GHOST_SPEED))),
                      (ghost1.position->y * SQUARE_SIZE) + (SQUARE_SIZE / 2) + Y_OFFSET - 
                      (Direction[ghost1.direction].y * SQUARE_SIZE * (1 - ((float) ghost1.moveProgress / GHOST_SPEED)))};
    DrawGhost(ghost1Center, ghost1.direction, framesCounter, color1, ghost1.behavior);
    Vector2 ghost2Center = (Vector2) {(ghost2.position->x * SQUARE_SIZE) + (SQUARE_SIZE / 2) + X_OFFSET - 
                      (Direction[ghost2.direction].x * SQUARE_SIZE * (1 - ((float) ghost2.moveProgress / GHOST_SPEED))),
          (ghost2.position->y * SQUARE_SIZE) + (SQUARE_SIZE / 2) + Y_OFFSET - 
                      (Direction[ghost2.direction].y * SQUARE_SIZE * (1 - ((float) ghost2.moveProgress / GHOST_SPEED)))};
    DrawGhost(ghost2Center, ghost2.direction, framesCounter, color2,ghost2.behavior);
    Vector2 ghost3Center = (Vector2){(ghost3.position->x * SQUARE_SIZE) + (SQUARE_SIZE / 2) + X_OFFSET - 
                      (Direction[ghost3.direction].x * SQUARE_SIZE * (1 - ((float) ghost3.moveProgress / GHOST_SPEED))),
          (ghost3.position->y * SQUARE_SIZE) + (SQUARE_SIZE / 2) + Y_OFFSET - 
                      (Direction[ghost3.direction].y * SQUARE_SIZE * (1 - ((float) ghost3.moveProgress / GHOST_SPEED)))};
    DrawGhost(ghost3Center, ghost3.direction, framesCounter, color3, ghost3.behavior);
}

void DrawPacMan() {
    Vector2 pacmanCenter = getCenter(pacman);
    DrawPacman(pacmanCenter,pacman.direction, framesCounter);
}

void DrawHUD() {
  //draw pause info (top left)
  DrawText("Press 'p'\nto pause", PADDING, PADDING, TEXT_SIZE, WHITE);
  //draw current score (top right)
  DrawText(FormatText("Score: \n%07i", score),
           X_OFFSET + (SQUARE_SIZE * WIDTH) + PADDING, PADDING, TEXT_SIZE, WHITE);
  //draw powerup indicator if necessary
  if (poweredUp) {
      int secondsLeft = powerFramesRemaining/60;
      DrawText(FormatText("Powered up \nfor %02i:%02i!", secondsLeft/60, secondsLeft%60), 
            X_OFFSET + (SQUARE_SIZE * WIDTH) + PADDING, (PADDING + GetScreenHeight()/2)/2, TEXT_SIZE, RED);
  }
  //draw number of lives left
  DrawText(FormatText("Lives: %d", lives), PADDING, GetScreenHeight()/2, TEXT_SIZE, WHITE);
  int radius = 12;
  for (int i = 0; i < lives; i++) {
    Vector2 centre = {radius + i * (3 * radius) + PADDING,
                      GetScreenHeight()/2 + PADDING + TEXT_SIZE};
    DrawCircleSector(centre, radius, 130, 410, 0, YELLOW);
  }
  //draw the elapsed time
  int minutes = framesCounter / (60 * 60);
  DrawText(FormatText("Timer: \n%02i:%02i", minutes, (framesCounter - (minutes * 60 * 60)) / 60),
           X_OFFSET + (SQUARE_SIZE * WIDTH) + PADDING,
           GetScreenHeight()/2, TEXT_SIZE, WHITE);
}

void DrawPause(){
     DrawText("GAME PAUSED", GetScreenWidth()/2 - MeasureText("GAME PAUSED", 40)/2, GetScreenHeight()/2 - 40, 40, GRAY);
}

void DrawGameOver(){
    if (!won && lives > 1) {
            char *lifeLost = "YOU LOST A LIFE!";
            char *tryLifeAgain = "PRESS [ENTER] TO TRY AGAIN!";
            DrawText(lifeLost, GetScreenWidth()/2 - MeasureText(lifeLost, 20)/2, GetScreenHeight()/2 - 100, 20, GRAY);
            DrawText(tryLifeAgain, GetScreenWidth()/2 - MeasureText(tryLifeAgain, 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);
            if (IsKeyPressed(KEY_ENTER)) {
                endScreen = --lives;
            }
        } else {
            //game over
            char *victoryText = "YOU WON!";
            //Time: \n%02i:%02i", minutes, (framesCounter - (minutes * 60 * 60)) / 60));
            char *tryAgain = "PRESS [ENTER] TO PLAY AGAIN OR [BACKSPACE] TO QUIT!";
            char *lossText = "YOU LOST! BIG OOF :^(";
            if (won) {
                DrawText(victoryText, GetScreenWidth()/2 - MeasureText(victoryText, 20)/2, GetScreenHeight()/2 - 100, 20, GRAY);
            } else {
                DrawText(lossText, GetScreenWidth()/2 - MeasureText(lossText, 20)/2, GetScreenHeight()/2 - 100, 20, GRAY);
            }
            DrawText(tryAgain, GetScreenWidth()/2 - MeasureText(tryAgain, 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);
            if (IsKeyPressed(KEY_ENTER)) {
                endScreen = 3;
            } else if (IsKeyPressed(KEY_BACKSPACE)) {
                endScreen = 0;
            }
        }
}

void DrawGameplayScreen(void) {
  //draw walls - for all walls in char array draw white square
  DrawWalls();
  DrawDoor();
  DrawNodes();
  DrawConsumables();
  DrawPacMan();
  DrawGhosts();
  DrawHUD();
  if(pause){
      DrawPause();
  }
  if(gameOver){
      DrawGameOver();
  }
}

void UnloadGameplayScreen(void) {
    FreeAll();
}

int FinishGameplayScreen(void) {
  return endScreen;
}
