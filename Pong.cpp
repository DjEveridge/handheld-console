#include <U8g2lib.h>
#include <Wire.h>
#include "pitches.h"
#include <TimerFreeTone.h>
#include "Pong.h"
#include "display.h"
#include "IRinput.h"

const byte buttonDownPin = 4;
const byte buttonUpPin = 7;
const byte joystickXPin = A3;
const byte joystickYPin = A2;
const byte swPin = 2;
const byte buzzerPin = 8;
bool lastButtonState = HIGH;
bool currentButtonState;
bool cheatActive = false;
bool gameStarted = false;
bool gameOver = false;
bool gamePaused = false;
bool gameQuit = false;
float velScaler;
unsigned long velResetTime = 0;
int hitCount = 0;
unsigned long lastValue = 0;


//paddles
const int leftPaddleX = 8;
int leftPaddleY = 8;
const int rightPaddleX = 116;
int rightPaddleY = 8;
const byte paddleWidth = 4;
const byte paddleHeight = 10;
const byte paddleSpeed = 5;
//ball
byte ballX = 60;
byte ballY = 20;
const byte ballRadius = 3;
float ballVelX = random(6, 10) * (1 + velScaler)/3.0;
float ballVelY = random(6,10) * (1 + velScaler)/3.0;
//score
byte leftPlayerScore = 0;
byte rightPlayerScore = 0;
byte leftScoreX = 46;
byte leftScoreY = 8;
byte rightScoreX = 66;
byte rightScoreY = 8;
byte scoreDividerX = 60;
byte scoreDividerY = 0;
byte scoreDividerWidth = 2;
byte scoreDividerHeight = 5;
const byte scoreToWin = 10;


void buzz(String);
void restart();
void quit();

void drawGameObjects() {
  //Draw paddles
  u8g2.drawBox(leftPaddleX, leftPaddleY,paddleWidth,paddleHeight);
  u8g2.drawBox(rightPaddleX, rightPaddleY,paddleWidth,paddleHeight);
  //Draw Ball
  u8g2.drawFilledEllipse(ballX, ballY, ballRadius, ballRadius, U8G2_DRAW_ALL);
  //Draw Ball trail
  for(int i = 2; i <= 3; i++) {
    u8g2.drawFilledEllipse(ballX - (int)(i * ballVelX), ballY - (int)(i * ballVelY), max(1, ballRadius - i), max(1, ballRadius - i), U8G2_DRAW_ALL);
  }
  //Draw Score
  char leftScoreBuffer[10];
  sprintf(leftScoreBuffer, "%d", leftPlayerScore);
  u8g2.drawStr(leftScoreX, leftScoreY, leftScoreBuffer);

  u8g2.drawLine(scoreDividerX, scoreDividerY, scoreDividerX, scoreDividerY + scoreDividerHeight);

  char rightScoreBuffer[10];
  sprintf(rightScoreBuffer, "%d", rightPlayerScore);
  u8g2.drawStr(rightScoreX, rightScoreY, rightScoreBuffer);
}

void moveBall() {
  velScaler = min(1.0, hitCount / 10.0);
  ballX += ballVelX * (1 + velScaler);
  ballY += ballVelY * (1 + velScaler);
  Serial.println(velScaler);
}

void changeVel() {
  ballVelX = -ballVelX;
  ballVelY = -ballVelY * random(2,3)/2.0;
}

void checkBallCollision() {
  //left paddle
if ((ballX - ballRadius) < leftPaddleX + paddleWidth &&
    (ballX + ballRadius) > leftPaddleX &&
    (ballY + ballRadius) > leftPaddleY &&
    (ballY - ballRadius) < leftPaddleY + paddleHeight) {
    
    ballX = leftPaddleX + paddleWidth + ballRadius;
    ballVelX = -ballVelX;
    hitCount++;
    buzz("Paddle");
}
  //right paddle
  if((ballX + ballRadius) > rightPaddleX && 
    (ballX-ballRadius) < rightPaddleX + paddleWidth&&
    (ballY - ballRadius) < rightPaddleY + paddleHeight && 
    (ballY + ballRadius) > (rightPaddleY)) {
    ballVelX = -ballVelX;
    ballX = rightPaddleX - ballRadius - 1;
    hitCount++;
    buzz("Paddle");
  } 

  if((ballX + ballRadius) > 128) {
    ballX = 60;
    ballY = 20;
    ballVelX = random(6,10) * (1+velScaler)/3;
    ballVelY = random(6,10) * (1+velScaler)/3;
    leftPlayerScore++;
    hitCount = 0;
    buzz("OffScreen");
  } 
  if((ballX - ballRadius) < 0) {
    ballX = 60;
    ballY = 20;
    ballVelX = random(6,10) * (1+velScaler)/3;
    ballVelY = random(6,10) * (1+velScaler)/3;
    rightPlayerScore++;
    hitCount = 0;
    buzz("OffScreen");
  }
  if((ballY - ballRadius) < 0 || (ballY + ballRadius) > 64) {
    ballVelY = -ballVelY;
    buzz("Top/Bottom");
  }
}

