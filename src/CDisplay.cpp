#ifndef DEBUG_LSC_DISPLAY
    #undef DEBUGINFOS
#endif

#include <Appl.h>
#include <EventHandler.h>
#include <SimpleDelay.h>
#include <Adafruit_GFX.h>
#include <Display.h>
#include <DisplayPage.h>



class CDisplayCursor {
    public:
        uint16_t X = 0;
        uint16_t Y = 0;
        CDisplay *pCurDisplay;

        CDisplayCursor(CDisplay *pDisplay) { 
            if(pDisplay) {
                pCurDisplay = pDisplay;
                X = pDisplay->getCursorX();
                Y = pDisplay->getCursorY();
            }
        }

        void restore() {
            if(pCurDisplay) pCurDisplay->setCursor(X,Y);
        }
};



CDisplay::CDisplay() :
    DISPLAY_CONSTRUCTOR {
    // Adafruit code page error correction.
    // @see https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives
    cp437(true);
    Appl.MsgBus.registerEventReceiver(this);
    m_oRotateDelay.start(Config.RotateTime);
    m_oPowerOffDelay.start(Config.PowerOffTime);
    m_oPageRefreshDelay.start(Config.PageRefreshTime);
}

/**
 * Initialize the display...
 * wait about 300ms from power on... so display is up an running...
 */
void CDisplay::init(int nI2CAddress, bool bResetPin) {
    // millis is 0 when device is switched on
    while(millis() < 300) {
        begin(nI2CAddress,bResetPin);
    }
    // In Debug Mode - show start panel from vendor for a second...
    #ifdef DEBUGINFOS
        display();
        delay(200);
    #endif
    clearDisplay();
    // Activate the start page, if it is in place...
    activatePage(DISPLAY_START_PAGE);
}

int CDisplay::receiveEvent(const void * pSender, int nMsgType, const void * pMessage, int nClass) {
    return(EVENT_MSG_RESULT_OK);
} 

void CDisplay::readConfigFrom(JsonObject & oCfg) {

};
void CDisplay::writeConfigTo(JsonObject & oCfg, bool bHideCritical) {
    
};

void CDisplay::printAt(uint16_t nX, uint16_t nY, const char *pszString) {
    if(pszString) {
        setCursor(nX,nY);
        print(pszString);
    }
}
int CDisplay::setCenteredTextCursor(const char * pszText, int nStart, int nEnd) {
    if(pszText) {
        int16_t nPosY = getCursorY();
        uint16_t nTextWidth,nTextHeight;
        int16_t nX,nY;
        getTextBounds(pszText,0,nPosY,&nX,&nY,&nTextWidth,&nTextHeight);
        // If a range is in place, set the vars - either to screen width or to the range defs.
        int16_t nDisplayWidth = width();
        int16_t nStartRange = nStart > -1 ? nStart : 0;
        int16_t nEndRange = nEnd > -1 ? nEnd : nDisplayWidth;
        int16_t nRangeWidth = nEndRange - nStartRange;
        int16_t nOffsetX = 0;
        // Check boundaries... either out of bounds, then use the start point, or inside...
        if(nTextWidth >= nRangeWidth) nOffsetX = 0;
        else nOffsetX = (nRangeWidth - nTextWidth) / 2;
        setCursor(nStartRange + nOffsetX,nY);
    }
    return(getCursorY());
}

int CDisplay::printlnCentered(const char * pszText, int nStart, int nEnd) {
    DEBUG_FUNC_START_PARMS("%s",NULL_POINTER_STRING(pszText));
    if(pszText) {
        setCenteredTextCursor(pszText,nStart,nEnd);
        println(pszText);
    }
    DEBUG_FUNC_END();
    return(getCursorY());
}

int CDisplay::printCentered(const char * pszText, int nStart, int nEnd) {
    DEBUG_FUNC_START_PARMS("%s",NULL_POINTER_STRING(pszText));
    if(pszText) {
        setCenteredTextCursor(pszText,nStart,nEnd);
        print(pszText);
    }
    DEBUG_FUNC_END();
    return(getCursorY());
}

int CDisplay::printRightAligned(const char * pszText) {
    DEBUG_FUNC_START();
    if(pszText) {
        int16_t  nPosY = getCursorY();
        int16_t  nX,nY;
        uint16_t nWidth,nHeight;

        getTextBounds(pszText,0,nPosY,&nX,&nY,&nWidth,&nHeight);
        int16_t nDisplayWidth = width();
        nX = (nDisplayWidth - nWidth);
        if(nX < 0) nX = 0;
        setCursor(nX,nY);
        char szBuffer[strlen(pszText) + 1];
        sprintf(szBuffer,"%*s", strlen(pszText), "");
        println(pszText);
    }
    DEBUG_FUNC_END();
    return(getCursorY());
}


