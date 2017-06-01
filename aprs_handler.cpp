// Example program for the APRS code in this repository

/*
 * Copyright (C) 2014 by Richard Nash (KC3ARY)
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <WProgram.h>
// Note: this example uses my GPS library for the Adafruit Ultimate GPS
// Code located here: https://github.com/rvnash/ultimate_gps_teensy3
#include "GPS.h"
#include "aprs.h"

// APRS Information
#define PTT_PIN_1 15 // Push to talk pin
#define PTT_PIN_2 16 // Push to talk pin

// Set your callsign and SSID here. Common values for the SSID are
#define S_CALLSIGN      "SA2BRJ"
#define S_CALLSIGN_ID   1   // 11 is usually for balloons
// Destination callsign: APRS (with SSID=0) is usually okay.
#define D_CALLSIGN      "APRS"
#define D_CALLSIGN_ID   0
// Symbol Table: '/' is primary table '\' is secondary table
#define SYMBOL_TABLE '/' 
// Primary Table Symbols: /O=balloon, /-=House, /v=Blue Van, />=Red Car
// /_=WX
#define SYMBOL_CHAR '_'

struct PathAddress addresses[] = {
  {(char *)D_CALLSIGN, D_CALLSIGN_ID},  // Destination callsign
  {(char *)S_CALLSIGN, S_CALLSIGN_ID},  // Source callsign
  {(char *)NULL, 0}, // Digi1 (first digi in the chain)
  {(char *)NULL, 0}  // Digi2 (second digi in the chain)
};

GPS *agps;

// setup() method runs once, when the sketch starts
void aprsSetup(GPS *g)
{
  agps = g;
  // Set up the APRS module
  aprs_setup(50, // number of preamble flags to send
	     PTT_PIN_1, // Use PTT pin
	     PTT_PIN_2, // Use PTT pin
	     100, // ms to wait after PTT to transmit
	     0, 0 // No VOX ton
	     );
}

// Function to broadcast your location

void broadcastLocation(const char *comment)
{
  // If above 5000 feet switch to a single hop path
  int nAddresses;
  // Below 1500 meters use a much more generous path (assuming a mobile station)
  // Path is "WIDE1-1,WIDE2-2"
  nAddresses = 4;
  addresses[2].callsign = "WIDE1";
  addresses[2].ssid = 1;
  addresses[3].callsign = "WIDE2";
  addresses[3].ssid = 2;

  // For debugging print out the path
  Serial.print("APRS(");
  Serial.print(nAddresses);
  Serial.print("): ");
  for (int i=0; i < nAddresses; i++) {
    Serial.print(addresses[i].callsign);
    Serial.print('-');
    Serial.print(addresses[i].ssid);
    if (i < nAddresses-1)
      Serial.print(',');
  }
  Serial.print(' ');
  Serial.print(SYMBOL_TABLE);
  Serial.print(SYMBOL_CHAR);
  Serial.println();

  // Send the packet
  aprs_send_wx(addresses, nAddresses
	    ,agps->day, agps->hour, agps->minute
	    ,agps->latitude, agps->longitude
	    //,gps.altitude // meters
	    ,14 // speed
	    ,15 // heading
	    ,SYMBOL_TABLE
	    ,SYMBOL_CHAR
	    ,comment);
}

uint32_t timeOfAPRS = 0;
void aprsLoop(const char *wxStr)
{
  if (timeOfAPRS + 300*1000 < millis()) { // every 5min
    broadcastLocation(wxStr);
    timeOfAPRS = millis();
  }
}

