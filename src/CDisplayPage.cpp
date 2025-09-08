
#ifndef DEBUG_LSC_DISPLAY_PAGES
    #undef DEBUGINFOS
#endif

#include <Arduino.h>
#include <DisplayPage.h>
#include <Msgs.h>
#include <Display.h>
#include <DevelopmentHelper.h>

CDisplayPage::CDisplayPage(String &strName,PageType eType) {
     m_strName = strName;  
     m_eType = eType; 
};
CDisplayPage::CDisplayPage(const char *pszName, PageType eType) { 
    m_strName = String(pszName);
    m_eType = eType; 
};
     

bool CDisplayPage::hasName(const char *pszName) {
    DEBUG_FUNC_START_PARMS("%s",NULL_POINTER_STRING(pszName));
    String strName(pszName);
    DEBUG_FUNC_END_PARMS("%d",strName == this->m_strName);
    return(strName == this->m_strName);
};

void CDisplayPage::show(CDisplay * pDisplay) {
    DEBUG_FUNC_START();
    DEBUG_FUNC_END();
}

void CDisplayPage::hide(CDisplay * pDisplay) {
    if(isActive && pDisplay) {
        pDisplay->clearDisplay();
        isActive = false;
    }
}
