/*
 * Code based on code from https://github.com/rvnash/aprs_teensy31
 */

#include "Weather.h"
#include "GPSHandler.h"
#include "aprs_handler.h"
#include "rxtx.h"

#define GPS_EN 2

GPS *wx_gps;

void setup() {
  Serial.begin(38400);
  Serial.println("WX");

  

  // GPS
  pinMode(GPS_EN, OUTPUT);
  digitalWrite(GPS_EN, LOW);
  delay(1000);
  digitalWrite(GPS_EN, HIGH);

  wx_gps = GPSSetup();
  weatherSetup();
  aprsSetup(wx_gps);
  rxtxSetup();
}

int i=0;
void loop() {
  static int ready = 0;
  static int lastday = -1;
  //Serial.println("hello :-)");
  /*
  while(Serial3.available()) {
    int k = Serial3.read();
    if(k == '$') Serial.println("");
    Serial.write(k);
  }
  */
  /*
  i++;
  if(i % 2 == 0) {
    Serial.println("High");
    digitalWrite(15, HIGH);
  } else {
    Serial.println("Low");
    digitalWrite(15, LOW);
  }
  */
  // Handle weather interrupt data
  weatherLoop();
  GPSLoop();
  //Serial.println(GPSReady());
  //Serial.println(weatherReady());
  if(GPSReady() && weatherReady()) {
    if(!ready++) { 
      Serial.println("Ready :-)");
    }
    if(lastday != wx_gps->day) {
      resetRain();
      lastday = wx_gps->day;
    }
    const char *wxStr = weatherString();
    aprsLoop(wxStr);
  }
  
  delay(250);
}



