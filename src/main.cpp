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
#include <DewPointSwitch.h>
#include <FanRelais.h>
#include <SysStatusLed.h>
#include <Display.h>
#include <DisplayPages.h>
#include <NTPHandler.h>
#include <Button.h>
#include <AppActions.h>
/*
#include <Adafruit_SH110X.h>
#include <Adafruit_GFX.h>
*/


CFS oFS;
CWiFiController oWiFiController;
CMQTTController oMqttController;
CWebSocket      oWebSocket("/ws");
CWebServer      oWebServer(80);

CDewSensor      oSensorID(D4,DHT11,SENSOR_INDOOR);
// CDewSensor      oSensorOD(D3,DHT11,SENSOR_OUTDOOR);
CDewSensor      oSensorOD(-1,USE_OPEN_WEATHER,SENSOR_OUTDOOR);

CDewPointSwitch oDewPointSwitch(oSensorID,oSensorOD);

CSysStatusLed   oStatusLED(D5,D6,D0,false);
CButton         oBtn(D7,true);
CFanRelais      oFanRelais(D8);
CDisplay        oDisplay;
CNTPHandler     oNTP;
CAppActions     oAppActions;


#ifdef DEBUGINFOS
void runDebugTests() {

  JsonDocument oCfg;
  JsonObject oCfgNode = oCfg.to<JsonObject>();
  Appl.writeConfigTo(oCfgNode,false);
  DEBUG_INFO("Current configuration:");
  DEBUG_JSON_OBJ(oCfgNode);

  oSensorID.getDewPoint();
  JsonDocument oStatus;
  JsonObject oStatusNode = oStatus.to<JsonObject>();
  Appl.writeStatusTo(oStatusNode);
  DEBUG_INFO("Current status:");
  DEBUG_JSON_OBJ(oStatusNode);

  DEBUG_INFO("Sys diag:");
  Appl.printDiag();
  Appl.Log.logInfo("Tests finished...");
  // Test of RGB LED
  // oStatusLED.runTests();
  
  DEBUG_INFO("Testing Fan...");
  oFanRelais.switchOn();
  delay(1000);
  oFanRelais.switchOff();
  oStatusLED.runTests();
}
#endif

void registerDisplayPages() {
  oDisplay.registerPage(new CDisplayLogoPage(DISPLAY_START_PAGE));
  oDisplay.registerPage(new CDisplayAccessPointPage("APConfig-Page"));
  oDisplay.registerPage(new CDisplayDewPointPage("DewPointPage"));
  oDisplay.registerPage(new CDisplayTempPage("TemperaturePage"));
  oDisplay.registerPage(new CDisplayHumidityPage("HumidityPage"));
  
}

void registerModules() {
Appl.addConfigHandler("dewswitch",&oDewPointSwitch);
  Appl.addConfigHandler("wifi",     &oWiFiController);
  Appl.addConfigHandler("mqtt",     &oMqttController);
  Appl.addConfigHandler("sensorID", &oSensorID);
  Appl.addConfigHandler("sensorOD", &oSensorOD);
  Appl.addConfigHandler("ntp",      &oNTP);
  Appl.addConfigHandler("display",  &oDisplay);
  

  Appl.addStatusHandler("dewswitch",&oDewPointSwitch);
  Appl.addStatusHandler("sensorID", &oSensorID);
  Appl.addStatusHandler("sensorOD", &oSensorOD);
  Appl.addStatusHandler("fan",      &oFanRelais);
  Appl.addStatusHandler("wifi",     &oWiFiController);
  Appl.addStatusHandler("mqtt",     &oMqttController);
  Appl.addStatusHandler("ntp",      &oNTP);
  
  Appl.MsgBus.registerEventReceiver(&oStatusLED);
  Appl.MsgBus.registerEventReceiver(&oSensorID);
  Appl.MsgBus.registerEventReceiver(&oSensorOD);
  Appl.MsgBus.registerEventReceiver(&oFanRelais);
  Appl.MsgBus.registerEventReceiver(&oNTP);
  Appl.MsgBus.registerEventReceiver(&oAppActions);
}



void setup() {
  Serial.begin(115200);
  DEBUG_FUNC_START();
  DEBUG_INFOS("\nInitializing application: \"%s\" Version: %s\n",APP_NAME,APP_VERSION);
  registerDisplayPages();
  oDisplay.init();
  // oBtn.setup(D7,true);

#ifdef DEVTESTS
  setupTest();
#endif
  registerModules();

  oStatusLED.setColor(RGB_COLOR::BLUE);

  Appl.init(APP_NAME,APP_VERSION);
  Appl.sayHello();

  bool bUseConfigData = !oBtn.isPressed();
  if(bUseConfigData) Appl.readConfigFrom(CONFIG_FILE);
  
  Appl.printDiag();
  Appl.Log.logInfo(F("Initializing services..."));
 
  // put your setup code here, to run once:
  oWiFiController.startWiFi(bUseConfigData);
  oMqttController.setup();
  registerWebRoutes(oWebServer);  
  // oWebServer.addMiddleware(&oCorsMiddleware);
  // oWebServer.addMiddleware(&oTestMW);
  oWebServer.addHandler(&oWebSocket);
  oWebServer.begin();
  
  Appl.Log.logInfo(F("Hello world... - let's start the show!"));
  oStatusLED.showStartupFlashLight(250);
  delay(200);

  #ifdef DEBUGINFOS
  //  runDebugTests();
  #endif
  oBtn.startMonitoring();

  if(oWiFiController.Status.isInAccessPointMode) {
    oDisplay.activatePage("APConfig-Page");
    oDisplay.freezePage();
  }
  DEBUG_FUNC_END();
}

void loop() {
  
  // The WebServer is handling already the GET/POST/WebSocket requests 
  // so look for new messages to be processed on the websocket
  oWebSocket.dispatchMessageQueue();

  // Check the Dew Point - if needed, switch the relais
  oDewPointSwitch.dispatch();

  // Update the status LED, depending on the current system status
  oStatusLED.updateLED();

  oDisplay.dispatch();

  // Send the heartbeat, if configured
  oMqttController.publishHeartBeat();

  oAppActions.dispatch();

}
