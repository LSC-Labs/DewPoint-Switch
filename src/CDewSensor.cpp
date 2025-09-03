#ifndef DEBUG_LSC_DEW_SENSOR
    #undef DEBUGINFOS
#endif

#include <LSCUtils.h>

// https://www.heise.de/select/make/2022/1/2135511212557842576
// https://www.heise.de/select/make/2022/2/2204711461516715363
// https://www.heise.de/select/make/2022/1/softlinks/x2yt?wt_mc=pred.red.make.make012022.022.softlink.softlink

// See also : https://github.com/MakeMagazinDE/Taupunktluefter
#include <DewSensor.h>
#include <Network.h>
#include <ESP8266HTTPClient.h>
#include <Msgs.h>

#define SENSOR_CFG_IS_SENSOR        F("physical")
#define SENSOR_CFG_ADJUST_TEMP      F("adjust_temp")
#define SENSOR_CFG_ADJUST_HUMIDITY  F("adjust_humidity")
#define SENSOR_CFG_OPENWEATHER_URL  F("web_url")
#define SENSOR_CFG_OPENWEATHER_KEY  F("web_key")
#define SENSOR_CFG_OPENWEATHER_LONG F("web_long")
#define SENSOR_CFG_OPENWEATHER_LAT  F("web_lat")
#define SENSOR_STATUS_TEMPC         F("temp_c")
#define SENSOR_STATUS_TEMPF         F("temp_f")
#define SENSOR_STATUS_HUMIDITY      F("hum")
#define SENSOR_STATUS_TEMPC_RAW     F("temp_c_raw")
#define SENSOR_STATUS_TEMPF_RAW     F("temp_f_raw")
#define SENSOR_STATUS_HUMIDITY_RAW  F("hum_raw")
#define SENSOR_STATUS_DEWPOINT      F("dew_point")
#define SENSOR_STATUS_LAST_UPD_MS   F("last_upd_ms")
#define SENSOR_STATUS_INTERNET      F("internet")







CDewSensor::CDewSensor() {};
CDewSensor::~CDewSensor() { if(pSensor != nullptr) free(pSensor); }

CDewSensor::CDewSensor(int nPin, int nSensorType, int nLocation) {
    DEBUG_FUNC_START();
    setup(nPin,nSensorType,nLocation);

};

void CDewSensor::setup(int nPin, int nSensorType, int nLocation) {
    if(nSensorType == USE_OPEN_WEATHER) {
        Config.bIsPhysicalSensor = false;
        m_oUpdateDelay.start(this->Config.nWeatherCallTimeout);
    } else {
        this->Config.bIsPhysicalSensor = true;
        this->pSensor =  new DHT(nPin,nSensorType);
        this->pSensor->begin();
        m_oUpdateDelay.start(this->Config.nSensorReadTimeout);
    }
    Status.SensorLocation = nLocation;    
};

void CDewSensor::readConfigFrom(JsonObject & oCfg) {
    LSC::setValue(&Config.bIsPhysicalSensor,    oCfg[SENSOR_CFG_IS_SENSOR]);
    LSC::setValue(&Config.adjustHumidity,       oCfg[SENSOR_CFG_ADJUST_HUMIDITY]);
    LSC::setValue(&Config.adjustTempC,          oCfg[SENSOR_CFG_ADJUST_TEMP]);
    LSC::setValue(Config.strWeatherAppID,       oCfg[SENSOR_CFG_OPENWEATHER_KEY]);
    LSC::setValue(Config.strWeatherLongitude,   oCfg[SENSOR_CFG_OPENWEATHER_LONG]);
    LSC::setValue(Config.strWeatherLatitude,    oCfg[SENSOR_CFG_OPENWEATHER_LAT]);
    DEBUG_INFOS(" ## -> Configured as %s sensor", Config.bIsPhysicalSensor ? "physical" : "open weather");
    m_oUpdateDelay.start( Config.bIsPhysicalSensor      ? 
                            Config.nSensorReadTimeout   :
                            Config.nWeatherCallTimeout  );
}

