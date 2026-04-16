#include "menu.h"
#include "display.h"
#include "IRinput.h"
#include "gamestate.h"

static GameState selectedGame = MENU;

void menuUpdate() {
  int input = getIRInput();
  if(input == -1) return;
  if(input == 0x30CF) {
    selectedGame = PONG;
  }
  if(input == 0x18E7) selectedGame = SNAKE;
  if(input == 0x6897) selectedGame = MENU;
  Serial.println(input);
}

GameState menuGetSelection() {
  GameState temp = selectedGame;
  selectedGame = MENU;
  return temp;
}

void menuRender() {
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(30, 20, "Select Game");
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(10,40, "1: PONG");
    u8g2.drawStr(70,40, "2: SNAKE");
}