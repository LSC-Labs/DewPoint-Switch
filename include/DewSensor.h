#pragma once

#include <Arduino.h>
#include <Appl.h>
#include <AppConfig.h>
#include <EventHandler.h>
#include <SimpleDelay.h>
#include <DHT.h>

struct DewStatus {
    float TempRawC      = NAN;
    float TempRawF      = NAN;
    float HumidityRaw   = NAN;
    float TempC         = NAN;
    float TempF         = NAN;
    float Humidity      = NAN;
    float DewPoint      = NAN;
    int           SensorLocation = -1;
    unsigned long LastWeatherCallMillis = 0;
    unsigned long LastSensorReadMillis  = 0;

};

struct DewConfig {
    bool bIsPhysicalSensor = true;
    float adjustTempC = 0.0;        // Temperature adjustment in °C
    float adjustHumidity = 0.0;     // Humidity adjustment
    /** only needed if open weather map is used **/
    // String strWeatherURL = "https://api.openweathermap.org/data/3.0/onecall";
    String strWeatherURL = "http://api.openweathermap.org/data/2.5/weather";
    String strWeatherAppID = "1234c67c123483f081234e2112341234";
    String strWeatherLongitude = "11.553641";
    String strWeatherLatitude  = "23.556778";
    String strWeatherExclude;
    unsigned long nWeatherCallTimeout = 1000 * 60 * 2; // Call every 2 minutes to avoid the 1000 calls per day limit.
#ifdef DEBUGINFOS
    unsigned long nSensorReadTimeout = 20000; // Read the sensor in Debug Mode to avoid message flooding   
#else
    unsigned long nSensorReadTimeout = 10000; // Read the sensor every 10 seconds (if physical sensor is used)    
#endif
};

class CDewSensor : public IMsgEventReceiver, public IConfigHandler,public IStatusHandler { //  : IConfigHandler, IStatusHandler {
    protected:
        DewConfig Config;
        CSimpleDelay m_oSensorReadDelay;      
        CSimpleDelay m_oUpdateDelay; // Update delay overall...
        DHT *pSensor = nullptr; 
        bool m_bInternetAvailable = false;
        JsonDocument OpenWeatherData;

    public:
        DewStatus Status;

    public: 
        CDewSensor();
        ~CDewSensor();
        CDewSensor(int nPort, int nSensorType, int nLocation);

        void begin(int nPort, int nSensorType, int nLocation);
        
        // Interface implementations
        void writeStatusTo(JsonObject & oObj) override;
        void readConfigFrom(JsonObject & oCfg) override;
        void writeConfigTo(JsonObject & oCfg, bool bHideCritical) override;
        int  receiveEvent(const void * pSender, int nMsg, const void * pData, int nType) override;

        float getTemperature(bool bAsFarenheit = false);
        float getHumidity();
        float getDewPoint(bool bInFarenheit = false);
        float calculateDewPoint(float fTemp, float fHum, bool bInFarenheit = false);
        bool  isInternetAvailable();
    protected:
       
        float adjustAndStoreTemperatures(float fRawTemp, bool bInFarenheit = false);
        float adjustAndStoreHumidity(float fRawHumi);
        void  updateFromPhysicalSensor();
        void  updateFromOpenWeatherMap();
        
};

