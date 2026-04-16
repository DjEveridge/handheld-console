#ifndef GAMESTATE_H
#define GAMESTATE_H

enum GameState {
  MENU,
  PONG,
  SNAKE
};

extern GameState currentGame;

void backToMenu();

#endif