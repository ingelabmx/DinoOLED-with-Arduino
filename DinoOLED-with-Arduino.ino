#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BUTTON_PIN 2

// Suelo
const int GROUND_Y = 54;

// Dinosaurio
int dinoX = 18;
float dinoY = GROUND_Y - 12;
float dinoVelY = 0;
bool isJumping = false;

const int dinoW = 12;
const int dinoH = 12;

// Física
const float gravity = 0.55;
const float jumpForce = -5.8;

// Obstáculo
int obsX = 128;
int obsY = GROUND_Y - 10;
int obsW = 8;
int obsH = 10;
int gameSpeed = 3;

// Puntos
unsigned long score = 0;
bool gameOver = false;

// Debounce
unsigned long lastButtonTime = 0;
const unsigned long debounceDelay = 60;

bool buttonPressed() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (millis() - lastButtonTime > debounceDelay) {
      lastButtonTime = millis();
      return true;
    }
  }
  return false;
}

void resetGame() {
  dinoY = GROUND_Y - dinoH;
  dinoVelY = 0;
  isJumping = false;

  obsX = 128;
  score = 0;
  gameSpeed = 3;
  gameOver = false;
}

void drawDino(int x, int y) {
  // cuerpo
  display.fillRect(x + 2, y + 2, 8, 8, SSD1306_WHITE);
  // cabeza
  display.fillRect(x + 8, y, 4, 4, SSD1306_WHITE);
  // pata
  display.drawLine(x + 3, y + 10, x + 3, y + 12, SSD1306_WHITE);
  display.drawLine(x + 8, y + 10, x + 8, y + 12, SSD1306_WHITE);
}

void drawObstacle(int x, int y) {
  display.fillRect(x, y, obsW, obsH, SSD1306_WHITE);
  display.drawLine(x + 2, y, x + 2, y - 4, SSD1306_WHITE);
  display.drawLine(x + 5, y, x + 5, y - 3, SSD1306_WHITE);
}

bool checkCollision() {
  int dLeft = dinoX;
  int dRight = dinoX + dinoW;
  int dTop = (int)dinoY;
  int dBottom = (int)dinoY + dinoH;

  int oLeft = obsX;
  int oRight = obsX + obsW;
  int oTop = obsY - 4;
  int oBottom = obsY + obsH;

  if (dRight > oLeft && dLeft < oRight && dBottom > oTop && dTop < oBottom) {
    return true;
  }
  return false;
}

void updateDino() {
  if (buttonPressed() && !isJumping && !gameOver) {
    dinoVelY = jumpForce;
    isJumping = true;
  }

  dinoY += dinoVelY;
  dinoVelY += gravity;

  if (dinoY >= GROUND_Y - dinoH) {
    dinoY = GROUND_Y - dinoH;
    dinoVelY = 0;
    isJumping = false;
  }
}

void updateObstacle() {
  obsX -= gameSpeed;

  if (obsX + obsW < 0) {
    obsX = 128 + random(10, 35);
    score++;
    
    if (score % 5 == 0 && gameSpeed < 7) {
      gameSpeed++;
    }
  }
}

void drawGround() {
  display.drawLine(0, GROUND_Y, SCREEN_WIDTH - 1, GROUND_Y, SSD1306_WHITE);

  for (int i = 0; i < SCREEN_WIDTH; i += 16) {
    display.drawPixel((i + (millis() / 20) % 16) % SCREEN_WIDTH, GROUND_Y + 2, SSD1306_WHITE);
  }
}

void drawScore() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(90, 0);
  display.print("P:");
  display.print(score);
}

void drawGameOver() {
  display.setTextSize(1);
  display.setCursor(35, 20);
  display.print("GAME OVER");

  display.setCursor(18, 35);
  display.print("Presiona para reiniciar");
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }

  randomSeed(analogRead(A0));

  display.clearDisplay();
  display.display();

  resetGame();
}

void loop() {
  display.clearDisplay();

  if (!gameOver) {
    updateDino();
    updateObstacle();

    if (checkCollision()) {
      gameOver = true;
    }

    drawGround();
    drawDino(dinoX, (int)dinoY);
    drawObstacle(obsX, obsY);
    drawScore();
  } else {
    drawGround();
    drawDino(dinoX, (int)dinoY);
    drawObstacle(obsX, obsY);
    drawScore();
    drawGameOver();

    if (buttonPressed()) {
      delay(150);
      resetGame();
    }
  }

  display.display();
  delay(25);
}