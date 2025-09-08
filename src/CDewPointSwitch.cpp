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

bool CDewPointSwitch::setNewDewpointState(float fIndoor,float fOutdoor) {
    bool bSetDone = false;
    if(!isnan(fIndoor) && !isnan(fOutdoor)) {
        if(m_fLastInternalDewPoint != fIndoor) {
            m_fLastInternalDewPoint = fIndoor;
            bSetDone = true;
        }
        if(m_fLastExternalDewPoint != fOutdoor) {
            m_fLastExternalDewPoint = fOutdoor;
            bSetDone = true;
        }
    }
    return(bSetDone);
}


/// @brief Dispatch and update if needed
void CDewPointSwitch::dispatch() {
    float fIndoor   = NAN;
    float fOutdoor  = NAN;
    if(pSID) fIndoor  = pSID->getDewPoint();
    if(pSOD) fOutdoor = pSOD->getDewPoint();
    
    if(m_oUpdateDelay.isDone()) {
        if(this->setNewDewpointState(fIndoor,fOutdoor)) {
            /// -1 = do nothing, 0 = switch off, 1 = switch on
            int nActivation = -1;    

            // Indoor - Outdoor => positive = indoor hum is higher then outdoor...
            float fDewPointDelta = fIndoor - fOutdoor;

            // Check if deactivation point is reached... if not, power off...
            if(fDewPointDelta < Config.dewPointActivationDelta) nActivation = 0;

            // Or is the delta below the Threshold plus hysteresis ?
            if((fDewPointDelta ) > (Config.dewPointActivationDelta + Config.activationHysteresis)) nActivation = 1;

            // Min Temp thresholds forces off, if lower...
            if(pSOD->getTemperature() < Config.minActivationLevelOutdoor) nActivation = 0;
            if(pSID->getTemperature() < Config.minActivationLevelIndoor) nActivation = 0;

            DEBUG_INFO("DewPointSwitch infos:");
            DEBUG_INFO("============> Indoor\t  Outdoor");
            DEBUG_INFOS(" - temp     : %f \t- %f", pSID->Status.TempC, pSOD->Status.TempC);
            DEBUG_INFOS(" - humidity : %f \t- %f", pSID->Status.Humidity, pSOD->Status.Humidity);
            DEBUG_INFOS(" - dewpoints: %f \t- %f", fIndoor, fOutdoor);
            DEBUG_INFOS(" - actDelta : %f",Config.dewPointActivationDelta);
            DEBUG_INFOS(" - hyster...: %f",Config.activationHysteresis);
            DEBUG_INFOS(" - dew delta: %f", fDewPointDelta);
            DEBUG_INFOS(" - activate : %d",nActivation);
            DEBUG_INFOS(" - timestamp: %s",Appl.getISODateTime());

            if(nActivation > -1) {
                Appl.MsgBus.sendEvent(
                                this,
                                nActivation == 1 ? MSG_FAN_ACTIVATE : MSG_FAN_DISABLE,
                                nullptr,
                                nActivation
                            );
            }
            
        }
        m_oUpdateDelay.restart();         
    }
}
