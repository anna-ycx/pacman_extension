#include "raylib.h"
#include <stdio.h>
#include "screens/screens.h"

#define SCREENWIDTH 800
#define SCREENHEIGHT 465

typedef enum GameScreen { LOADING, TITLE, INSTRUCT, GAMEPLAY, ENDING } GameScreen;

int main(void){
  InitWindow(SCREENWIDTH, SCREENHEIGHT, "Pacman");
  GameScreen currentScreen = LOADING;
  InitLoadingScreen();
  SetTargetFPS(60);
  
  //init game when screen = gameplay
  
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    switch (currentScreen) {
      case LOADING:
        DrawLoadingScreen();
        break;
      case TITLE:
        DrawTitleScreen();
        break;
      case INSTRUCT:
        DrawInstructScreen();
        break;
      case GAMEPLAY:
       DrawGameplayScreen();
        break;
      case ENDING:
        DrawEndingScreen();
        break;
    } 
    EndDrawing();
    
    switch (currentScreen) {
      case LOADING:
        UpdateLoadingScreen();
        if (FinishLoadingScreen()) {
            UnloadLoadingScreen();
            currentScreen = TITLE;
            InitTitleScreen();
        }
        break;
        
      case TITLE:
        UpdateTitleScreen();
        if(FinishTitleScreen() == 1){
            
          //don't need to unload title screen as theres nothing on it
          currentScreen = INSTRUCT;
          InitInstructScreen();
        } else if(FinishTitleScreen() == 2){
          currentScreen = GAMEPLAY;
          InitGameplayScreen(3);
        }
        break;
      case INSTRUCT:
        UpdateInstructScreen();
        if (FinishInstructScreen() == 1) {
          currentScreen = GAMEPLAY;
          InitGameplayScreen(3);
        } else if(FinishInstructScreen() == 2){
          currentScreen = TITLE;
          InitTitleScreen();
        }
        break;
      case GAMEPLAY:
        UpdateGameplayScreen();
        if (FinishGameplayScreen() == 0) {
            UnloadGameplayScreen();
            InitEndingScreen();
            currentScreen = ENDING;
        } else if (FinishGameplayScreen() > 0) {
            //UnloadGameplayScreen();
            InitLoseLife();
        }
        /*
        } else if (FinishGameplayScreen() == 2) {
            UnloadGameplayScreen();
            InitGameplayScreen(2);
        } else if (FinishGameplayScreen() == 3) {
            UnloadGameplayScreen();
            InitGameplayScreen(3);
        }
        */
        break;
      case ENDING:
        UpdateEndingScreen();
        if(FinishEndingScreen() == 1) {
            CloseWindow();
        }
        break;
    }
  }
  
  //unload 
  CloseWindow();
  return 0;
}

