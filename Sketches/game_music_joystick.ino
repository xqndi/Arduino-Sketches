#include <LiquidCrystal.h>
#include "LedControl.h"
#include "pitches.h"

const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output

int x_out = 0;
int y_out = 0;

int x_pos = 0;
int y_pos = 0;

int point_x_pos = -1;
int point_y_pos = -1;

unsigned long timer = -1;
unsigned long time_game_over = 5000;
unsigned long score = 0;

LiquidCrystal lcd(7, 4, 9, 5, 6, 13);
LedControl lc=LedControl(12,10,11,1);

// --------------------------------------------------------------
// --------------------------------------------------------------
void setup() 
{
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);
  
  lc.shutdown(0,false);
  lc.setIntensity(0,0);
  lc.clearDisplay(0);

  Serial.begin(9600);
  tone(8,  NOTE_E5, 100);
  start();
  spawnPoint();
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void start()
{
  lcd.print("Hello, press");
  lcd.setCursor(0,1);
  lcd.print("left to play!");
  while(analogRead(X_pin) > 200);
  
  tone(8,  NOTE_C5, 75);
  delay(100);
  tone(8, NOTE_D5, 50);
  delay(100);

  lcd.setCursor(0,0);
  lcd.print("Catch dot before");
  lcd.setCursor(0,1);
  lcd.print("time is over");
  while(analogRead(X_pin) < 200);
  while(analogRead(X_pin) > 200);

  tone(8,  NOTE_C5, 75);
  delay(100);
  tone(8, NOTE_D5, 50);
  delay(100);

  lcd.setCursor(0,0);
  lcd.print("    GOOOOO!!!   ");
  lcd.setCursor(0,1);
  lcd.print("                ");
  
  timer = millis();
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void loop() 
{
  x_out = 0;
  y_out = 0;
  int x_in = analogRead(X_pin);
  int y_in = analogRead(Y_pin);

  setOutput(x_in, y_in);
  movePlayer();

  if (point_x_pos == x_pos && point_y_pos == y_pos)
  {
    time_game_over -= 250;
    
    lcd.setCursor(0,0);
    lcd.print("  Score: ");
    lcd.print(++score);
    lcd.print("   ");
    
    tone(8,  NOTE_C5, 75);
    delay(100);
    tone(8, NOTE_E5, 50);
    delay(100);
    
    spawnPoint();
    timer = millis();
  }

  if ((millis()  - timer) > time_game_over)
  {
    endGame();
  }
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void spawnPoint()
{
  do
  {
    point_x_pos = random(0,8);
    point_y_pos = random(0,8); 
  } while (point_x_pos == x_pos && point_y_pos == y_pos);

  lc.setLed(0,point_x_pos, point_y_pos,true);  
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void movePlayer()
{
  lc.setLed(0,x_pos,y_pos,false); 
  x_pos += x_out;
  y_pos += y_out;

  if (x_pos > 7)
  {
    x_pos = 7;
  }
  else if (x_pos < 0)
  {
    x_pos = 0;
  }
  if (y_pos > 7)
  {
    y_pos = 7;
  }
  else if (y_pos < 0)
  {
    y_pos = 0;
  }
  lc.setLed(0,x_pos,y_pos,true);  
  delay(150); 
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void setOutput(int x_input, int y_input)
{
  if (x_input < 400)
  {
    y_out = -1;
  }
  else if (x_input > 600)
  {
    y_out = 1;
  }
  if (y_input > 600)
  {
    x_out = 1;
  }
  else if (y_input < 400)
  {
    x_out = -1;
  }
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void endGame()
{ 
  lcd.setCursor(0,0);
  lcd.print(" /YOUR SCORE: ");
  lcd.print(score);
  lcd.setCursor(0,1);
  lcd.print(" -------------/");
  
  while(true)
  {
    for (int i = 0; i < 3; i++)
    {
      tone(8, NOTE_DS5, 50); 
      delay(25);
      tone(8, NOTE_E5, 50); 
      delay(725);
      tone(8, NOTE_E5, 50); 
      delay(250);
      tone(8, NOTE_F5, 50); 
      delay(500);
      tone(8, NOTE_C5, 50); 
      delay(250);
      tone(8, NOTE_A4, 50); 
      delay(250);
    }
    tone(8, NOTE_DS5, 50); 
    delay(25);
    tone(8, NOTE_E5, 50); 
    delay(725);
    tone(8, NOTE_E5, 50); 
    delay(250);
    tone(8, NOTE_F5, 50); 
    delay(250);
    tone(8, NOTE_E5, 50); 
    delay(250);
    tone(8, NOTE_C5, 50); 
    delay(250);
    tone(8, NOTE_A4, 50); 
    delay(250);
  }
}
