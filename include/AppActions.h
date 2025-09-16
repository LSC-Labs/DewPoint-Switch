#pragma once
#include <Arduino.h>
#include <Appl.h>
#include <EventHandler.h>
#include <Msgs.h>
#include <SimpleDelay.h>

class CAppActions : public IMsgEventReceiver {
    protected:
        bool m_bRebootPending = false;
        bool m_bButtonPressed = false;
        CSimpleDelay m_oButtonRestartDelay;
        long m_ulButtonPressed = 0L;
        long m_ulWaitUntilRestart = 5000;   // 5 secs button pressed before restart - default...

    public:
        CAppActions();
        int receiveEvent(const void *pSender, int nMsg, const void *pMsg, int nMsgType);
        void dispatch();
};