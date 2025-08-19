#pragma once
#include <Arduino.h>
#include <Appl.h>
#include <AppConfig.h>
#include <DHT.h>

struct DewStatus {
    float TempRaw = NAN;
    float HumidityRaw = NAN;
    float TempC = NAN;
    float Humidity = NAN;
    float DewPoint = NAN;
    int           SensorLocation = -1;
    unsigned long LastCallMillis = 0;

};

struct DewConfig {
    bool bIsSensor = true;
    float adjustTempC = 0.0;        // Temperature adjustment
    float adjustHumidity = 0.0;     // Humidity adjustment
    /** only needed if open weather map is used **/
    // String strWeatherURL = "https://api.openweathermap.org/data/3.0/onecall";
    String strWeatherURL = "https://api.openweathermap.org/data/2.5/weather";
    String strWeatherAppID = "36cfc67cc00883f082b22e216d6bfb6b";
    String strWeatherLongitude = "11.553641";
    String strWeatherLatitude  = "48.251690";
    String strWeatherExclude;
    unsigned long nWeatherCallTimeout = 1000 * 60 * 2; // Call every 2 minutes to avoid the 1000 calls per day limit.

};

class CDewSensor { //  : IConfigHandler, IStatusHandler {
    protected:
        DewConfig Config;
        DewStatus Status;
        DHT *pSensor = nullptr;
        JsonDocument OpenWeatherData;

    public:
        CDewSensor();
        ~CDewSensor();
        CDewSensor(int nPort, int nSensorType, int nLocation);

        void setup(int nPort, int nSensorType, int nLocation);
        void dispatch();
        float setTemperature(float fRawTemp);
        float setHumidity(float fRawHumi);

    protected:
        void updateFromWeatherMapData();
        float getRawTemperature(bool bAsFarenheit = false);
        float getRawHumidity();
        float calculateDewPoint(float fTemp, float fHum);
};

