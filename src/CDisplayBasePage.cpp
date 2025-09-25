#ifndef DEBUG_LSC_DISPLAY_PAGES
    #undef DEBUGINFOS
#endif


#include <Appl.h>
#include <EventHandler.h>
#include <Msgs.h>
#include <Network.h>
#include <Display.h>
#include <DisplayBasePage.h>

const unsigned char ICON_WIFI_FULL_1[] PROGMEM  ={ // wifi icon
  0xff, 0x00, 0x7e, 0x00, 0x3c, 0x00, 0x18, 0x00
};
const unsigned char ICON_WIFI_HALF_1[] PROGMEM  ={ // wifi icon
  0x00, 0x00, 0x7e, 0x00, 0x3c, 0x00, 0x18, 0x00
};
const unsigned char ICON_WIFI_BAD_1[] PROGMEM  ={ // wifi icon
  0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x18, 0x00
};
const unsigned char ICON_WIFI_OFF_1[] PROGMEM  ={ // wifi icon
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00
};



void CDisplayBasePage::init() {
  m_oWiFiRefresh.start(5000,false);
  Appl.MsgBus.registerEventReceiver(this);
};

/**
 * Collect all necessary informations about the current status to display...
 */
int CDisplayBasePage::receiveEvent(const void * pSender, int nMsgType, const void * pMessage, int nType) {
    switch(nMsgType) {
        case MSG_SENSOR_STATUS: 
                                if(nType == SENSOR_INDOOR)  Status.pSI = (DewStatus *) pMessage;
                                if(nType == SENSOR_OUTDOOR) Status.pSO = (DewStatus *) pMessage;
                                break;
        case MSG_REBOOT_REQUEST: Status.isRebootPending = true;    break;
        case MSG_BUTTON_ON     : Status.isButtonPressed = true;    break;
        case MSG_BUTTON_OFF    : Status.isButtonPressed = false;   break;
        case MSG_WIFI_CONNECTED: 
                    Status.isInAccessPointMode = (nType == WIFI_ACCESS_POINT_MODE);
                    Status.isInStationMode     = (nType == WIFI_STATION_MODE);
                    Status.isWiFiConnected = Status.isInAccessPointMode || Status.isInStationMode;
                    break;
        case MSG_WIFI_DISABLED :
                    Status.isInAccessPointMode = false;
                    Status.isInStationMode = false;
                    Status.isWiFiConnected = false;
                    break;
        case MSG_FAN_ACTIVATE : Status.isFanActive = true; break;
        case MSG_FAN_DISABLE  : Status.isFanActive = false; break;
    }
    return(EVENT_MSG_RESULT_OK);
}
namespace BaseDisplay {
  unsigned int nFanRotateCounter = 0;
  const char tRotateChars[] = "/-\\|";
};

/**
 * draw a standard page header at the display
 * Text size will become 1, the color white.
 * Left is the current time, right is the WiFi signal strength,
 * below a line..
 * @return the new y position where the page can start to write..
 */
int CDisplayBasePage::drawPageHeader(CDisplay *pDisplay) {
    DEBUG_FUNC_START();
    pDisplay->setTextSize(1);
    pDisplay->setCursor(0,0);
    pDisplay->setTextColor(COLOR_WHITE);
    pDisplay->print(Appl.getISOTime());

    if(m_oWiFiRefresh.isDone()) {
      m_nRSSI = WiFi.RSSI();
      m_oWiFiRefresh.restart();
    }

    // char szWiFiStatus[10];
    // sprintf(szWiFiStatus,"%ddB",m_nRSSI);
    // pDisplay->printRightAligned(szWiFiStatus);

    // Draw the WiFi bitmap, depending on the RSSI strength...
    // int nPosX = pDisplay->setRightAlignedTextCursor(szWiFiStatus);

    char szFanView[2] = "o";
    if(Status.isFanActive) {
      if(BaseDisplay::nFanRotateCounter >= strlen(BaseDisplay::tRotateChars)) BaseDisplay::nFanRotateCounter = 0;
        szFanView[0] = BaseDisplay::tRotateChars[BaseDisplay::nFanRotateCounter++];
    } 
    pDisplay->printAt(pDisplay->width() - (3 * 8),0,szFanView);
    
    
    const unsigned char * pBitMap = ICON_WIFI_OFF_1;
    if(m_nRSSI > -98) pBitMap = ICON_WIFI_BAD_1;
    if(m_nRSSI > -82) pBitMap = ICON_WIFI_HALF_1;
    if(m_nRSSI > -60) pBitMap = ICON_WIFI_FULL_1;
    pDisplay->drawBitmap(pDisplay->width() - 8,0,pBitMap,8,8,COLOR_WHITE);

    pDisplay->drawLine(0,9,pDisplay->width(),9,COLOR_WHITE);
    pDisplay->setCursor(0,11);
    DEBUG_FUNC_END();
    return(pDisplay->getCursorY());
}

int CDisplayBasePage::drawNumberFrame(CDisplay *pDisplay) {
  int16_t nMiddle = pDisplay->width() / 2;
  int16_t nOffsetX = 6;
  int16_t nOffsetY = pDisplay->getCursorY() + 2;
  pDisplay->drawLine(nMiddle,pDisplay->getCursorY(),nMiddle,pDisplay->height(),COLOR_WHITE);
  pDisplay->printAt(nOffsetX +6 ,nOffsetY,"Indoor");
  pDisplay->printAt(nMiddle +6 + nOffsetX,nOffsetY,"Outdoor");
  nOffsetY += 12;
  return(nOffsetY);
}