//starting coordinates of game space
#define X_OFFSET 190
#define Y_OFFSET 0

//size of a grid square
#define SQUARE_SIZE 15

#define TEXT_SIZE 20
#define PADDING 25

#define MOVE_FRAMES 10

#ifndef SCREENS_H
#define SCREENS_H

void InitLoadingScreen(void);
void UpdateLoadingScreen(void);
void DrawLoadingScreen(void);
int FinishLoadingScreen(void);
void UnloadLoadingScreen(void);

void InitTitleScreen(void);
void UpdateTitleScreen(void);
void DrawTitleScreen(void);
int FinishTitleScreen(void);

void InitInstructScreen(void);
void UpdateInstructScreen(void);
void DrawInstructScreen(void);
int FinishInstructScreen(void);

void InitGameplayScreen(int lifeCount);
void InitLoseLife();
void UpdateGameplayScreen(void);
void DrawGameplayScreen(void);
int FinishGameplayScreen(void);
void UnloadGameplayScreen(void);

void InitEndingScreen(void);
void UpdateEndingScreen(void);
void DrawEndingScreen(void);
int FinishEndingScreen(void);

#endif 