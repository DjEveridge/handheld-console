#include "gamestate.h"
#include "display.h"
#include "menu.h"
#include "Pong.h"
#include "snake.h"
#include "IRinput.h"


GameState currentGame = MENU;


void setup() {
  // put your setup code here, to run once:


  Serial.begin(9600);



  randomSeed(analogRead(A0));
  displayInit();
  irInit();
}

void loop() {

  //  FIRST: handle state change


  //  THEN: run current state
  switch (currentGame) {

    case MENU:
      menuUpdate();
      break;

    case PONG:
      pongUpdate();
      break;
    case SNAKE:
      snakeUpdate();
      break;
  }

  backToMenu();
  if (currentGame == MENU) {
    GameState next = menuGetSelection();
  

    if (next != MENU) {
      currentGame = next;

      if (currentGame == PONG) {
        pongInit();
        pongStart();
      }
      if(currentGame == SNAKE) snakeInit();
    }
  }

  //  RENDER (this part is fine)
  u8g2.firstPage();
  do {
    switch (currentGame) {
      case MENU:
        menuRender();
        break;
      case PONG:
        pongRender();
        break;
      case SNAKE:
        snakeRender();
        break;
    }
  } while (u8g2.nextPage());
}