void CDewSensor::writeConfigTo(JsonObject & oCfg, bool bHideCritical) {
    oCfg[SENSOR_CFG_IS_SENSOR]          = Config.bIsPhysicalSensor;
    oCfg[SENSOR_CFG_ADJUST_HUMIDITY]    = Config.adjustHumidity;
    oCfg[SENSOR_CFG_ADJUST_TEMP]        = Config.adjustTempC;
    oCfg[SENSOR_CFG_OPENWEATHER_KEY]    = Config.strWeatherAppID;
    oCfg[SENSOR_CFG_OPENWEATHER_LONG]   = Config.strWeatherLongitude;
    oCfg[SENSOR_CFG_OPENWEATHER_LAT]    = Config.strWeatherLatitude;
}

void CDewSensor::writeStatusTo(JsonObject & oStatus) {
    oStatus[SENSOR_CFG_IS_SENSOR]       = Config.bIsPhysicalSensor;
    oStatus[SENSOR_STATUS_TEMPC]        = Status.TempC;
    oStatus[SENSOR_STATUS_TEMPC_RAW]    = Status.TempRawC;
    oStatus[SENSOR_STATUS_TEMPF]        = Status.TempF;
    oStatus[SENSOR_STATUS_TEMPF_RAW]    = Status.TempRawF;
    oStatus[SENSOR_STATUS_HUMIDITY]     = Status.Humidity;
    oStatus[SENSOR_STATUS_HUMIDITY_RAW] = Status.HumidityRaw;
    oStatus[SENSOR_STATUS_DEWPOINT]     = Status.DewPoint;
    oStatus[SENSOR_STATUS_LAST_UPD_MS]  = Status.LastWeatherCallMillis;
    if(!Config.bIsPhysicalSensor) {
        oStatus[SENSOR_STATUS_INTERNET]     = isInternetAvailable();
    }
}

/// @brief Listen to the messagebus - detect if internet is available or not.
/// @param pSender 
/// @param nMsg 
/// @param pData 
/// @param nType 
/// @return 
int CDewSensor::receiveEvent(const void * pSender, int nMsg, const void * pData, int nType) {
    switch(nMsg) {
        case MSG_WIFI_CONNECTED:    m_bInternetAvailable = (nType == WIFI_STATION_MODE);
            DEBUG_INFOS(" ## -> Setting Internet access to %d",isInternetAvailable());
                                    break;
        case MSG_WIFI_DISABLED :    m_bInternetAvailable = false;
            DEBUG_INFO(" ## -> Disabling Internet access for sensors...");
                                    break;
    }
    return(EVENT_MSG_RESULT_OK);
}

bool CDewSensor::isInternetAvailable() {
    return(this->m_bInternetAvailable);
}

/// @brief Stores the temperature in the Status Object
/// @param fRawData 
/// @param bAsFarenheit if true, fRawData is in Fahrenheit
/// @return The adjusted temperature in Celsius or Fahrenheit
float CDewSensor::adjustAndStoreTemperatures(float fRawData, bool bAsFarenheit) {
    DEBUG_FUNC_START_PARMS("fRawData=%f,bAsFarenheit=%d",fRawData,bAsFarenheit);
    if(!isnan(fRawData)) {
        if(bAsFarenheit) {
            Status.TempRawF =  fRawData;
            Status.TempF = fRawData + LSC::getFarenheitFromCelsius(Config.adjustTempC);
            Status.TempRawC = LSC::getCelsiusFromFarenheit(fRawData);
            Status.TempC = LSC::getCelsiusFromFarenheit(Status.TempRawF);
        } else {
            Status.TempRawC = fRawData;
            Status.TempC = fRawData + Config.adjustTempC;
            Status.TempRawF = LSC::getFarenheitFromCelsius(fRawData);
            Status.TempF = LSC::getFarenheitFromCelsius(Status.TempC);
        }
    } 
    DEBUG_FUNC_END_PARMS("%f",bAsFarenheit ? Status.TempF : Status.TempC);
    return(bAsFarenheit ? Status.TempF : Status.TempC);
}