void movePaddles() {
  //left paddle
  if(analogRead(joystickYPin) < 400) {
    leftPaddleY -= paddleSpeed;
  }
  if(analogRead(joystickYPin) > 600) {
    leftPaddleY += paddleSpeed;
  }

  //right paddle
  if(digitalRead(buttonDownPin) == LOW) {
    rightPaddleY += paddleSpeed;
  }
  if(digitalRead(buttonUpPin) == LOW) {
    rightPaddleY -= paddleSpeed;
  }

  //don't let paddles go off screen
  if(leftPaddleY < 2) leftPaddleY = 2;
  if((leftPaddleY + paddleHeight) > 62) leftPaddleY = 62 - paddleHeight;

  if(rightPaddleY < 2) rightPaddleY = 2;
  if((rightPaddleY + paddleHeight) > 62) rightPaddleY = 62 - paddleHeight;

  //Dev cheat
  currentButtonState = digitalRead(swPin);
  if(lastButtonState == HIGH && currentButtonState == LOW) {
    cheatActive = !cheatActive;
  }
  lastButtonState = currentButtonState;
  if(cheatActive) {
    leftPaddleY = ballY - 5;
  }
}

void startScreen() {
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(40, 20, "Pong");
  u8g2.setFont(u8g2_font_4x6_tr);
  u8g2.drawStr(10, 40, "Press right button to start");
  if(digitalRead(buttonUpPin) == LOW) {
    gameStarted = true;
    restart();
  }
}

void checkForWin() {
  if(rightPlayerScore >= scoreToWin || leftPlayerScore >= scoreToWin) gameOver = true;
}

void displayWinner() {
  u8g2.setFont(u8g2_font_6x10_tr);
  if(leftPlayerScore >= scoreToWin) u8g2.drawStr(16,30, "Player 1 Wins!!!");
  if(rightPlayerScore >= scoreToWin) u8g2.drawStr(16,30, "Player 2 Wins!!!");
  u8g2.drawStr(12, 40, "1 to Play Again");
}

void buzz(String contactArea) {

  if(contactArea == "Paddle") {
    TimerFreeTone(buzzerPin, 523, 20);
  } else if(contactArea == "Top/Bottom") {
    TimerFreeTone(buzzerPin, 1046, 20);
  } else if(contactArea == "OffScreen") {
    TimerFreeTone(buzzerPin, 784, 20);
  }

}




void setupIR() {
  int input = getIRInput();

  if (input == -1) return; // no button pressed




    if (input == 0x02FD) {
      gamePaused = !gamePaused;
    }

    if (!gameQuit && input == 0x30CF) {
      restart();
    }

    if (input == 0x18E7) {
      quit();
    }

    if (gameQuit && input == 0x30CF) {
      gameQuit = false;
      gameStarted = false;
      gameOver = false;
      gamePaused = false;
    }

    if (gameOver && input == 0x30CF) {
      restart();
    }
    if(gamePaused && input == 0x7A85) {

    }

}

  void displayPauseMenu() {
    u8g2.drawStr(20, 30, "Pause to resume");
    u8g2.drawStr(10, 40, "1 to Restart");
    u8g2.drawStr(10, 50, "2 to Quit");
  }
  void restart() {
    gamePaused = false;
    gameStarted = true;
    gameOver = false;
    cheatActive = false;

    leftPaddleY = 8;
    rightPaddleY = 8;

  ballX = 60;
  ballY = 20;
  ballVelX = random(6, 10) * (1 + velScaler)/3.0;
  ballVelY = random(6,10) * (1 + velScaler)/3.0;

  leftPlayerScore = 0;
  rightPlayerScore = 0;
  }

  void displayQuitScreen() {
    u8g2.drawStr(20, 30, "Thanks For Playing!");
    u8g2.drawStr(10, 40, "1 to Play Again");
  }

  void quit() {
    gameQuit = true;
    gamePaused = false;
    gameStarted = false;
    gameOver = false;
  }

  void pongInit() {
    pinMode(buttonDownPin, INPUT_PULLUP);
    pinMode(buttonUpPin, INPUT_PULLUP);
    pinMode(swPin, INPUT_PULLUP);

  }

  void pongUpdate() {
      checkForWin();
      setupIR();
      Serial.println("Pong updating");
    if(gameStarted && !gameOver && !gamePaused) {
      moveBall();
      checkBallCollision();
      movePaddles();
    }
  }

void pongRender() {

      u8g2.setFont(u8g2_font_6x10_tr);
      if (gameQuit) {
        displayQuitScreen();
        }else if(gameStarted && !gameOver){
        drawGameObjects();
        if(gamePaused) displayPauseMenu();
        } else if (gameOver) {
          displayWinner();
        }  else {
          startScreen();
        }


}

void pongStart() {
}
