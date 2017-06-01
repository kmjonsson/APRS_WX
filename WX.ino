/*
 * Code based on code from https://github.com/rvnash/aprs_teensy31
 */

#include "Weather.h"
#include "GPSHandler.h"
#include "aprs_handler.h"

void setup() {
  Serial.begin(38400);
  Serial.println("WX");

  GPS *gps = GPSSetup();  
  weatherSetup();
  aprsSetup(gps);
}

void loop() {
  // Handle weather interrupt data
  weatherLoop();
  GPSLoop();
  if(GPSReady() && weatherReady()) {
    aprsLoop("WX");
  }
}



