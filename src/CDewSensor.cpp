#ifndef DEBUG_LSC_DEW_SENSOR
    #undef DEBUG_INFOS
#endif

#include <LSCUtils.h>

// https://www.heise.de/select/make/2022/1/2135511212557842576
// https://www.heise.de/select/make/2022/2/2204711461516715363
// https://www.heise.de/select/make/2022/1/softlinks/x2yt?wt_mc=pred.red.make.make012022.022.softlink.softlink

// See also : https://github.com/MakeMagazinDE/Taupunktluefter
#include <DewSensor.h>
#include <Network.h>
#include <ESP8266HTTPClient.h>

#define SENSOR_CFG_IS_SENSOR        F("sensor")
#define SENSOR_CFG_ADJUST_TEMP      F("adjust_temp")
#define SENSOR_CFG_ADJUST_HUMIDITY  F("adjust_humidity")
#define SENSOR_CFG_OPENWEATHER_URL  F("web_url")
#define SENSOR_CFG_OPENWEATHER_KEY  F("web_key")
#define SENSOR_CFG_OPENWEATHER_LONG F("web_long")
#define SENSOR_CFG_OPENWEATHER_LAT  F("web_lat")
#define SENSOR_STATUS_TEMPC         F("tempc")
#define SENSOR_STATUS_HUMIDITY      F("hum")
#define SENSOR_STATUS_TEMPC_RAW     F("tempc_raw")
#define SENSOR_STATUS_HUMIDITY_RAW  F("hum_raw")
#define SENSOR_STATUS_DEWPOINT      F("dew_point")
#define SENSOR_STATUS_LAST_UPD_MS   F("last_upd_ms")






CDewSensor::CDewSensor() {};
CDewSensor::~CDewSensor() { if(pSensor != nullptr) free(pSensor); }

CDewSensor::CDewSensor(int nPin, int nSensorType, int nLocation) {
    setup(nPin,nSensorType,nLocation);
};

void CDewSensor::setup(int nPin, int nSensorType, int nLocation) {
    if(nSensorType == USE_OPEN_WEATHER) {
        Config.bIsSensor = false;
    } else {
        this->Config.bIsSensor = true;
        this->pSensor =  new DHT(nPin,nSensorType);
        this->pSensor->begin();
    }
    Status.SensorLocation = nLocation;    
};

void CDewSensor::readConfigFrom(JsonObject & oCfg) {
    LSC::setValue(&Config.bIsSensor,            oCfg[SENSOR_CFG_IS_SENSOR]);
    LSC::setValue(&Config.adjustHumidity,       oCfg[SENSOR_CFG_ADJUST_HUMIDITY]);
    LSC::setValue(&Config.adjustTempC,          oCfg[SENSOR_CFG_ADJUST_TEMP]);
    LSC::setValue(Config.strWeatherAppID,       oCfg[SENSOR_CFG_OPENWEATHER_KEY]);
    LSC::setValue(Config.strWeatherLongitude,   oCfg[SENSOR_CFG_OPENWEATHER_LONG]);
    LSC::setValue(Config.strWeatherLatitude,    oCfg[SENSOR_CFG_OPENWEATHER_LAT]);
}

void CDewSensor::writeConfigTo(JsonObject & oCfg, bool bHideCritical) {
    oCfg[SENSOR_CFG_IS_SENSOR]          = Config.bIsSensor;
    oCfg[SENSOR_CFG_ADJUST_HUMIDITY]    = Config.adjustHumidity;
    oCfg[SENSOR_CFG_ADJUST_TEMP]        = Config.adjustTempC;
    oCfg[SENSOR_CFG_OPENWEATHER_KEY]    = Config.strWeatherAppID;
    oCfg[SENSOR_CFG_OPENWEATHER_LONG]   = Config.strWeatherLongitude;
    oCfg[SENSOR_CFG_OPENWEATHER_LAT]    = Config.strWeatherLatitude;
}

void CDewSensor::writeStatusTo(JsonObject & oStatus) {
    oStatus[SENSOR_STATUS_TEMPC]        = Status.TempC;
    oStatus[SENSOR_STATUS_TEMPC_RAW]    = Status.TempRaw;
    oStatus[SENSOR_STATUS_HUMIDITY]     = Status.Humidity;
    oStatus[SENSOR_STATUS_HUMIDITY_RAW] = Status.HumidityRaw;
    oStatus[SENSOR_STATUS_DEWPOINT]     = Status.DewPoint;
    oStatus[SENSOR_STATUS_LAST_UPD_MS]  = Status.LastCallMillis;
}

