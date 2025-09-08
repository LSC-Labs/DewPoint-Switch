#include <Appl.h>
#include <EventHandler.h>
#include <Msgs.h>
#include <Network.h>
#include <Display.h>
#include <DisplayBasePage.h>


void CDisplayBasePage::init() {
    Appl.MsgBus.registerEventReceiver(this);
};

/**
 * Collect all necessary informations about the current status to display...
 */
int CDisplayBasePage::receiveEvent(const void * pSender, int nMsgType, const void * pMessage, int nType) {
    switch(nMsgType) {
        case MSG_SENSOR_STATUS: 
                                if(nType == SENSOR_INDOOR) Status.pSI = (DewStatus *) pMessage;
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


    }
    return(EVENT_MSG_RESULT_OK);
}

int CDisplayBasePage::drawPageHeader(CDisplay *pDisplay) {
    DEBUG_FUNC_START();
    pDisplay->setTextSize(1);
    pDisplay->setCursor(0,0);
    pDisplay->setTextColor(SCREEN_TEXT_WHITE);
    pDisplay->print(Appl.getISOTime());
    char szWiFiStatus[80];
    sprintf(szWiFiStatus,"%s %d%%","RSSI",WiFi.RSSI());
    pDisplay->printRightAligned(szWiFiStatus);
    pDisplay->drawLine(0,9,pDisplay->width(),9,SCREEN_TEXT_WHITE);
    pDisplay->setCursor(0,11);
    DEBUG_FUNC_END();
    return(pDisplay->getCursorY());
}
