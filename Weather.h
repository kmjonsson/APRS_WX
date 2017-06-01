#ifndef _WEATHER_H_
#define _WEATHER_H_

#define WEATHER_PIN 14

struct weatherData_s {
    // from WEATHER_* defines
    byte modified;
    // Temperature
    int temp;
    byte humidity;

    // Wind
    int windTemp;           // / 10.0
    int windChillTemp;  // / 10.0
    int windSpeed;      // / 10.0
    int windGustSpeed;  // / 10.0
    byte windDirection; // * 22.5

    // Rain
    uint16_t rain;
};

typedef struct weatherData_s weatherData_t;

char *weatherString(char *str);
void weatherSetup();
void weatherLoop();
int weatherReady();

#endif
