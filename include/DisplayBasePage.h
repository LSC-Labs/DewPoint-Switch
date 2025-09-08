#pragma once

#include <DisplayPage.h>
#include <EventHandler.h>
#include <DewSensor.h>

struct DisplayStatus {
    DewStatus *pSI              = nullptr;     // Status sensor indoor
    DewStatus *pSO              = nullptr;     // Status sensor outdoor
    bool isRebootPending        = false;
    bool isButtonPressed        = false;
    bool isInAccessPointMode    = false;
    bool isInStationMode        = false;
    bool isWiFiConnected        = false;
};

class CDisplayBasePage : public CDisplayPage, public IMsgEventReceiver {
    protected:
        DisplayStatus Status;

    public:
        CDisplayBasePage(const char * pszName, PageType eType) : CDisplayPage(pszName,eType){init();};
        CDisplayBasePage(String strName, PageType eType) : CDisplayPage(strName,eType){init();};
        int receiveEvent(const void * pSender, int nMsgType, const void * pMessage, int nClass) override;
        int drawPageHeader(CDisplay *pDisplay);

        void sprintfSensorDewPoint(char *pszBuffer,DewStatus *pStatus) {
            if(pszBuffer) {
                float fData = pStatus ? pStatus->DewPoint ? pStatus->DewPoint : NAN : NAN;
                if(isnan(fData)) strcpy(pszBuffer,"--.-");
                else sprintf(pszBuffer,"%2.1f",fData);
            }
        }
        void sprintfSensorTemp(char *pszBuffer,DewStatus *pStatus) {
            if(pszBuffer) {
                float fData = pStatus ? pStatus->TempC ? pStatus->TempC : NAN : NAN;
                if(isnan(fData)) sprintf(pszBuffer," --.- %cC",0xf8);
                else {
                    if(fData > -1) sprintf(pszBuffer," %2.1f %cC",fData,0xf8);
                    else sprintf(pszBuffer,"%2.1f %cC",fData,0xf8);
                }
            }
        }
        void sprintfSensorHumidity(char *pszBuffer,DewStatus *pStatus) {
            if(pszBuffer) {
                float fData = pStatus ? pStatus->Humidity ? pStatus->Humidity : NAN : NAN;
                if(isnan(fData)) strcpy(pszBuffer," -- %%");
                else {
                    if(fData > -1) sprintf(pszBuffer,"%2.f %%",fData);
                    else sprintf(pszBuffer,"%2.f %%",fData);
                }
            }
        }

        void printTemperature(CDisplay *pDisplay, float fTemp) {
            if(isnan(fTemp)) pDisplay->printf(" --.- %cC",0xf8);
            else {
                if(fTemp > -1) pDisplay->printf(" %2.1f %cC",fTemp,0xf8);
                else pDisplay->printf("%2.1f %cC",fTemp,0xf8);
            }
        }

        void printSensorTemperature(CDisplay *pDisplay,DewStatus *pStatus) {
            float fTemp = pStatus ? pStatus->TempC ? pStatus->TempC : NAN : NAN;
            printTemperature(pDisplay,fTemp);
        }

        void printSensorDewPoint(CDisplay *pDisplay,DewStatus *pStatus) {
            float fTemp = pStatus ? pStatus->DewPoint ? pStatus->DewPoint : NAN : NAN;
            if(isnan(fTemp)) pDisplay->printf("--.- %c",0xf8);
            else pDisplay->printf("%2.1f %c",fTemp,0xf8);
        }

        void printSensorHumidity(CDisplay *pDisplay,DewStatus *pStatus) {
            if(pDisplay) {
                float fValue = pStatus ? pStatus->Humidity ? pStatus->Humidity : NAN : NAN;
                if(isnan(fValue)) pDisplay->print("-- %");
                else pDisplay->printf("%2.f %%",fValue);
            }
        }
    protected:
        void init();
        
};
