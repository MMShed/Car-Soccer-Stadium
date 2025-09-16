// Author: Maheer Morshed, Ishwarejan Balaratnam
// Date: January 19, 2023
// Description: Program for the power boost component. It stays on until a car goes over it, and then it waits for some time to turn back on
#include <Adafruit_NeoPixel.h>

#define PIN_A 7

#define NUMPIXELS 16

// Establishes the boost component
Adafruit_NeoPixel Boost = Adafruit_NeoPixel(
  NUMPIXELS, PIN_A, NEO_GRB + NEO_KHZ800);


const int CollisionDetector = A0; // Photoresistor to detect a car
const int delayLength = 50;

// Values that the photoresistor readings should fall between to detect a car
const int min_value = 400;
const int max_value = 600;

// Tracks the values the photoresistor reads
int read_value = 0;

void ActivateBoost ()
{
  // Activates each individual light in sequence
  for(int i=0; i<NUMPIXELS;i++) {
   
  Boost.setPixelColor(i,Boost.Color(0,0,20));
  Boost.show();
 
  delay(delayLength);
   
  }  
}


void DeactivateBoost ()
{
  Boost.clear ();
  // Deactivates each light in sequence
  for(int i=0; i<NUMPIXELS;i++) {
   
  Boost.setPixelColor(i,Boost.Color(0,0,0));
  Boost.show();
 
  delay(delayLength);
   
  }  

  // Makes sure that the boost doesn't do anything until a car gets off of it
  while (read_value > min_value && read_value < max_value)
  {
    read_value = analogRead (CollisionDetector);  
  }

  delay (5000);
  ActivateBoost (); // Turns boost back on after 5 seconds
}

void setup() {
  Serial.begin (9600);
  Boost.begin ();
  Boost.show ();
  pinMode (CollisionDetector, INPUT);
  ActivateBoost ();

}

void loop() 
{
  read_value = analogRead (CollisionDetector);

  if (read_value > min_value && read_value < max_value) // When the boost detects a car
  {
    delay (1250); // Stays on a bit longer to allow the car to fully go over the boost
    DeactivateBoost ();
  }
}
