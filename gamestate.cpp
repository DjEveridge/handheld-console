#include "menu.h"
#include "IRinput.h"
#include "gamestate.h"

void backToMenu() {
  int value = getIRInput();
  if(value == 0x6897) GameState next = currentGame = MENU;
}