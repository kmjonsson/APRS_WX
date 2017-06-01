
#include "Weather.h"
#include "GPSHandler.h"

void setup() {
  Serial.begin(38400);
  Serial.println("WX");

  GPSSetup();  
  weatherSetup();
}

void loop() {
  // Handle weather interrupt data
  weatherLoop();
  GPSLoop();
}



