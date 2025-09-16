// Author: Maheer Morshed, Ishwarejan Balaratnam
// Date: January 19, 2023
// Description: Program to set up a 5-minute timer for each match. When the match is over, this program
// will send a signal to the score-tracking arduino to stop counting scores. Also, this program will
// tell the time-tracking arduino to display the winner after each match.

#include <LiquidCrystal_I2C.h>

// Establishes LCD for timing a match
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Stores signal
char pause;

// Time-tracking variables
long countdown_seconds = 0;
long countdown_minute = 5;

// Team scores
int TeamA = 0;
int TeamB = 0;

// Pauses timer for 5 seconds, and displays the 5 second pause countdown 
void PauseTimer ()
{
  lcd.clear ();
  lcd.setCursor (2,0);
  lcd.print ("Resuming in:");
  for (int i = 5; i >= 1; i--)
  {
    lcd.setCursor (7,1);
    lcd.print (i);
    delay (1000);
  }
  lcd.clear ();
  SetTimerHeader ();
  lcd.setCursor (6,1);
}


void SetTimerHeader ()
{
  lcd.setCursor(5, 0);
  lcd.print("Timer:");
}

void setup()
{
  Serial.begin (9600);
  lcd.begin(); // Boots up LCD
  lcd.backlight();
  SetTimerHeader ();
  lcd.setCursor (6,1);
}

void loop()
{

  if (Serial.available () > 0) // Receives signal to trigger the 5-second pause
  {
    pause = Serial.read ();
    if (pause == 'A') // Pauses the timer when a signal that a goal has been scored is received 
    {
      PauseTimer ();
    }
    if (pause == 'X') // Receives signal that Team A scored a goal
    {
      TeamA += 1;
    }
    if (pause == 'Y') // Receives signal that Team B scored a goal
    {
      TeamB += 1;  
    }
  }
 
  lcd.print (countdown_minute);
  lcd.print (":");
  
  if (countdown_seconds < 10)
  {
    lcd.print (0);
  }
  
  lcd.print (countdown_seconds);
 
  delay (1050);
  lcd.setCursor (6,1);

  if (countdown_seconds == 0)

    if (countdown_minute == 0) // Stops the timer when 5 minutes have passed
    {
      while (true)
      {
        lcd.clear ();
        lcd.print ("Game over");
        delay (450);
        lcd.clear ();
        Serial.write ('G'); // Signals to the score-tracking arduino that the match is over

        // Compares scores to determine a winner / tie
        if (TeamA > TeamB)
        {
          lcd.print ("Team A has won!");  
        }
        else if (TeamB > TeamA)
        {
          lcd.print ("Team B has won!");  
        }
        else
        {
          lcd.print ("Its a tie!");  
        }
        while (true) {} // Do nothing to make sure the message on both lcds doesn't change
      }  
    }
    else
    {
      countdown_minute -= 1; 
      countdown_seconds = 59;  
    }
  else
  {
    countdown_seconds -= 1;
  }
 
}