float CDewSensor::setTemperature(float fRawData) {
    if(!isnan(fRawData)) Status.TempC = fRawData + Config.adjustTempC;
    return(Status.TempC);
}

float CDewSensor::setHumidity(float fRawData) {
    if(!isnan(fRawData)) Status.Humidity = fRawData + Config.adjustHumidity;
    return(Status.Humidity);
}

// units=metric <- Default is kelvin !
// https://api.openweathermap.org/data/2.5/weather?lat=48.251690&lon=11.553641&appid=bf2414e748fff1ee23a45cd32bc61f90
// {"coord":{"lon":11.5536,"lat":48.2517},"weather":[{"id":800,"main":"Clear","description":"clear sky","icon":"01d"}],"base":"stations","main":{"temp":284.94,"feels_like":283.69,"temp_min":283.79,"temp_max":285.41,"pressure":1022,"humidity":58,"sea_level":1022,"grnd_level":963},"visibility":10000,"wind":{"speed":3.55,"deg":271,"gust":5.75},"clouds":{"all":1},"dt":1744358722,"sys":{"type":2,"id":2013241,"country":"DE","sunrise":1744345891,"sunset":1744394260},"timezone":7200,"id":2859147,"name":"Oberschleißheim
void  CDewSensor::updateFromWeatherMapData() {
    DEBUG_FUNC_START();
    if(Status.LastCallMillis == 0 || (Status.LastCallMillis + Config.nWeatherCallTimeout) > millis()) {
        DEBUG_INFOS(" ->> calling web API (%lu + %lu) > %lu",Status.LastCallMillis,Config.nWeatherCallTimeout, millis());
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
            DEBUG_INFO(" ------- Analysing received data :");
            DEBUG_JSON_OBJ(OpenWeatherData);
            JsonObject oMain = OpenWeatherData["main"].as<JsonObject>();
            DEBUG_JSON_OBJ(oMain);
            float fTempC = oMain["temp"];
            float fHumi  = oMain["humidity"];
            if(!isnan(fTempC)) {
                DEBUG_INFOS(" - setting temperature to: %f",fTempC);
                Status.TempC = fTempC;
            }
            if(!isnan(fHumi)) {
                DEBUG_INFOS(" - setting humidity to: %f",fHumi);
                Status.Humidity = fHumi;
            }
        }
        Status.LastCallMillis = millis();
    }
    DEBUG_FUNC_END();
}

float CDewSensor::getRawHumidity() {
    DEBUG_FUNC_START();
    float fResult = NAN;
    if(this->pSensor) {
        fResult = this->pSensor->readHumidity();
        DEBUG_INFO(" - reading from sensor...");
    }
    else {
        updateFromWeatherMapData();
        fResult = Status.Humidity;
    }
    DEBUG_INFOS(" - humidity    : %f",fResult);
    return(fResult);
}

float CDewSensor::getRawTemperature(bool bAsFarenheit) {
    DEBUG_FUNC_START();
    float fResult = NAN;
    if(this->pSensor) {
        DEBUG_INFO(" - reading from sensor...");
        fResult = this->pSensor->readTemperature(bAsFarenheit); 
    }
    else {
        updateFromWeatherMapData();
        fResult = Status.TempC;
    }
    fResult = setTemperature(fResult);
    DEBUG_INFOS(" - temperature : %f",fResult);
    return(fResult);
}

float CDewSensor::calculateDewPoint(float fTemp, float fHumidity) {
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
  DEBUG_FUNC_END_PARMS("%f",tt);
  return { tt }; 
}

void CDewSensor::dispatch() {
    float fTemperature = getRawTemperature();
    float fHumidity = getRawHumidity();
    if(!isnan(fTemperature) && !isnan(fHumidity)) {
        Status.TempC = fTemperature;
        Status.Humidity = fHumidity;
        Status.DewPoint = calculateDewPoint(fTemperature,fHumidity);
        Appl.MsgBus.sendEvent(this,MSG_SENSOR_STATUS,&Status,Status.SensorLocation);
    }
}