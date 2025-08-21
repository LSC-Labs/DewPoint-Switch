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



CFS oFS;
CWiFiController oWiFiController;
CMQTTController oMqttController;
CWebSocket      oWebSocket("/ws");
CWebServer      oWebServer(80);

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
  
  Appl.addConfigHandler("dew",    &oDewApp);
  Appl.addConfigHandler("wifi",   &oWiFiController);
  Appl.addConfigHandler("mqtt",   &oMqttController);
  Appl.addConfigHandler("sensor1",&oSensorID);
  Appl.addConfigHandler("sensor2",&oSensorOD);
  

  Appl.addStatusHandler("dew",    &oDewApp);
  Appl.addStatusHandler("wifi",   &oWiFiController);
  Appl.addStatusHandler("mqtt",   &oMqttController);
  Appl.addStatusHandler("sensor1",&oSensorID);
  Appl.addStatusHandler("sensor2",&oSensorOD);

  Appl.MsgBus.registerEventReceiver(&oDewApp);

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
  delay(200);

  #ifdef DEBUGINFOS
    runDebugTests();
  #endif
}

void loop() {
  // The WebServer is handling already the GET/POST/WebSocket requests 

  // so look for new messages to be processed on the websocket
  oWebSocket.dispatchMessageQueue();

  // Send the heartbeat, if configured
  oMqttController.publishHeartBeat();

  // put your main code here, to run repeatedly:
  oSensorID.dispatch();
  oSensorOD.dispatch();
  oDewApp.dispatch();

}
