#pragma once

#include <ConfigHandler.h>
#include <StatusHandler.h>
#include <DewSensor.h>


struct AppConfig {
    float minActivationLevelIntern =  10.0; // Min internal temp to activate
    float minActivationLevelExtern = -10.0; // Min external temp to activate
    float dewPointActivationDelta  =   5.0; // Dewpoint activation Delta between intern/extern
    float activationHysteresis     =   1.0; // Hysteresis / Switch threshold temp
};

struct AppStatus {
    bool ActiveVentilation = false; // Ventilation is needed and activ
    bool isInErrorState    = false; // If a sensor is in error, the flag is true.
    String internalErrorMsg = "";
    String externalErrorMsg = "";
};

class CDewPointSwitch : public IConfigHandler, public IStatusHandler{
    public:
        AppStatus Status;
        AppConfig Config;
        // DewStatus *pIndoorStatus;
        // DewStatus *pOutdoorStatus;
        
        CDewSensor SID;
        CDewSensor SOD;

        CDewPointSwitch(CDewSensor &oSID,CDewSensor &oSOD) {
            SID = oSID;
            SOD = oSOD;
        }

        void dispatch();
        // Implement the Interfaces
        void writeStatusTo(JsonObject & oData) override;
        void readConfigFrom(JsonObject & oCfg) override;
        void writeConfigTo(JsonObject & oCfg, bool bHideCritical) override;

};
