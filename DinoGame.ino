#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define JUMP_BUTTON 0   // Pin for jump button

// Dino position and movement
int dinoX = 10;
int dinoY = 48;      // Ground level
int velocity = 0;
int gravity = 1;
bool jumping = false;

// Obstacle
int obstacleX = SCREEN_WIDTH;
int speed = 2;
int score = 0;
int highScore = 0;

// Preferences for storing high score
Preferences prefs;

// Static dino bitmap (16x16)
const unsigned char dinoBitmap[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x3f, 0xe0, 
  0x3c, 0xfe, 0x3f, 0xfe, 0x3f, 0xfe, 0x3f, 0xfe, 
  0x1f, 0xfc, 0x0f, 0xc0, 0x07, 0xe0, 0x3b, 0xe0, 
  0x05, 0xc0, 0x07, 0xc0, 0x00, 0xc0, 0x00, 0xc0
};
#define DINO_W 16
#define DINO_H 16

// Static obstacle bitmap (16x16)
const unsigned char obstacleBitmap[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80,
  0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80,
  0x09, 0x90, 0x09, 0x90, 0x09, 0xd0, 0x09, 0xf0,
  0x0f, 0x80, 0x03, 0x80, 0x01, 0x80, 0x01, 0x80
};
#define OBSTACLE_W 16
#define OBSTACLE_H 16

void setup() {
  Serial.begin(115200);
  Wire.begin(4, 5);  // SDA=4, SCL=5
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  pinMode(JUMP_BUTTON, INPUT_PULLUP);

  // Initialize preferences and read high score
  prefs.begin("dinoGame", false);
  highScore = prefs.getInt("highScore", 0);
}

void loop() {
  // Handle jump
  if(digitalRead(JUMP_BUTTON) == LOW && !jumping) {
    velocity = -8;
    jumping = true;
  }

  // Apply gravity
  dinoY += velocity;
  velocity += gravity;
  if(dinoY >= 48) { // Ground level
    dinoY = 48;
    velocity = 0;
    jumping = false;
  }

  // Update obstacle
  obstacleX -= speed;
  if(obstacleX < -OBSTACLE_W) {
    obstacleX = SCREEN_WIDTH;
    score++;
    if(speed < 6) speed++; // Gradually increase speed
  }

  // Collision detection
  if(obstacleX < dinoX + DINO_W && obstacleX + (OBSTACLE_W - 2)> dinoX && dinoY + DINO_H > 48) {
    // Update high score if needed
    if(score > highScore) {
      highScore = score;
      prefs.putInt("highScore", highScore);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(20, 20);
    display.print("GAME OVER");
    display.setCursor(20, 35);
    display.print("Score: ");
    display.print(score);
    display.setCursor(20, 50);
    display.print("High: ");
    display.print(highScore);
    display.display();
    delay(2000);

    // Reset game
    obstacleX = SCREEN_WIDTH;
    dinoY = 48;
    velocity = 0;
    jumping = false;
    score = 0;
    speed = 4;
    return;
  }

  // Draw everything
  display.clearDisplay();

  // Draw static dino
  display.drawBitmap(dinoX, dinoY, dinoBitmap, DINO_W, DINO_H, WHITE);

  // Draw obstacle
  display.drawBitmap(obstacleX, 48, obstacleBitmap, OBSTACLE_W, OBSTACLE_H, WHITE);

  // Draw ground
  display.drawLine(0, 64, SCREEN_WIDTH, 64, WHITE);

  // Draw score and high score
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(50, 0);
  display.print("Score:");
  display.print(score);
  display.setCursor(0, 0);
  display.print("High:");
  display.print(highScore);

  display.display();
  delay(30);
}
