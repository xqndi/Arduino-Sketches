#include "pitches.h"

const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output

int pitch_level = 0; //base pitch offset
bool input_switch = false;

void setup() 
{
  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);
  Serial.begin(9600);
}

void loop() 
{
  int x = analogRead(X_pin);
  int y = analogRead(Y_pin);

  //if button is pressed - shift general pitch level of program
  if ((digitalRead(SW_pin) == LOW) && (!input_switch))
  {
    if (pitch_level < 150)
    {
      pitch_level += 50;
    }
    else
    {
      pitch_level = 0;
    }
    input_switch = true;
  }
  if (digitalRead(SW_pin) == HIGH)
  {
    input_switch = false;
  }
 
  int norm_pitch = normalizeInput(x+y);
  //output normalized pitch
  tone(8, norm_pitch, 1);
}
  
int normalizeInput(int input)
{
  int out = (input / 4);
  out += (600 + pitch_level);

  return out;
}

// only used for debugging - doesn't really work while outputting tone 
void printData(int x, int y, int norm_value)
{
  Serial.print("-----------\n");
  Serial.print(x);
  Serial.print("\n");
  Serial.print(y);
  Serial.print("\n");
  Serial.print(norm_value);
  Serial.print("\n");
  
  delay(200);
}
