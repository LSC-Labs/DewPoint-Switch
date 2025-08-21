#include <Arduino.h>
#include <AppConfig.h>
#include <FileSystem.h>
#include <Appl.h>
#include <WiFiController.h>
#include <MQTTController.h>
#include <WebSocket.h>
#include <WebServer.h>
#include <WebRoutes.h>
#include <DevelopmentHelper.h>
#include <DewSensor.h>
#include <DewPointApp.h>
#include <FanRelais.h>
#include <SysStatusLed.h>



CFS oFS;
CWiFiController oWiFiController;
CMQTTController oMqttController;
CWebSocket      oWebSocket("/ws");
CWebServer      oWebServer(80);

CDewSensor      oSensorID(D2,DHT11,SENSOR_INDOOR);
CDewSensor      oSensorOD(-1,USE_OPEN_WEATHER,SENSOR_OUTDOOR);
CDewPointApp    oDewApp;

CSysStatusLed   oStatusLED(D5,D6,D7,false);
CFanRelais      oFanRelais(D1);

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
  
  Appl.addConfigHandler("dew",      &oDewApp);
  Appl.addConfigHandler("wifi",     &oWiFiController);
  Appl.addConfigHandler("mqtt",     &oMqttController);
  Appl.addConfigHandler("sensorID", &oSensorID);
  Appl.addConfigHandler("sensorOD", &oSensorOD);
  

  Appl.addStatusHandler("dew",      &oDewApp);
  Appl.addStatusHandler("sensorID", &oSensorID);
  Appl.addStatusHandler("sensorOD", &oSensorOD);
  Appl.addStatusHandler("fan",      &oFanRelais);
  Appl.addStatusHandler("wifi",     &oWiFiController);
  Appl.addStatusHandler("mqtt",     &oMqttController);
  
  Appl.MsgBus.registerEventReceiver(&oDewApp);
  Appl.MsgBus.registerEventReceiver(&oStatusLED);

  oStatusLED.setColor(RGB_COLOR::BLUE);

  Appl.init(APP_NAME,APP_VERSION);
  Appl.sayHello();
  
  bool bUseConfigData = Appl.readConfigFrom(CONFIG_FILE);
  Appl.printDiag();
  Appl.Log.logInfo(F("Initializing services..."));
 
  // put your setup code here, to run once:
  oWiFiController.startWiFi(bUseConfigData);
  oMqttController.setup();
  registerWebRoutes(oWebServer);  
  
  Appl.Log.logInfo(F("Hello world... - let's start the show!"));
  oStatusLED.showStartupFlashLight(250);
  delay(200);

  #ifdef DEBUGINFOS
    runDebugTests();
  #endif
}

void loop() {

  // The WebServer is handling already the GET/POST/WebSocket requests 
  // so look for new messages to be processed on the websocket
  oWebSocket.dispatchMessageQueue();

  // put your main code here, to run repeatedly:
  oSensorID.dispatch();
  oSensorOD.dispatch();

  // Know the values are in place... decide
  oDewApp.dispatch();

  // Send the heartbeat, if configured
  oMqttController.publishHeartBeat();

  oStatusLED.updateLED();
}
