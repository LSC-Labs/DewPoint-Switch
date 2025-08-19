#include <AppConfig.h>
#include <DewPointApp.h>
#include <LSCUtils.h>

#define DEW_CFG_HYSTERESIS      F("Hysteresis")
#define DEW_CFG_DEWDELTA        F("DewDelta")
#define DEW_CFG_MIN_INTERN      F("MinIntern")
#define DEW_CFG_MIN_EXTERN      F("MinExtern")
#define DEW_STATUS_ERROR        F("inError")
#define DEW_STATUS_INTERRMSG    F("intErrMsg")
#define DEW_STATUS_EXTERRMSG    F("extErrMsg")
#define DEW_STATUS_ACTIVEVAN    F("vanActive")

int CDewPointApp::receiveEvent(const void * pSender, int nMsgId, const void * pMessage, int nType) {

    switch(nMsgId) {
        case MSG_SENSOR_STATUS : // Sensor is sending it's status...
                                break;
    }
    return(EVENT_MSG_RESULT_OK);
}

void CDewPointApp::writeStatusTo(JsonObject & oStatus) {
    oStatus[DEW_STATUS_ERROR]       = Status.isInErrorState;
    oStatus[DEW_STATUS_INTERRMSG]   = Status.internalErrorMsg;
    oStatus[DEW_STATUS_EXTERRMSG]   = Status.externalErrorMsg;
    oStatus[DEW_STATUS_ACTIVEVAN]   = Status.ActiveVentilation;
}

void CDewPointApp::writeConfigTo(JsonObject & oCfg, bool bHideCritical) {
    oCfg[DEW_CFG_HYSTERESIS] = Config.activationHysteresis;
    oCfg[DEW_CFG_DEWDELTA]   = Config.dewPointActivationDelta;
    oCfg[DEW_CFG_MIN_INTERN] = Config.minActivationLevelIntern;
    oCfg[DEW_CFG_MIN_EXTERN] = Config.minActivationLevelExtern;
}

void CDewPointApp::readConfigFrom(JsonObject & oCfg) {
    LSC::setValue(&Config.activationHysteresis,oCfg[DEW_CFG_HYSTERESIS]);
    LSC::setValue(&Config.dewPointActivationDelta,oCfg[DEW_CFG_DEWDELTA]);
    LSC::setValue(&Config.minActivationLevelIntern,oCfg[DEW_CFG_MIN_INTERN]);
    LSC::setValue(&Config.minActivationLevelExtern,oCfg[DEW_CFG_MIN_EXTERN]);
}