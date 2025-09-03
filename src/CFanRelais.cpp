#ifndef DEBUG_LSC_DEW_FAN
    #undef DEBUGINFOS
#endif
#include <FanRelais.h>
#include <DevelopmentHelper.h>


CFanRelais::CFanRelais(int nSwitchPin, bool bLowLevelIsOff) :
    COutputPinController::COutputPinController(nSwitchPin, bLowLevelIsOff) 
    { 
    }

int CFanRelais::receiveEvent(const void * pSender, int nMsg, const void * pData, int nType) {
    switch(nMsg) {
        case MSG_FAN_ACTIVATE : this->switchOn();  DEBUG_INFO("FAN: switching on"); break;
        case MSG_FAN_DISABLE  : this->switchOff(); DEBUG_INFO("FAN: switching off"); break;
    }
    return(EVENT_MSG_RESULT_OK);
}

void CFanRelais::writeStatusTo(JsonObject & oStatus) {
    oStatus["active"] = this->isOn();
}


