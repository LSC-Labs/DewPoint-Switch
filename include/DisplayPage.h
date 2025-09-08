#pragma once
#include <Arduino.h>
#include <EventHandler.h>
#include <_DisplayHardware.h>

#ifndef DISPLAY_START_PAGE
    #define DISPLAY_START_PAGE "LOGO"
#endif

class CDisplay;

enum PageType {
    ALL = 0,
    SETUP = 1,
    LOOP  = 2
};

class IDisplayPage
{
    public:
        virtual PageType getType() = 0;
        virtual void show(CDisplay * pDisplay) = 0;
        virtual void hide(CDisplay * pDisplay) = 0;
        virtual bool hasName(const char * pszName) = 0;
}; 

class CDisplayPage : public IDisplayPage {

    protected:
        String      m_strName;
        PageType    m_eType;
        
    public:
        bool isActive = false;
        
    public:
        CDisplayPage(String &strName,PageType eType = PageType::LOOP);
        CDisplayPage(const char *pszName, PageType eType = PageType::LOOP);
        const char * getName() { return(m_strName.c_str()); };
        PageType getType() { return(m_eType); };
        bool hasName(const char *pszName);
        virtual void show(      CDisplay * pDisplay);
        virtual void hide(      CDisplay * pDisplay);
//         virtual int receiveEvent(const void * pSender, int nMsgType, const void * pMessage, int nClass) override;

    protected:
};