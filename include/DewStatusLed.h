#pragma once

#include <SysStatusLed.h>

class CDewStatusLed : public CSysStatusLed {
    bool m_bFanIsActive = false;
    public:
        CDewStatusLed(int nRedPin, int nGreenPin, int nBluePin, bool bActiveLow = true);
        int receiveEvent(const void * pSender, int nMsgId, const void * pMessage, int nType) override;
        unsigned long getNormalBlinkOffTime() override;
        unsigned long getNormalBlinkOnTime() override;
};