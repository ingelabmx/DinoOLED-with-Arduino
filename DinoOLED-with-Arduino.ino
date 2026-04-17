#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BUTTON_PIN 2

const int GROUND_Y = 54;
const int DINO_X = 18;
const int DINO_W = 16;
const int DINO_H = 16;
const int OBSTACLE_W = 10;
const int OBSTACLE_H = 12;

const float gravity = 0.72f;
const float jumpForce = -6.4f;
const uint8_t frameDelayMs = 20;

float dinoY = GROUND_Y - DINO_H;
float dinoVelY = 0;
bool isJumping = false;

int obsX = SCREEN_WIDTH;
int obsY = GROUND_Y - OBSTACLE_H;
int gameSpeed = 3;

unsigned long score = 0;
bool gameOver = false;

unsigned long lastButtonTime = 0;
const unsigned long debounceDelay = 60;
unsigned long lastFrameTime = 0;

int groundOffset = 0;
int dustX = -10;
int dustLife = 0;
bool blinkEye = false;

const unsigned char PROGMEM dinoRunFrameA[] = {
  0b00000111, 0b11111100,
  0b00000111, 0b11111110,
  0b00000110, 0b01111110,
  0b00000110, 0b01111110,
  0b00000111, 0b11110000,
  0b00000111, 0b11111100,
  0b11100111, 0b11111100,
  0b11111111, 0b11100000,
  0b01111111, 0b11110000,
  0b00111111, 0b11111000,
  0b00011111, 0b11111100,
  0b00001111, 0b11101100,
  0b00000111, 0b11001000,
  0b00000110, 0b00011000,
  0b00000110, 0b00011000,
  0b00000110, 0b00000000
};

const unsigned char PROGMEM dinoRunFrameB[] = {
  0b00000111, 0b11111100,
  0b00000111, 0b11111110,
  0b00000110, 0b01111110,
  0b00000110, 0b01111110,
  0b00000111, 0b11110000,
  0b00000111, 0b11111100,
  0b11100111, 0b11111100,
  0b11111111, 0b11100000,
  0b01111111, 0b11110000,
  0b00111111, 0b11111000,
  0b00011111, 0b11111100,
  0b00001111, 0b11101100,
  0b00000111, 0b11011000,
  0b00000110, 0b01100000,
  0b00000000, 0b01111000,
  0b00000000, 0b01100000
};

const unsigned char PROGMEM dinoJumpFrame[] = {
  0b00000111, 0b11111100,
  0b00000111, 0b11111110,
  0b00000110, 0b01111110,
  0b00000110, 0b01111110,
  0b00000111, 0b11110000,
  0b00000111, 0b11111100,
  0b11100111, 0b11111100,
  0b11111111, 0b11100000,
  0b01111111, 0b11110000,
  0b00111111, 0b11111000,
  0b00011111, 0b11111100,
  0b00001111, 0b11101100,
  0b00000111, 0b11001000,
  0b00000110, 0b00110000,
  0b00000000, 0b01100000,
  0b00000000, 0b11000000
};

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
  dinoY = GROUND_Y - DINO_H;
  dinoVelY = 0;
  isJumping = false;

  obsX = SCREEN_WIDTH + random(8, 30);
  obsY = GROUND_Y - OBSTACLE_H;
  score = 0;
  gameSpeed = 3;
  gameOver = false;
  groundOffset = 0;
  dustX = -10;
  dustLife = 0;
  blinkEye = false;
  lastFrameTime = millis();
}

void drawDino(int x, int y) {
  const bool runningFrame = !isJumping && ((millis() / 110) % 2 == 0);
  const unsigned char *frame = dinoJumpFrame;

  if (!isJumping) {
    frame = runningFrame ? dinoRunFrameA : dinoRunFrameB;
  }

  display.drawBitmap(x, y, frame, DINO_W, DINO_H, SSD1306_WHITE);

  if (blinkEye) {
    display.drawPixel(x + 9, y + 2, SSD1306_WHITE);
  }
}

