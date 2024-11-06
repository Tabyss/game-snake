#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

const int buttonLeft = 2;
const int buttonRight = 3;
const int buzzerPin = 7;

enum Direction { UP,
                 RIGHT,
                 DOWN,
                 LEFT };
Direction dir = RIGHT;

struct Point {
  int x;
  int y;
};

Point snake[20];
int snakeLength = 3;

int headX = 10;
int headY = 2;

Point food;


byte headUp[8] = {
  B00000,
  B00100,
  B00100,
  B01110,
  B01110,
  B01110,
  B01110,
  B01110
};

byte headRight[8] = {
  B00000,
  B00000,
  B11000,
  B11110,
  B11110,
  B11000,
  B00000,
  B00000
};

byte headDown[8] = {
  B01110,
  B01110,
  B01110,
  B01110,
  B01110,
  B00100,
  B00100,
  B00000
};

byte headLeft[8] = {
  B00000,
  B00000,
  B00011,
  B01111,
  B01111,
  B00011,
  B00000,
  B00000
};


byte foodChar[8] = {
  B00000,
  B00010,
  B00100,
  B01110,
  B11111,
  B11111,
  B01110,
  B00000
};


byte bodyVer[8] = {
  B00000,
  B00000,
  B11111,
  B11111,
  B11111,
  B11111,
  B00000,
  B00000
};

byte bodyHor[8] = {
  B01110,
  B01110,
  B01110,
  B01110,
  B01110,
  B01110,
  B01110,
  B01110
};

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(buttonLeft, INPUT_PULLUP);
  pinMode(buttonRight, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  lcd.createChar(0, foodChar);
  lcd.createChar(1, headUp);
  lcd.createChar(2, headRight);
  lcd.createChar(3, headDown);
  lcd.createChar(4, headLeft);
  lcd.createChar(5, bodyVer);
  lcd.createChar(6, bodyHor);

  initializeSnake();
  generateFood();
}

void initializeSnake() {
  snakeLength = 3;
  headX = 10;
  headY = 2;
  for (int i = 0; i < snakeLength; i++) {
    snake[i] = { headX - i, headY };
  }
}

void loop() {
  checkButtons();
  moveSnake();

  if (checkCollision()) {
    snakeLength = snakeLength + 1;  
    generateFood();                
  }

  if (checkSelfCollision()) {  
    gameOver();                
  }

  updateLCD();
  delay(500);  
}

void checkButtons() {
  if (digitalRead(buttonLeft) == LOW) {
    tone(buzzerPin, 1000, 100);                   
    dir = static_cast<Direction>((dir + 3) % 4);  
    delay(200);                                   
  }
  if (digitalRead(buttonRight) == LOW) {
    tone(buzzerPin, 1000, 100);                   
    dir = static_cast<Direction>((dir + 1) % 4);  
    delay(200);                               
  }
}

void moveSnake() {
  for (int i = snakeLength - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }

  switch (dir) {
    case UP: headY--; break;
    case RIGHT: headX++; break;
    case DOWN: headY++; break;
    case LEFT: headX--; break;
  }

  headX = (headX + 20) % 20;
  headY = (headY + 4) % 4;

  snake[0] = { headX, headY }; 
}

bool checkCollision() {
  return (headX == food.x && headY == food.y);
}

void generateFood() {
  food.x = random(0, 19);  
  food.y = random(0, 3);  

  for (int i = 0; i < snakeLength; i++) {
    if (food.x == snake[i].x && food.y == snake[i].y) {
      generateFood();  
      return;
    }
  }
}

bool checkSelfCollision() {
  for (int i = 1; i < snakeLength; i++) {
    if (headX == snake[i].x && headY == snake[i].y) {
      return true;  
    }
  }
  return false;
}

void gameOver() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over!");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(snakeLength - 3); 

  tone(buzzerPin, 500, 1000); 
  waitForRestart();         
}

void waitForRestart() {
  while (true) {
    lcd.setCursor(2, 3);
    lcd.print("Press to Restart");
    delay(500);

    lcd.setCursor(2, 3);
    lcd.print("                ");
    delay(300);
    if (digitalRead(buttonLeft) == LOW || digitalRead(buttonRight) == LOW) {
      delay(200);
      initializeSnake();
      generateFood();
      dir = RIGHT;
      updateLCD();
      break;
    }
  }
}

void updateLCD() {
  lcd.clear();
  drawSnake();
  drawFood();
}

void drawSnake() {
  switch (dir) {
    case UP:
      lcd.setCursor(snake[0].x, snake[0].y);
      lcd.write(byte(1));
      break;
    case RIGHT:
      lcd.setCursor(snake[0].x, snake[0].y);
      lcd.write(byte(2));
      break;
    case DOWN:
      lcd.setCursor(snake[0].x, snake[0].y);
      lcd.write(byte(3));
      break;
    case LEFT:
      lcd.setCursor(snake[0].x, snake[0].y);
      lcd.write(byte(4));
      break;
  }

  for (int i = 1; i < snakeLength; i++) {
    lcd.setCursor(snake[i].x, snake[i].y);
    if (snake[i].x == snake[i - 1].x) {
      lcd.write(byte(6)); 
    } else {
      lcd.write(byte(5)); 
    }
  }
}

void drawFood() {
  lcd.setCursor(food.x, food.y);
  lcd.write(byte(0)); 
}