void CDisplay::registerPage(IDisplayPage * pDisplayPage) {
    if(pDisplayPage) {
        m_tPages.push_back(pDisplayPage);
    }
}

IDisplayPage * CDisplay::findPage(String &strName) {
    return(findPage(strName.c_str()));
}

void CDisplay::hideCurrentPage() {
    if(m_pCurrentPage != nullptr) m_pCurrentPage->hide(this);
}

void CDisplay::showCurrentPage() {
    DEBUG_FUNC_START();
    if(m_pCurrentPage != nullptr) {
        m_pCurrentPage->show(this);
        m_oPageRefreshDelay.restart();
    }
    DEBUG_FUNC_END();
}

IDisplayPage * CDisplay::findPage(const char *pszName) {
    DEBUG_FUNC_START_PARMS("%s",NULL_POINTER_STRING(pszName));
    IDisplayPage *pFoundPage = nullptr;
    if(pszName) {
        for(IDisplayPage *pPage : m_tPages) {
            if(pPage != nullptr) {
                    if(pPage->hasName(pszName)) { pFoundPage = pPage; break; }
            }
        }
    }
    DEBUG_FUNC_END_PARMS("found: %d", pFoundPage != nullptr);
    return(pFoundPage);
}

IDisplayPage * CDisplay::findNextPage(PageType eType) {
    IDisplayPage *pNextPage = nullptr;
    IDisplayPage *pCurPage = nullptr;
    IDisplayPage *pFirstPage = nullptr;
    /**
     * Next Page will be set when the current page is found.
     * If nothing is found, we  start from the first page again...
     */
    for(IDisplayPage *pPage : m_tPages) {
        // Ensure no nullptr and the correct requested page type.
        if(pPage != nullptr && (
                // Either the search is for ALL
                eType == PageType::ALL              || 
                // Or the type matches exactly
                pPage->getType() == eType           ||
                // Or it is an ALL page...
                pPage->getType() == PageType::ALL
            )) {
            // If no page is in place, the first one of type wins.
            if(m_pCurrentPage == nullptr)  { 
                pNextPage = pPage; break; 
            }

            // pCurPage is set - so take this one, cause we are in the next loop.
            if(pCurPage != nullptr) { pNextPage = pPage; break; }

            // Remember the first page, if nothing will be found
            if(pFirstPage == nullptr) pFirstPage = pPage;

            // Current Page found (nullptr check is above...)
            if(m_pCurrentPage == pPage) pCurPage = pPage;
        }
    }
    // If nothing was found, 
    return(pNextPage == nullptr ? pFirstPage : pNextPage);
}

bool CDisplay::activatePage(String &strName) {
    return(activatePage(strName.c_str()));
}

bool CDisplay::activatePage(const char *pszName) {
    DEBUG_FUNC_START_PARMS("%s",pszName);
    bool bIsActivated = m_strCurrentPageName == pszName;
    if(!bIsActivated) {
        IDisplayPage *pNewPage = findPage(pszName);
        if(pNewPage != nullptr) {
            if(pNewPage != m_pCurrentPage ){
                hideCurrentPage();
                m_pCurrentPage = pNewPage;
                bIsActivated = true;
            } else {
                bIsActivated = true;
            }
        }
        showCurrentPage();
    }
    refreshCurrentPage();
    DEBUG_FUNC_END_PARMS("%d",bIsActivated);
    return(bIsActivated);
}

bool CDisplay::activateNextPage(PageType eType) {
    bool bActivated = false;
    IDisplayPage *pNextPage = findNextPage(eType);
    if(pNextPage != nullptr) {
        if(pNextPage != m_pCurrentPage) {
            hideCurrentPage();
            m_pCurrentPage = pNextPage;
            showCurrentPage();
            bActivated = true;
        }
    }
    refreshCurrentPage();
    return(bActivated);
}

void CDisplay::refreshCurrentPage(bool bForce) {
    if(bForce || m_oPageRefreshDelay.isDone()) {
        showCurrentPage();
        m_oPageRefreshDelay.restart();
    }
}
       

/**
 * Dispatcher, will be called in each loop...
 */
void CDisplay::dispatch() {
    // Power off configured and reached ?
    if(!Config.AlwaysOn && !m_bNoRotateClearedActive) {
        if(m_oPowerOffDelay.isDone()) {
            this->clearDisplay();
            m_bNoRotateClearedActive = true;
        }
    } else if(Config.RotateDisplays) {
        if(m_oRotateDelay.isDone()) {
            DEBUG_INFO("## -> activate next page of type LOOP");
            activateNextPage(PageType::LOOP);
            m_oRotateDelay.restart();
        }
        refreshCurrentPage();
    } else activatePage(Config.StaticDisplayName);
}