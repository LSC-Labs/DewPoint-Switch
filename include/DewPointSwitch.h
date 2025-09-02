#pragma once

#include <ConfigHandler.h>
#include <StatusHandler.h>
#include <DewSensor.h>
#include <SimpleDelay.h>


struct AppConfig {
    float minActivationLevelIndoor =  10.0; // Min internal temp to activate
    float minActivationLevelOutdoor = -10.0; // Min external temp to activate
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
    private:
        CSimpleDelay m_oUpdateDelay; // Update every second
        float m_fLastInternalDewPoint = 0;
        float m_fLastExternalDewPoint = 0;
    public:
        AppStatus Status;
        AppConfig Config;
        
        CDewSensor *pSID;
        CDewSensor *pSOD;

        CDewPointSwitch(CDewSensor &oSID,CDewSensor &oSOD) {
            pSID = &oSID;
            pSOD = &oSOD;
            m_oUpdateDelay.start(1000);
        }

        void dispatch();
        // Implement the Interfaces
        void writeStatusTo(JsonObject & oData) override;
        void readConfigFrom(JsonObject & oCfg) override;
        void writeConfigTo(JsonObject & oCfg, bool bHideCritical) override;

};
