#ifndef DEBUG_LSC_DEW_SWITCH
    #undef DEBUGINFOS
#endif

#include <AppConfig.h>
#include <DewPointSwitch.h>
#include <LSCUtils.h>


#define DEW_CFG_HYSTERESIS      F("hysteresis")
#define DEW_CFG_DEWDELTA        F("dewdelta")
#define DEW_CFG_MIN_INTERN      F("min_indoor")
#define DEW_CFG_MIN_EXTERN      F("min_outdoor")
#define DEW_STATUS_ERROR        F("in_error")
#define DEW_STATUS_INTERRMSG    F("int_error")
#define DEW_STATUS_EXTERRMSG    F("ext_error")
#define DEW_STATUS_ACTIVEVAN    F("van_active")


void CDewPointSwitch::writeStatusTo(JsonObject & oStatus) {
    oStatus[DEW_STATUS_ERROR]       = Status.isInErrorState;
    oStatus[DEW_STATUS_INTERRMSG]   = Status.internalErrorMsg;
    oStatus[DEW_STATUS_EXTERRMSG]   = Status.externalErrorMsg;
    oStatus[DEW_STATUS_ACTIVEVAN]   = Status.ActiveVentilation;
}

void CDewPointSwitch::writeConfigTo(JsonObject & oCfg, bool bHideCritical) {
    oCfg[DEW_CFG_HYSTERESIS] = Config.activationHysteresis;
    oCfg[DEW_CFG_DEWDELTA]   = Config.dewPointActivationDelta;
    oCfg[DEW_CFG_MIN_INTERN] = Config.minActivationLevelIndoor;
    oCfg[DEW_CFG_MIN_EXTERN] = Config.minActivationLevelOutdoor;
}

void CDewPointSwitch::readConfigFrom(JsonObject & oCfg) {
    LSC::setValue(&Config.activationHysteresis,     oCfg[DEW_CFG_HYSTERESIS]);
    LSC::setValue(&Config.dewPointActivationDelta,  oCfg[DEW_CFG_DEWDELTA]);
    LSC::setValue(&Config.minActivationLevelIndoor, oCfg[DEW_CFG_MIN_INTERN]);
    LSC::setValue(&Config.minActivationLevelOutdoor, oCfg[DEW_CFG_MIN_EXTERN]);
}

/// @brief Dispatch and update if needed
void CDewPointSwitch::dispatch() {
    float fIndoor   = NAN;
    float fOutdoor  = NAN;
    if(pSID) fIndoor  = pSID->getDewPoint();
    if(pSOD) fOutdoor = pSOD->getDewPoint();
    
    
    if(!isnan(fIndoor) && !isnan(fOutdoor)) {
        if(m_oUpdateDelay.isDone()) {
            bool bNewStatus = false;
            if(m_fLastInternalDewPoint != fIndoor) {
                m_fLastInternalDewPoint = fIndoor;
                bNewStatus = true;
            }
            if(m_fLastExternalDewPoint != fOutdoor) {
                m_fLastExternalDewPoint = fOutdoor;
                bNewStatus = true;
            }
            if(bNewStatus) {
                DEBUG_INFO("DewPointSwitch infos:");
                DEBUG_INFOS(" - temp     : %f \t- %f", pSID->Status.TempC, pSOD->Status.TempC);
                DEBUG_INFOS(" - humidity : %f \t- %f", pSID->Status.Humidity, pSOD->Status.Humidity);
                DEBUG_INFOS(" - dewpoints: %f \t- %f", fIndoor, fOutdoor);
            }
            m_oUpdateDelay.restart();
        }
    }
}