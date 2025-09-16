#include <AppActions.h>


CAppActions::CAppActions() {
    m_oButtonRestartDelay.start(5000,false);
}
int CAppActions::receiveEvent(const void *pSender, int nMsg, const void *pMsg, int nMsgType){
    switch(nMsg) {
        case MSG_BUTTON_ON  : // Button is pressed... Start the timer
                                m_bButtonPressed = true;
                                m_oButtonRestartDelay.restart();
                                break;
        case MSG_BUTTON_OFF :   m_bButtonPressed = false; 
                                break;
    }
    return(EVENT_MSG_RESULT_OK);
}

void CAppActions::dispatch() {
    // When button is pressed long enough - restart...
    if(m_bButtonPressed) {
        if(m_oButtonRestartDelay.isDone()) {
            m_bRebootPending = true;
            Appl.MsgBus.sendEvent(this,MSG_REBOOT_REQUEST,0,0);
        }
    }
}