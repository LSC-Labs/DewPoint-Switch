#pragma once

#include <Msgs.h>
#include <AppConfig.h>
#include <EventHandler.h>
#include <OutputPinController.h>
#include <StatusHandler.h>


class CFanRelais : public COutputPinController , public IMsgEventReceiver, public IStatusHandler {
    public:
        CFanRelais(int nSwitchPin, bool bLowLevelIsOff = true);
        void writeStatusTo(JsonObject & oStatus);
        int receiveEvent(const void * pSender, int nMsg, const void * pData, int nType) override;
    
};

