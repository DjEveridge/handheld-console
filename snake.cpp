#include "snake.h"
#include "display.h"
#include "IRinput.h"

enum SnakeGameState {
  Start,
  Paused,
  Playing,
  GameOver
};

static SnakeGameState currentState = Start;

void snakeInit() {
  Serial.println("Sanke Init");
  currentState = Start;
}

void snakeUpdate() {
  handleGameState();
}

void snakeRender() {
  u8g2.drawStr(20,20, "Snake Render");
}

void handleGameState() {
  int value = getIRInput();
  if(currentState == Start && value == 0x30CF) currentState = Playing;
  if(currentState == Playing && value == 0x02FD) currentState = Paused;
  if(currentState == GameOver && value == 0x30CF) currentState = Start;
}