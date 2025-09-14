#pragma once

#include <EventHandler.h>
#include <Display.h>
#include <DisplayPage.h>
#include <DisplayBasePage.h>


class CDisplayTestPage : public CDisplayPage {
    public:
        CDisplayTestPage(const char *pszName) : CDisplayPage(pszName,PageType::SETUP) {}
        virtual void show(CDisplay *pDisplay) override;
    
};

class CDisplayLogoPage : public CDisplayPage {
    public:
        CDisplayLogoPage(const char *pszName) : CDisplayPage(pszName,PageType::SETUP,10000) {}
        virtual void show(CDisplay *pDisplay) override;
    
};

class CDisplayDewPointPage : public CDisplayBasePage {
    public:
        CDisplayDewPointPage(const char *pszName) : CDisplayBasePage(pszName,PageType::LOOP) {}
        virtual void show(CDisplay *pDisplay) override;
    
};

class CDisplayTempPage : public CDisplayBasePage {
    public:
        CDisplayTempPage(const char *pszName) : CDisplayBasePage(pszName,PageType::LOOP) {}
        virtual void show(CDisplay *pDisplay) override;
};

class CDisplayHumidityPage : public CDisplayBasePage {
    public:
        CDisplayHumidityPage(const char *pszName) : CDisplayBasePage(pszName,PageType::LOOP) {}
        virtual void show(CDisplay *pDisplay) override;
};

