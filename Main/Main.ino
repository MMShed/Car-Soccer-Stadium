// Author: Maheer Morshed, Ishwarejan Balaratnam
// Date: January 19, 2023
// Description: Program for the main functionalities of the stadium field. This program enables the ultrasonic sensor at
// each net to determine when a goal has been scored, and to update the scores of the match on the lcd.

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Screen to display score


// Ultrasonic Sensor Information
const int US1Trig = 4;
const int US1Echo = 5;

const int US2Trig = 6;
const int US2Echo = 7;

// Goal Detection Necessities
long Dist1;
long Dist2;
bool scored = false;

// Tracking Scores
int TeamA = 0;
int TeamB = 0;

// Signals to communicate with other arduinos
char cooldown = 'D';
char msg;
bool pause = false;

bool GoalAIn = false;
bool GoalBIn = false;

// Author of this function is Electronic Curiosities
{
long readUltrasonicDistance(int triggerPin, int echoPin) 

  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);

  // Sets the trigger pin to HIGH state for 10 microseconds
 
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
 
// Reads the echo pin, and returns the sound wave travel time in microseconds
  return pulseIn(echoPin, HIGH);
}

void DisplayScore ()
{
  lcd.clear ();
  lcd.setCursor (5,0);
  lcd.print ("Score:");
  lcd.setCursor (7,1);
  lcd.print (TeamA);
  lcd.print ("-");
  lcd.print (TeamB);
  delay (150);
}

void setup() {
  // Initiates Serial Communication
  Serial.begin (9600);

  // Boots up LCD
  lcd.begin();
  lcd.backlight();
  DisplayScore ();
}

void SendSignal (char sig)
{
  Serial.write (sig);
  delayMicroseconds (20);
}

void ActivateCooldown ()
{
  cooldown = 'A';
  Serial.write (cooldown);
  for (int i = 5; i >= 1; i--)
  {
    delay (1000);
  }
  cooldown = 'D';
  Serial.write (cooldown);
}

void loop() {
  if (not GoalAIn || not GoalBIn)
  {
    // Calculation formula code is also by Electronic Curiosities
    Dist1 = 0.01723 * readUltrasonicDistance(US1Trig, US1Echo); // Give distance between sensor and object in cm
    Dist2 = 0.01723 * readUltrasonicDistance(US2Trig, US2Echo);

    // If a goal has been scored, the distance between an object (the ball) and sensor should be 7 cm or less
    // However, from experimentation, the sensor reads values over 1200 cm when something directly touches it,
    // hence the greater than 1200 cm argument
    GoalAIn = ((Dist1 > 1200 || Dist1 <= 7) && Dist1 != 0);
    GoalBIn = ((Dist2 > 1200 || Dist2 <= 7) && Dist1 != 0);
  }
  
  // When Team A scores
  while (GoalAIn)
  {
    TeamA += 1;
    SendSignal('X'); // Signals the other arduino that a goal from Team A has been scored
    while (GoalAIn)
    {
      Dist1 = 0.01723 * readUltrasonicDistance(US1Trig, US1Echo);
      GoalAIn = ((Dist1 > 1200 || Dist1 <= 7) && Dist1 != 0);    
      delayMicroseconds (20);  
    }
    scored = true;
  }

  //When Team B scores
  while (GoalBIn) 
  {
    TeamB += 1;
    SendSignal('Y'); // Signals the other arduino that Team B scored a goal
    while (GoalBIn)
    {
      Dist2 = 0.01723 * readUltrasonicDistance(US2Trig, US2Echo);
      GoalBIn = ((Dist2 > 1200 || Dist2 <= 7) && Dist2 != 0);
      delayMicroseconds (20);
    }
    scored = true;
  }
  
  if (scored) // Triggers 5-second cooldown, giving players enough time to remove the ball from a net
  {
    ActivateCooldown ();
    DisplayScore(); // Displays the updated score
    scored = false;
  }

  // Receives signal from other arduino when match is over 
  if (Serial.available () > 0)
  {
    msg = Serial.read ();
    if (msg == 'G')
    {
      while (true) {}  // Prevents incremention of goals and keeps the LCD displaying the final scores
    }
  }

}