/// @brief Stores the humidity in the Status Object.
/// @param fRawData 
/// @return The adjusted value - or if no value is given, the current humidity.
float CDewSensor::adjustAndStoreHumidity(float fRawData) {
    DEBUG_FUNC_START_PARMS("fRawData=%f",fRawData);
    if(!isnan(fRawData)) {
        Status.HumidityRaw = fRawData;
        Status.Humidity = fRawData + Config.adjustHumidity;
    }
    DEBUG_FUNC_END_PARMS("%f",Status.Humidity);
    return(Status.Humidity);
}


void CDewSensor::updateFromPhysicalSensor() {
    DEBUG_FUNC_START();
    if(this->pSensor) {
        if(m_oUpdateDelay.isDone()) {
            DEBUG_INFO(" ->> reading physical sensor");
            // Force reading in Celsius
            try {
                adjustAndStoreTemperatures(pSensor->readTemperature(false),false);
                adjustAndStoreHumidity(this->pSensor->readHumidity());
            } catch(...) {
                Serial.println("[E] Exception while reading physical sensor");
            }
            Status.LastSensorReadMillis = m_oUpdateDelay.restart();
        } else {
            DEBUG_INFOS(" -> Next sensor read in %lu ms", m_oUpdateDelay.getRemaining());
        }
    }
    DEBUG_FUNC_END();
}   

// units=metric <- Default is kelvin !
// https://api.openweathermap.org/data/2.5/weather?lat=48.251690&lon=11.553641&appid=bf2414e748fff1ee23a45cd32bc61f90
// {"coord":{"lon":11.5536,"lat":48.2517},"weather":[{"id":800,"main":"Clear","description":"clear sky","icon":"01d"}],"base":"stations","main":{"temp":284.94,"feels_like":283.69,"temp_min":283.79,"temp_max":285.41,"pressure":1022,"humidity":58,"sea_level":1022,"grnd_level":963},"visibility":10000,"wind":{"speed":3.55,"deg":271,"gust":5.75},"clouds":{"all":1},"dt":1744358722,"sys":{"type":2,"id":2013241,"country":"DE","sunrise":1744345891,"sunset":1744394260},"timezone":7200,"id":2859147,"name":"Oberschleißheim
void  CDewSensor::updateFromOpenWeatherMap() {
    DEBUG_FUNC_START();
   
    if(isInternetAvailable()) {
    
        if(m_oUpdateDelay.isDone()) {
            try {
                DEBUG_INFOS(" ->> calling web API (%lu + %lu) > %lu",Status.LastWeatherCallMillis,Config.nWeatherCallTimeout, millis());
                String strCallUrl = Config.strWeatherURL + 
                                    "?units=metric"
                                    "&lat=" + Config.strWeatherLatitude + 
                                    "&lon=" + Config.strWeatherLongitude +
                                    "&appid=" + Config.strWeatherAppID +
                                    "&exclude" + Config.strWeatherExclude;
                // TODO: Implement the call here...
                //       - send result via MQTT
                //       - store temperature and humidity in status...
                DEBUG_INFOS("- Calling : %s", strCallUrl.c_str());
                std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
                client->setInsecure();
                HTTPClient oClient;
                // WiFiClient oWiFiClient;
                oClient.begin(*client,strCallUrl);
                int nCode = oClient.GET();
                String strBody = oClient.getString();
                oClient.end();
                DEBUG_INFOS("Received code : %d", nCode);
                DEBUG_INFOS("Received data :\n%s",strBody.c_str());
                if(nCode == 200) {
                    OpenWeatherData.clear();
                    deserializeJson(OpenWeatherData,strBody);
                    // OpenWeatherData["c"] = APP_COPYRIGHT;
                    // OpenWeatherData.set(strBody.c_str());
                    Appl.MsgBus.sendEvent(this,MSG_OPEN_WEATHER_DATA,&OpenWeatherData,Status.DewPoint);
                    JsonObject oMain = OpenWeatherData["main"].as<JsonObject>();
                    DEBUG_JSON_OBJ(oMain);
                    float fTempC = oMain["temp"];
                    float fHumi  = oMain["humidity"];
                    adjustAndStoreTemperatures(fTempC,false);
                    adjustAndStoreHumidity(fHumi);
                }
            } catch(...) {
                Serial.println("[E] Exception in requesting data from Open weather");
            }
            Status.LastWeatherCallMillis = m_oUpdateDelay.restart();
        } else {
            DEBUG_INFOS(" -> Next internet call in %lu ms", m_oUpdateDelay.getRemaining());
        }
        
    }
    DEBUG_FUNC_END();
}

