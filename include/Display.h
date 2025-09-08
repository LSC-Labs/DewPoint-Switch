#pragma once
#include <functional>
#include <vector>
#include <SimpleDelay.h>
#include <ConfigHandler.h>
#include <EventHandler.h>
#include <DisplayPage.h>
#include <_DisplayHardware.h>
#include <Adafruit_GFX.h>

struct DisplayConfig {
    int  RotateTime     = 8000; // Time to display a page, when RotateDisplay is on.
    bool RotateDisplays = true; // Rotate the displays, or show a static page
    String StaticDisplayName;   // Static page name to display, if rotate is off
    bool AlwaysOn = true;       // Display stays active - otherways the screen is empty after power off time
    int  PowerOffTime = 10000;  // Power off time - screen becomes inactive.
    int  PageRefreshTime = 1000;// Refresh page every second... (for clock)    
};

class CDisplay :public IConfigHandler, public IMsgEventReceiver, public DISPLAY_CHIP_DRIVER_CLASS {

    private:
        std::vector<IDisplayPage*> m_tPages;
        IDisplayPage *m_pCurrentPage;
        String m_strCurrentPageName;
        DisplayConfig Config;
        CSimpleDelay m_oRotateDelay;
        CSimpleDelay m_oPowerOffDelay;
        CSimpleDelay m_oPageRefreshDelay;
        bool m_bNoRotateClearedActive = false;

    public: 
        CDisplay();
        void init(int nI2CAddress = 0x3c, bool bResetPin = true);
        void registerPage(IDisplayPage * pDisplayPage);
        void dispatch();

        IDisplayPage * findPage(String &strName);
        IDisplayPage * findPage(const char *pszName);
        IDisplayPage * findNextPage(PageType eType);
        bool activatePage(const char *pszName);
        bool activatePage(String &strName);
        bool activateNextPage(PageType eType);
        void hideCurrentPage();
        void showCurrentPage();
        void refreshCurrentPage(bool bForce = false);

        void readConfigFrom(JsonObject & oCfg) override;
        void writeConfigTo(JsonObject & oCfg, bool bHideCritical) override;
        int receiveEvent(const void * pSender, int nMsgType, const void * pMessage, int nClass) override;

        int  setCenteredTextCursor(const char *pszText, int nStart = -1, int nEnd = -1);
        int  printCentered(const char * pszText, int nStart = -1, int nEnd = -1);
        int  printlnCentered(const char * pszText, int nStart = -1, int nEnd = -1);
        int  printRightAligned(const char * pszText);
        void printAt(uint16_t nX, uint16_t nY, const char *pszText);

    protected:


};