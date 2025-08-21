#include <AppConfig.h>
#include <DewPointApp.h>
#include <LSCUtils.h>

#define DEW_CFG_HYSTERESIS      F("hysteresis")
#define DEW_CFG_DEWDELTA        F("delta")
#define DEW_CFG_MIN_INTERN      F("min_intern")
#define DEW_CFG_MIN_EXTERN      F("min_extern")
#define DEW_STATUS_ERROR        F("in_error")
#define DEW_STATUS_INTERRMSG    F("int_error")
#define DEW_STATUS_EXTERRMSG    F("ext_error")
#define DEW_STATUS_ACTIVEVAN    F("van_active")

int CDewPointApp::receiveEvent(const void * pSender, int nMsgId, const void * pMessage, int nType) {

    switch(nMsgId) {
        case MSG_SENSOR_STATUS : // Sensor is sending it's status...
                                if(nType == SENSOR_INDOOR) this->pIndoorStatus  = (DewStatus *) pMessage;
                                else                       this->pOutdoorStatus = (DewStatus *) pMessage;
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
    LSC::setValue(&Config.activationHysteresis,     oCfg[DEW_CFG_HYSTERESIS]);
    LSC::setValue(&Config.dewPointActivationDelta,  oCfg[DEW_CFG_DEWDELTA]);
    LSC::setValue(&Config.minActivationLevelIntern, oCfg[DEW_CFG_MIN_INTERN]);
    LSC::setValue(&Config.minActivationLevelExtern, oCfg[DEW_CFG_MIN_EXTERN]);
}

/// @brief Dispatch and update if needed
void CDewPointApp::dispatch() {
    if(this->pIndoorStatus && this->pOutdoorStatus) {
        DEBUG_INFO("DewPoint infos:");
        DEBUG_INFOS(" - temp     : %f \t- %f", pIndoorStatus->TempC, pOutdoorStatus->TempC);
        DEBUG_INFOS(" - humidity : %f \t- %f", pIndoorStatus->Humidity, pOutdoorStatus->Humidity);
        DEBUG_INFOS(" - dewpoint : %f \t- %f", pIndoorStatus->DewPoint, pOutdoorStatus->DewPoint);
    }
}