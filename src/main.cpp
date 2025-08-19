#include <Arduino.h>
#include <AppConfig.h>
#include <FileSystem.h>
#include <Appl.h>
#include <WiFiController.h>
#include <MQTTController.h>
#include <DevelopmentHelper.h>
#include <DewSensor.h>
#include <DewPointApp.h>



CFS oFS;
CWiFiController oWiFiController;
CMQTTController oMqttController;

// DHT oDHTIndoor(D2,DHT11);
// DHT oDHTIndoor(-1,DHT11);

CDewSensor oSensorID(D2,DHT11,SENSOR_INDOOR);
CDewSensor oSensorOD(-1,USE_OPEN_WEATHER,SENSOR_OUTDOOR);
CDewPointApp oDewApp;


#ifdef DEBUGINFOS
void runDebugTests() {
  JsonDocument oCfg;
  JsonObject oCfgNode = oCfg.to<JsonObject>();
  Appl.writeConfigTo(oCfgNode,false);
  DEBUG_INFO("Current configuration:");
  DEBUG_JSON_OBJ(oCfgNode);
}
#endif




void setup() {
  Serial.begin(115200);
  DEBUG_INFOS("\nInitializing application: \"%s\" Version: %s\n",APP_NAME,APP_VERSION);
  
  Appl.addConfigHandler("app", &oDewApp);
  Appl.addConfigHandler("wifi",&oWiFiController);
  Appl.addConfigHandler("mqtt",&oMqttController);

  Appl.addConfigHandler("app",&oDewApp);
  Appl.addStatusHandler("wifi",&oWiFiController);
  Appl.addStatusHandler("mqtt",&oMqttController);

  Appl.MsgBus.registerEventReceiver(&oDewApp);


  Appl.init("Dew Controller","0.1.0");
  Appl.sayHello();
  
  bool bUseConfigData = Appl.readConfigFrom(CONFIG_FILE);
  Appl.printDiag();
  Appl.Log.logInfo(F("Initializing services..."));
 
  // put your setup code here, to run once:
  oWiFiController.startWiFi(bUseConfigData);
  oMqttController.setup();
  
  Appl.Log.logInfo(F("Hello world... - let's start the show!"));
  delay(200);

  #ifdef DEBUGINFOS
    runDebugTests();
  #endif
}

void loop() {
  // put your main code here, to run repeatedly:
}