/// @brief Get the current Humidity and stores the adjusted values into the Status object
/// @return the current (adjusted) humidity
float CDewSensor::getHumidity() {
    DEBUG_FUNC_START();
    if(this->pSensor) {
        updateFromPhysicalSensor();
    }
    else {
        updateFromOpenWeatherMap();
    }
    DEBUG_FUNC_END_PARMS(" - humidity    : %f",Status.Humidity);
    return(Status.Humidity);
}

/// @brief Get the current Temperatur and stores the adjusted values into the Status object
/// @param bAsFarenheit Retur value in Celsius or Fahrenheit
/// @return the current (adjusted) temperature
float CDewSensor::getTemperature(bool bAsFarenheit) {
    DEBUG_FUNC_START_PARMS("bAsFarenheit=%d",bAsFarenheit);
    if(this->pSensor) {
        updateFromPhysicalSensor();
    }
    else {
        updateFromOpenWeatherMap();
    }
    float fResult = bAsFarenheit ? Status.TempF : Status.TempC;
    DEBUG_FUNC_END_PARMS(" - temperature : %f",fResult);

    return(fResult);
}

/// @brief Dew Point temperature calculate
/// @param fTemp Temperature in celsius or farenheit
/// @param fHumidity Humidity
/// @param bAsFarenheit fTemp is in Farenheit ?
/// @return Dew Point Temperature in Celsius or Farenheit (depends on bAsFahrenheit)
float CDewSensor::calculateDewPoint(float fTemp, float fHumidity, bool bAsFarenheit) {
    if(bAsFarenheit) fTemp = LSC::getCelsiusFromFarenheit(fTemp);
    DEBUG_FUNC_START_PARMS("%f,%f",fTemp,fHumidity);
    float a, b;
  
  if (fTemp >= 0) {
    a = 7.5;
    b = 237.3;
  } else {
    a = 7.6;
    b = 240.7;
  }
  
  // Sättigungsdampfdruck in hPa
  float sdd = 6.1078 * pow(10, (a*fTemp)/(b+fTemp));
  
  // Dampfdruck in hPa
  float dd = sdd * (fHumidity/100);
  
  // v-Parameter
  float v = log10(dd/6.1078);
  
  // Taupunkttemperatur (°C)
  float tt = (b*v) / (a-v);

  DEBUG_FUNC_END_PARMS("%f",bAsFarenheit ? LSC::getFarenheitFromCelsius(tt) : tt);
  return { tt }; 
}

/// @brief Updates Temp and Humidity, calculates the Dew Point and sends the
///        SensorStatus object via the message bus.
float CDewSensor::getDewPoint(bool bInFarenheit) {
    float fResult = NAN;
    getTemperature();
    getHumidity();
    if(!isnan(Status.TempC) && !isnan(Status.Humidity)) {
        Status.DewPoint = calculateDewPoint(Status.TempC,Status.Humidity,bInFarenheit);
        fResult = Status.DewPoint;
        Appl.MsgBus.sendEvent(this,MSG_SENSOR_STATUS,&Status,Status.SensorLocation);
    }
    return(fResult);
}