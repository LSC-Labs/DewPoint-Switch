#pragma once
#include <Arduino.h>
#include <Appl.h>
#include <EventHandler.h>
#include <Msgs.h>

class CAppActions : public IMsgEventReceiver {
    protected:
        bool m_bButtonPressed = false;
        long m_ulButtonPressed = 0L;
        long m_ulWaitUntilRestart = 5000;   // 5 secs button pressed before restart - default...

    public:
        int receiveEvent(const void *pSender, int nMsg, const void *pMsg, int nMsgType){
            switch(nMsg) {
                case MSG_BUTTON_ON  : m_ulButtonPressed = millis(); break;
                case MSG_BUTTON_OFF : m_ulButtonPressed = 0; break;
            }
        }

        void dispatch() {
            // When button is pressed long enough - restart...
            if(m_ulButtonPressed > 0) {
                if((m_ulButtonPressed + m_ulWaitUntilRestart) > millis()) {
                    Appl.MsgBus.sendEvent(this,MSG_REBOOT_REQUEST,0,0);
                }
            }
        }
};