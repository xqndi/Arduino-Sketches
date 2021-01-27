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

int bullet_x_pos = -1;
int bullet_y_pos = -1;

unsigned long timer = 0;
unsigned long score = 0;
unsigned long turn_counter = 0;

unsigned long time_game_over = 5000;
unsigned long delay_time = 150;

bool is_catch = true;
bool is_shooting = false;

bool active_bullet = false;
bool active_enemy = false;

typedef struct _Shotgun_
{
  int cooldown = 25;
  bool is_alive = false;

  int left_x = -5;
  int left_y = -5;
  int mid_x = -5;
  int mid_y = -5;
  int right_x = -5; 
  int right_y = -5;
} Shotgun;

Shotgun shotGun;

LiquidCrystal lcd(7, 4, 9, 5, 6, 13);
LedControl lc = LedControl(12,10,11,1);

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

  startAll();
  
  if (is_catch)
  {
    startCatch();
    spawnPointCatch();
  }
  else
  {
    startShoot();
    spawnPlayerShoot();
  }
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void loop() 
{
  if (is_catch)
  {
    loopCatch();
  }
  else
  {
    loopShoot();
  }
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void startAll()
{
  lcd.print("left for catch");
  lcd.setCursor(0,1);
  lcd.print("right for shoot");
  int input;
  do
  {
  input = analogRead(X_pin);
  } while((input > 200) && (input < 800));

  if (input >= 800)
  {
    is_catch = false;
  }

  playIntervall(NOTE_C5, NOTE_D5);
  lcd.clear();
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
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void initShotgun()
{
  shotGun.cooldown--;
  if ((digitalRead(SW_pin) != LOW) || (shotGun.cooldown > 0))
  {
    return;
  }
  playIntervall(NOTE_E2, NOTE_E4);
  lc.setLed(0,shotGun.left_x, shotGun.left_y,false);
  lc.setLed(0,shotGun.mid_x, shotGun.mid_y,false);
  lc.setLed(0,shotGun.right_x, shotGun.right_y,false);
  
  shotGun = Shotgun();
  shotGun.is_alive = true;
  
  shotGun.left_x = x_pos - 1;
  shotGun.left_y = y_pos - 1;
  shotGun.mid_x = x_pos - 1;
  shotGun.mid_y = y_pos;
  shotGun.right_x = x_pos - 1;
  shotGun.right_y = y_pos + 1;
  
  lc.setLed(0,shotGun.left_x, shotGun.left_y,true);
  lc.setLed(0,shotGun.mid_x, shotGun.mid_y,true);
  lc.setLed(0,shotGun.right_x, shotGun.right_y,true);
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void moveShotgun()
{
  if (!shotGun.is_alive)
  {
    return;
  }
  lc.setLed(0,shotGun.left_x, shotGun.left_y,false);
  lc.setLed(0,shotGun.mid_x, shotGun.mid_y,false);
  lc.setLed(0,shotGun.right_x, shotGun.right_y,false);
  
  shotGun.left_x -= 1;
  shotGun.left_y -= 1;
  shotGun.mid_x -= 1;
  shotGun.right_x -= 1;
  shotGun.right_y += 1;

  lc.setLed(0,shotGun.left_x, shotGun.left_y,true);
  lc.setLed(0,shotGun.mid_x, shotGun.mid_y,true);
  lc.setLed(0,shotGun.right_x, shotGun.right_y,true);
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void startShoot()
{
  lcd.setCursor(0,0);
  lcd.print("Hit dots before");
  lcd.setCursor(0,1);
  lcd.print("dots reach end");
  while(analogRead(X_pin) > 800);
  while(analogRead(X_pin) < 800);

  playIntervall(NOTE_C5, NOTE_D5);
  
  lcd.clear();
  lcd.print("press <left/");
  lcd.setCursor(0,1);
  lcd.print("right> to move");
  while(analogRead(X_pin) > 800);
  while(analogRead(X_pin) < 800);

  playIntervall(NOTE_C5, NOTE_D5);

  lcd.clear();
  lcd.print("press <up>");
  lcd.setCursor(0,1);
  lcd.print("to shoot");
  while(analogRead(X_pin) > 800);
  while(analogRead(X_pin) < 800);

  playIntervall(NOTE_C5, NOTE_D5);

  lcd.clear();
  lcd.print("press <button>");
  lcd.setCursor(0,1);
  lcd.print("for shotgun");
  while(analogRead(X_pin) > 800);
  while(analogRead(X_pin) < 800);

  playIntervall(NOTE_C5, NOTE_D5);

  lcd.clear();
  lcd.print("    GOOOOO!!!   ");
  
  timer = millis();
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void spawnEnemyShoot()
{
  if (active_enemy)
  {
    return;
  }
  active_enemy = true;
  point_x_pos = 0;
  point_y_pos = random(0,8); 

  lc.setLed(0,point_x_pos, point_y_pos,true);
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void moveEnemyShoot()
{
  if (!active_enemy)
  {
    return;
  }
  if ((turn_counter++) % 10)
  {
    return;
  }

  lc.setLed(0,point_x_pos, point_y_pos, false);
  point_x_pos += 1;

  int dist = -5;
  do
  {
    dist = random(-1,2);
  }while((point_y_pos + dist) < 0 || (point_y_pos + dist) > 7);

  point_y_pos += dist;
  lc.setLed(0,point_x_pos, point_y_pos,true);

  if (point_x_pos > 7)
  {
    lc.setLed(0,point_x_pos, point_y_pos, false);
    active_enemy = false;
    endGameCatch();
  }
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void detectCollision()
{
  bool has_collided = false;
  if (!active_enemy)
  {
    return;
  }
  if ((point_x_pos == bullet_x_pos) && (point_y_pos == bullet_y_pos))
  {
    active_bullet = false;
    has_collided = true;
  }
  else if ((shotGun.right_x == point_x_pos) && (shotGun.right_y == point_y_pos))
  {
    has_collided = true;
    shotGun.right_x = -1;
    shotGun.right_y = -1;
  }
  else if ((shotGun.mid_x == point_x_pos) && (shotGun.mid_y == point_y_pos))
  {
    has_collided = true;
    shotGun.mid_x = -1;
    shotGun.mid_y = -1;
  }
  else if ((shotGun.left_x == point_x_pos) && (shotGun.left_y == point_y_pos))
  {
    has_collided = true;
    shotGun.left_x = -1;
    shotGun.left_y = -1;
  }

  if (has_collided)
  {
    lc.setLed(0,point_x_pos, point_y_pos,false);  
    lcd.clear();
    lcd.print("  Score: ");
    lcd.print(++score);
  
    delay_time -= 10;
    playIntervall(NOTE_E3, NOTE_C3);
    active_enemy = false;
  }
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void spawnPlayerShoot()
{
  x_pos = 7;
  y_pos = 4; 
  lc.setLed(0,x_pos, y_pos,true);  
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void loopShoot() 
{
  is_shooting = false;
  
  x_out = 0;
  y_out = 0;
  
  int x_in = analogRead(X_pin);
  int y_in = analogRead(Y_pin);

  spawnEnemyShoot();

  setOutputShoot(x_in, y_in);
  movePlayer();
  shootBullet();
  initShotgun();
  detectCollision();
  
  delay(delay_time);
  
  moveBullet();
  moveShotgun();
  detectCollision();
  moveEnemyShoot();
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void shootBullet()
{
  if (!is_shooting || active_bullet)
  {
    return;
  }
  active_bullet = true;
  bullet_x_pos = x_pos - 1;
  bullet_y_pos = y_pos;
  lc.setLed(0,bullet_x_pos, bullet_y_pos,true);
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void moveBullet()
{
  if (!active_bullet)
  {
    return;
  }

  lc.setLed(0,bullet_x_pos, bullet_y_pos, false);
  bullet_x_pos -= 1;
  
  lc.setLed(0,bullet_x_pos, bullet_y_pos,true);

  if (bullet_x_pos < 0)
  {
    lc.setLed(0,bullet_x_pos, bullet_y_pos, false);
    active_bullet = false;
  }
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void setOutputShoot(int x_input, int y_input)
{
  if (x_input < 400)
  {
    y_out = -1;
  }
  else if (x_input > 600)
  {
    y_out = 1;
  }
  if (y_input < 400)
  {
    is_shooting = true;
    x_out = 0;
  }
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void startCatch()
{
  lcd.setCursor(0,0);
  lcd.print("Catch dot before");
  lcd.setCursor(0,1);
  lcd.print("time is over");
  while(analogRead(X_pin) < 200);
  while(analogRead(X_pin) > 200);

  playIntervall(NOTE_C5, NOTE_D5);

  lcd.clear();
  lcd.print("    GOOOOO!!!   ");
  
  timer = millis();
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void loopCatch() 
{
  x_out = 0;
  y_out = 0;
  int x_in = analogRead(X_pin);
  int y_in = analogRead(Y_pin);

  setOutputCatch(x_in, y_in);
  movePlayer();
  delay(delay_time);

  if (point_x_pos == x_pos && point_y_pos == y_pos)
  {
    time_game_over -= 250;
    
    lcd.clear();
    lcd.print("  Score: ");
    lcd.print(++score);
    
    playIntervall(NOTE_C5, NOTE_E5);
    
    spawnPointCatch();
    timer = millis();
  }

  if ((millis()  - timer) > time_game_over)
  {
    endGameCatch();
  }
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void spawnPointCatch()
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
void setOutputCatch(int x_input, int y_input)
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
void playIntervall(int first_note, int second_note)
{
  tone(8, first_note, 75);
  delay(100);
  tone(8, second_note, 50);
  delay(100);
}

// --------------------------------------------------------------
// --------------------------------------------------------------
void endGameCatch()
{ 
  lcd.clear();
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
