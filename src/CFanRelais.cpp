#include <FanRelais.h>


CFanRelais::CFanRelais(int nSwitchPin, bool bLowLevelIsOff) :
    COutputPinController::COutputPinController(nSwitchPin, bLowLevelIsOff) 
    { 
    }

int CFanRelais::receiveEvent(const void * pSender, int nMsg, const void * pData, int nType) {
    switch(nMsg) {
        case MSG_FAN_ACTIVATE : this->switchOn(); break;
        case MSG_FAN_DISABLE  : this->switchOff(); break;
    }
    return(EVENT_MSG_RESULT_OK);
}

void CFanRelais::writeStatusTo(JsonObject & oStatus) {
    oStatus["active"] = this->isOn();
}