void drawObstacle(int x, int y) {
  display.fillRoundRect(x + 2, y + 2, 5, 10, 1, SSD1306_WHITE);
  display.drawLine(x + 4, y, x + 4, y + 12, SSD1306_WHITE);
  display.drawLine(x + 2, y + 4, x, y + 2, SSD1306_WHITE);
  display.drawLine(x + 6, y + 5, x + 8, y + 3, SSD1306_WHITE);
  display.drawLine(x + 1, y + 7, x, y + 8, SSD1306_WHITE);
  display.drawLine(x + 7, y + 8, x + 9, y + 9, SSD1306_WHITE);
}

bool checkCollision() {
  const int dLeft = DINO_X + 2;
  const int dRight = DINO_X + DINO_W - 2;
  const int dTop = (int)dinoY + 1;
  const int dBottom = (int)dinoY + DINO_H;

  const int oLeft = obsX;
  const int oRight = obsX + OBSTACLE_W;
  const int oTop = obsY;
  const int oBottom = obsY + OBSTACLE_H;

  return dRight > oLeft && dLeft < oRight && dBottom > oTop && dTop < oBottom;
}

void updateDino() {
  if (buttonPressed() && !isJumping && !gameOver) {
    dinoVelY = jumpForce;
    isJumping = true;
    dustX = DINO_X - 2;
    dustLife = 4;
  }

  dinoY += dinoVelY;
  dinoVelY += gravity;

  if (dinoY >= GROUND_Y - DINO_H) {
    dinoY = GROUND_Y - DINO_H;
    dinoVelY = 0;
    isJumping = false;
  }

  blinkEye = ((millis() / 900) % 7) == 0;
}

void updateObstacle() {
  obsX -= gameSpeed;

  if (obsX + OBSTACLE_W < 0) {
    obsX = SCREEN_WIDTH + random(14, 40);
    score++;

    if (score % 4 == 0 && gameSpeed < 8) {
      gameSpeed++;
    }
  }
}

void updateEffects() {
  groundOffset = (groundOffset + gameSpeed) % 16;

  if (dustLife > 0) {
    dustX -= 1;
    dustLife--;
  }
}

void drawGround() {
  display.drawLine(0, GROUND_Y, SCREEN_WIDTH - 1, GROUND_Y, SSD1306_WHITE);

  for (int x = -groundOffset; x < SCREEN_WIDTH; x += 16) {
    display.drawLine(x, GROUND_Y + 2, x + 5, GROUND_Y + 2, SSD1306_WHITE);
    display.drawPixel(x + 9, GROUND_Y + 3, SSD1306_WHITE);
    display.drawPixel(x + 12, GROUND_Y + 1, SSD1306_WHITE);
  }

  if (dustLife > 0) {
    display.drawPixel(dustX, GROUND_Y - 1, SSD1306_WHITE);
    display.drawPixel(dustX + 2, GROUND_Y - 2, SSD1306_WHITE);
    display.drawPixel(dustX + 4, GROUND_Y - 1, SSD1306_WHITE);
  }
}

void drawScore() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(82, 0);
  display.print("P:");
  display.print(score);
  display.setCursor(0, 0);
  display.print("V:");
  display.print(gameSpeed);
}

void drawGameOver() {
  display.fillRect(18, 17, 92, 24, SSD1306_BLACK);
  display.drawRoundRect(18, 17, 92, 24, 3, SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(38, 22);
  display.print("GAME OVER");
  display.setCursor(27, 32);
  display.print("Pulsa para seguir");
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;) {
    }
  }

  randomSeed(analogRead(A0));
  display.clearDisplay();
  display.display();
  resetGame();
}

void loop() {
  const unsigned long now = millis();
  if (now - lastFrameTime < frameDelayMs) {
    return;
  }
  lastFrameTime = now;

  display.clearDisplay();

  if (!gameOver) {
    updateDino();
    updateObstacle();
    updateEffects();

    if (checkCollision()) {
      gameOver = true;
    }
  } else if (buttonPressed()) {
    resetGame();
  }

  drawGround();
  drawDino(DINO_X, (int)dinoY);
  drawObstacle(obsX, obsY);
  drawScore();

  if (gameOver) {
    drawGameOver();
  }

  display.display();
}
