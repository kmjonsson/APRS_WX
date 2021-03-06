/*
 * Code based on code from https://github.com/rvnash/aprs_teensy31
 */

#include "Weather.h"
#include "GPSHandler.h"
#include "aprs_handler.h"
#include "rxtx.h"

#define GPS_EN 2
#define WATCHDOG_PIN 23

GPS *wx_gps;

void wdt_ext_reset() {
  digitalWrite(WATCHDOG_PIN, !digitalRead(WATCHDOG_PIN));
}

void setup() {
  Serial.begin(38400);
  delay(5000);
  Serial.println("WX");

  pinMode(WATCHDOG_PIN, OUTPUT);
  
  // GPS
  pinMode(GPS_EN, OUTPUT);
  digitalWrite(GPS_EN, LOW);
  delay(1000);
  digitalWrite(GPS_EN, HIGH);

  Serial.println("WX - GPS Setup");
  wx_gps = GPSSetup();
  Serial.println("WX - WX Setup");
  weatherSetup();
  Serial.println("WX - ARPS Setup");
  aprsSetup(wx_gps);
  Serial.println("WX - RXTX Setup");
  rxtxSetup();
}

int i=0;
void loop() {
  static int ready = 0;
  static int lastday = -1;

  wdt_ext_reset();
  
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
  } else {
    if(!GPSReady()) {
      Serial.println("GPS is not ready");
    }
    if(!weatherReady()) {
      Serial.println("WX is not ready");
    } else {
      const char *wxStr = weatherString();
      Serial.println(wxStr);
    }
  }
  delay(250);
}



