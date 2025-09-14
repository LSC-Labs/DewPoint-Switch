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
CButton         oBtn(D8,true);
CSysStatusLed   oStatusLED(D5,D6,D0,false);
CFanRelais      oFanRelais(D7);
CDisplay        oDisplay;
CNTPHandler     oNTP;

/*
#define i2c_Address 0x3c

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };
*/
#ifdef DEVINFOS
void testdrawchar(void) {
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);

  for (uint8_t i = 0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }
  display.display();
  delay(1);
}
void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];

  // initialize
  for (uint8_t f = 0; f < NUMFLAKES; f++) {
    icons[f][XPOS] = random(display.width());
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random(5) + 1;

    Serial.print("x: ");
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(" y: ");
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(" dy: ");
    Serial.println(icons[f][DELTAY], DEC);
  }

  while (1) {
    // draw each icon
    for (uint8_t f = 0; f < NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SH110X_WHITE);
    }
    display.display();
    delay(200);

    // then erase it + move it
    for (uint8_t f = 0; f < NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SH110X_BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > display.height()) {
        icons[f][XPOS] = random(display.width());
        icons[f][YPOS] = 0;
        icons[f][DELTAY] = random(5) + 1;
      }
    }
  }
}
void testdrawline() {
  for (int16_t i = 0; i < display.width(); i += 4) {
    display.drawLine(0, 0, i, display.height() - 1, SH110X_WHITE);
    display.display();
    delay(1);
  }
  for (int16_t i = 0; i < display.height(); i += 4) {
    display.drawLine(0, 0, display.width() - 1, i, SH110X_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();
  for (int16_t i = 0; i < display.width(); i += 4) {
    display.drawLine(0, display.height() - 1, i, 0, SH110X_WHITE);
    display.display();
    delay(1);
  }
  for (int16_t i = display.height() - 1; i >= 0; i -= 4) {
    display.drawLine(0, display.height() - 1, display.width() - 1, i, SH110X_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();
  for (int16_t i = display.width() - 1; i >= 0; i -= 4) {
    display.drawLine(display.width() - 1, display.height() - 1, i, 0, SH110X_WHITE);
    display.display();
    delay(1);
  }
  for (int16_t i = display.height() - 1; i >= 0; i -= 4) {
    display.drawLine(display.width() - 1, display.height() - 1, 0, i, SH110X_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();
  for (int16_t i = 0; i < display.height(); i += 4) {
    display.drawLine(display.width() - 1, 0, 0, i, SH110X_WHITE);
    display.display();
    delay(1);
  }
  for (int16_t i = 0; i < display.width(); i += 4) {
    display.drawLine(display.width() - 1, 0, i, display.height() - 1, SH110X_WHITE);
    display.display();
    delay(1);
  }
  delay(250);
}

void setupTest() {
  // https://github.com/adafruit/Adafruit_SH110x/blob/master/examples/OLED_QTPY_SH1106/SH1106_128x64_i2c_QTPY/SH1106_128x64_i2c_QTPY.ino
  delay(250); // Wait until display is powered up
  display.begin(i2c_Address,true);
  display.display();
  delay(6000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

//   Serial.println("--- drawing a pixel !");
  // Draw a single pixel in white
  // display.drawPixel(10, 10, WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  // display.display();
  // delay(2000);
  // Serial.println("--- test drawing lines !");
  // testdrawline();
  // delay(2000);
  // draw the first ~12 characters in the font
  Serial.println("--- test drawing chars !");
  display.clearDisplay();
  testdrawchar();
  display.display();
  delay(15000);
  display.clearDisplay();

  // Serial.println("--- test drawing bitmaps !");
  // testdrawbitmap(logo_bmp, LOGO_HEIGHT, LOGO_WIDTH);
  // display.display();
  // delay(2000);
  // display.clearDisplay();
 // draw a bitmap icon and 'animate' movement

  // text display tests
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("WLAN (DRLiebl) 78%");
  display.println("");
  /*
  display.println("Failure is always an option");
  display.setTextColor(SH110X_BLACK, SH110X_WHITE); // 'inverted' text
  display.println(3.141592);
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  */
  display.setTextSize(2);
  display.println("17.5 23.5");
  display.setTextSize(1);
  display.println("");
  display.println("Indoor  23.4C 36%");
  display.println("Outdoor 13.5C 56%");
  display.println("Fan is active");
  display.display();
  delay(2000);
  display.clearDisplay();


}
#endif
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
  
  bool bUseConfigData = Appl.readConfigFrom(CONFIG_FILE);
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
    runDebugTests();
  #endif
  oBtn.startMonitoring();
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


}
