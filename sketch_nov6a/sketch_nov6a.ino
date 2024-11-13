#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);  // LCD address 0x27, size 20x4

const int buttonLeft = 2;   // Pin for left turn button
const int buttonRight = 3;  // Pin for right turn button
const int buzzerPin = 6;    // Pin for buzzer

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


byte headUp[8] = { B00000, B00100, B00100, B01110, B01110, B01110, B01110, B01110 };
byte headRight[8] = { B00000, B00000, B11000, B11110, B11110, B11000, B00000, B00000 };
byte headDown[8] = { B01110, B01110, B01110, B01110, B01110, B00100, B00100, B00000 };
byte headLeft[8] = { B00000, B00000, B00011, B01111, B01111, B00011, B00000, B00000 };
byte foodChar[8] = { B00000, B00010, B00100, B01110, B11111, B11111, B01110, B00000 };
byte bodyVer[8] = { B11000, B11100, B11111, B11111, B11111, B11111, B11100, B11000 };
byte bodyHor[8] = { B01110, B01110, B01110, B01110, B01110, B01110, B01110, B01110 };

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(buttonLeft, INPUT_PULLUP);
  pinMode(buttonRight, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW); 

  lcd.createChar(0, foodChar);   
  lcd.createChar(1, headUp);     
  lcd.createChar(2, headRight);  
  lcd.createChar(3, headDown);   
  lcd.createChar(4, headLeft);   
  lcd.createChar(5, bodyVer);    
  lcd.createChar(6, bodyHor);    

  showWelcomeScreen();
  initializeSnake();
  generateFood();
}

void showWelcomeScreen() {
  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print("Welcome to Snake!");

  lcd.setCursor(2, 2);
  lcd.print("Press any button");

  String groupText = "Kelompok 2  24.21.1582 24.21.1590 23.11.5708 23.11.5740 ";
  
  unsigned long previousMillis = 0;
  int groupTextPos = 0;
  int namesTextPos = 0;
  const unsigned long interval = 300;
  
  while (true) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      lcd.setCursor(0, 0);
      lcd.print(groupText.substring(groupTextPos, groupTextPos + 20));
      groupTextPos = (groupTextPos + 1) % groupText.length();
    }
    
    if (digitalRead(buttonLeft) == LOW || digitalRead(buttonRight) == LOW) {
      delay(200); 
      break;      
    }
  }

  lcd.clear();
}

void initializeSnake() {
  snakeLength = 2; 
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
    snakeLength++;   
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
    dir = static_cast<Direction>((dir + 3) % 4);  // Turn left
    digitalWrite(buzzerPin, HIGH);
    delay(20);
    digitalWrite(buzzerPin, LOW);
  }
  if (digitalRead(buttonRight) == LOW) {
    dir = static_cast<Direction>((dir + 1) % 4);  // Turn right
    digitalWrite(buzzerPin, HIGH);
    delay(20);
    digitalWrite(buzzerPin, LOW);
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
  food.x = random(0, 20);
  food.y = random(0, 4);
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

  digitalWrite(buzzerPin, HIGH);
  delay(250);
  digitalWrite(buzzerPin, LOW);

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
    lcd.write(snake[i].x == snake[i - 1].x ? byte(6) : byte(5));
  }
}

void drawFood() {
  lcd.setCursor(food.x, food.y);
  lcd.write(byte(0));
}