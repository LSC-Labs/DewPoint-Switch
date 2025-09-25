#include <DewStatusLed.h>
#include <EventHandler.h>
#include <AppConfig.h>

CDewStatusLed::CDewStatusLed(int nRedPin, int nGreenPin, int nBluePin,bool bActiveLow) :
    CSysStatusLed(nRedPin,nGreenPin,nBluePin,bActiveLow) {}

int CDewStatusLed::receiveEvent(const void *pSender, int nMsgId, const void *pData, int nMsgType) {
    if(nMsgId == MSG_FAN_ACTIVATE)  m_bFanIsActive = true;
    if(nMsgId == MSG_FAN_DISABLE)   m_bFanIsActive = false;
    return(CSysStatusLed::receiveEvent(pSender,nMsgId,pData,nMsgType));
}

unsigned long CDewStatusLed::getNormalBlinkOffTime() {
    if(m_bFanIsActive) return(200);
    else return CSysStatusLed::getNormalBlinkOffTime();
}

unsigned long CDewStatusLed::getNormalBlinkOnTime() {
    if(m_bFanIsActive) return(1000);
    else return CSysStatusLed::getNormalBlinkOnTime();
}
