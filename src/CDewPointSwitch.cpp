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
        float fIndoor   = this->SID.getDewPoint();
        float fOutdoor  = this->SOD.getDewPoint();
        
    if(!isnan(fIndoor) && !isnan(fOutdoor)) {
        DEBUG_INFO("DewPointSwitch infos:");
        DEBUG_INFOS(" - temp     : %f \t- %f", SID.Status.TempC, SOD.Status.TempC);
        DEBUG_INFOS(" - humidity : %f \t- %f", SID.Status.Humidity, SOD.Status.Humidity);
        DEBUG_INFOS(" - dewpoint : %f \t- %f", fIndoor, fOutdoor);
    }
}