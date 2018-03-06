#include <SensorReceiver.h>

#include "Weather.h"

//Weather
#define WEATHER_TEMP 1
#define WEATHER_WIND 2
#define WEATHER_RAIN 4

weatherData_t weatherData;
weatherData_t safeWeather;

volatile int wPosw=0, wPosr=0;
int weatherReadyState=0;

// Weather loop (copy data from interrupt to "main" memory)
void weatherLoop() {
  if(wPosw != wPosr) {
    weatherReadyState = 1;
    weatherData_t w;
    memcpy(&safeWeather,&weatherData,sizeof(safeWeather));
    memcpy(&w,&weatherData,sizeof(safeWeather));
    wPosr = (wPosr+1) % 2;

/*    // Print temperature. Note: temp is 10x the actual temperature!
    Serial.print("Temperature: ");
    Serial.print(w.temp / 10.0); // units

    // Print humidity
    Serial.print(" deg, Humidity: ");
    Serial.print(w.humidity);
    Serial.println("% REL");

    // Wind
    Serial.print("temp=");
    Serial.print(w.windTemp / 10.0);
    Serial.print(" wind_chill_temp=");
    Serial.print(w.windChillTemp / 10.0);
    Serial.print(" wind_speed=");
    Serial.print(w.windSpeed / 10.0);
    Serial.print(" mph (");
    Serial.print(w.windSpeed * 0.044704);
    Serial.print(" m/s)");
    Serial.print(" wind_gust_speed=");
    Serial.print(w.windGustSpeed / 10.0);
    Serial.print(" mph (");
    Serial.print(w.windGustSpeed * 0.044704);
    Serial.print(" m/s)");
    Serial.print(" wind_direction: ");
    Serial.println(w.windDirection * 22.5);

    // Rain
    Serial.print("Rain: ");
    Serial.print(w.rain * 0.7);
    Serial.println(" mm");
  */  
    if(safeWeather.modified & WEATHER_TEMP) {
      //weatherTime.temp = last_time;
    }

    // Wind
    if(safeWeather.modified & WEATHER_WIND) {
      //weatherTime.wind = last_time;
    }
    
    // Rain
    if(safeWeather.modified & WEATHER_RAIN) {
      //weatherTime.rain = last_time;
    }
  }
}

//char str[64];
void decodeWeather(byte *data) {
  //Serial.print("decodeWeather");
  // Got space for more data?
  if(wPosr != wPosw) { return; }

  // is data a ThermoHygro-device?
  if ((data[3] & 0x1f) == 0x1e) {
    byte channel, randomId;
    weatherData_t *w = &weatherData;
    w->modified = WEATHER_TEMP;

    // Decode the data
    SensorReceiver::decodeThermoHygro(data, channel, randomId, w->temp, w->humidity);

    // Temperature channel
//    Serial.print(", Channel: ");
//    Serial.println(channel, DEC);   

    // Release
    wPosw = (wPosw+1) % 2;
  } else if((data[3] & 0x1f) == 0x0C) {
    weatherData_t *w = &weatherData;
    w->modified = WEATHER_WIND;
    
    int temp = 100 * (data[5] & 0x0f) + 10 * (data[4] >> 4) + (data[4] & 0x0f);
    // temp is negative?
    if (!(data[5] & 0x80)) {
        temp = -temp;
    }
    w->windTemp = temp; // Temp * 10 in degC
    
    temp = 100 * (data[7] & 0x0f) + 10 * (data[6] >> 4) + (data[6] & 0x0f);
    // temp is negative?
    if (!(data[7] & 0x80)) {
        temp = -temp;
    }
    w->windChillTemp = temp; // Temp * 10 in degC
    
    w->windSpeed = (data[9] & 0xF) * 100 + (data[8] >> 4) * 10 + (data[8] & 0xF);
    w->windGustSpeed = (data[10] & 0xF) * 10 + (data[10] >> 4) * 100 + (data[9] >> 4);
    
    w->windDirection = data[11] >> 4;
    w->windDirection ^= (w->windDirection & 8) >> 1;
    w->windDirection ^= (w->windDirection & 4) >> 1;
    w->windDirection ^= (w->windDirection & 2) >> 1;
    w->windDirection = -w->windDirection & 0xF;
    
    // Release
    wPosr = (wPosr+1) % 2;
  } else if(data[2] == 0xCC) {
    weatherData_t *w = &weatherData;
    w->modified = WEATHER_RAIN;
    
    w->rain = ((uint16_t)data[5] << 8) + data[4];
    
    // Release
    wPosr = (wPosr+1) % 2;
  } /*else {
    char str[20];
    //sprintf(str,"%02X",(data[3] & 0x1F));
    //Serial.print("Unknown: ");
    for(int i=0;i<14;i++) {
      sprintf(str,"%02X ",data[i]);
      Serial.print(str);
    }
    Serial.println(".");
    sprintf(str,"%02X ",(data[3] & 0x1F));
    Serial.println(str);    
  }*/
  //Serial.println("decodeWeather done");
}

// Weather setup
void weatherSetup() {
  memset(&weatherData,0,sizeof(weatherData));
  memset(&safeWeather,0,sizeof(safeWeather));
  SensorReceiver::init(digitalPinToInterrupt(WEATHER_PIN) , decodeWeather);
}

volatile uint16_t rain_since_middnight=0;

void resetRain() {
  rain_since_middnight = safeWeather.rain;
}

const char *weatherString() {
  static char wxStr[100];
  static int c=0;
  sprintf(wxStr,"%03d/%03dg%03dt%03dh%02dP%03d#%04d.DIY,%d,%d,%d,%d",(int)(safeWeather.windDirection * 22.5),
                                                          (int)(safeWeather.windSpeed / 10.0),
                                                          (int)(safeWeather.windGustSpeed / 10.0),
                                                          (int)((safeWeather.temp*1.8/10.0)+32),
                                                          safeWeather.humidity,
                                                          (int)((safeWeather.rain-rain_since_middnight) * (0.7 * 100 / 25.4)), // 0.7 * 100 / 25.4
                                                         safeWeather.rain%10000,
                                                         safeWeather.temp,safeWeather.windTemp,safeWeather.windChillTemp,c++/1200);
  return wxStr;
}

int weatherReady() {
  return weatherReadyState;
}



