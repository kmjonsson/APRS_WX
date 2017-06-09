#include "GPSHandler.h"

HardwareSerial &gpsSerial = Serial3;
GPS gps(&gpsSerial,true);

GPS *GPSSetup() {
  gps.startSerial(9600);
  delay(1000);
  gps.setSentencesToReceive(OUTPUT_RMC_GGA);
  return &gps;
}

int gps_data_available = 0;

void GPSLoop() {
  if (gps.sentenceAvailable()) {
    //Serial.print("GPS: ");
    //Serial.println(gps.parseSentence());
    gps.parseSentence();
  }

  if (gps.newValuesSinceDataRead()) {
    gps.dataRead();
    Serial.printf("Location: %f, %f altitude %f\n\r",gps.latitude, gps.longitude, gps.altitude);
    Serial.printf("Time: %04d-%02d-%02d, %02d:%02d:%02d\n\r",2000+gps.year, gps.month, gps.day,gps.hour, gps.minute, gps.seconds);
    gps_data_available = 1;
  }
}

int GPSReady() {
	return gps_data_available;
}

