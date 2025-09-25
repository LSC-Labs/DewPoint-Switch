#ifndef DEBUG_LSC_DISPLAY
    #undef DEBUGINFOS
#endif

#include <Appl.h>
#include <EventHandler.h>
#include <SimpleDelay.h>
#include <Adafruit_GFX.h>
#include <Display.h>
#include <DisplayPage.h>
#include <LSCUtils.h>
#include <JsonHelper.h>
#include <AppActions.h>

#define CFG_ALWAYS_ON       F("alwaysOn")
#define CFG_STATIC_PAGE     F("staticPage")
#define CFG_ROTATE          F("rotate")
#define CFG_POWER_OFF_TIME  F("powerOff")
#define CFG_ROTATE_TIME     F("rotateTime")
  
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
    switch(nMsgType) {
        case MSG_DISPLAY_NEXT_PAGE: // Show next page (mostly by button)
            PageType eType = m_pCurrentPage ? m_pCurrentPage->getType() : PageType::LOOP;
            // this->activateNextPage(eType);
            break;
    }
    return(EVENT_MSG_RESULT_OK);
} 

void CDisplay::readConfigFrom(JsonObject & oCfg) {
    LSC::setValue(&Config.AlwaysOn,          oCfg[CFG_ALWAYS_ON]);
    LSC::setValue(Config.StaticDisplayName,  oCfg[CFG_STATIC_PAGE]);
    LSC::setValue(&Config.RotateDisplays,    oCfg[CFG_ROTATE]);
    LSC::setValue(&Config.RotateTime,        oCfg[CFG_ROTATE_TIME]);
    LSC::setValue(&Config.PowerOffTime,      oCfg[CFG_POWER_OFF_TIME]);
};

void CDisplay::writeConfigTo(JsonObject & oCfg, bool bHideCritical) {
    oCfg[CFG_ALWAYS_ON]         = Config.AlwaysOn;
    oCfg[CFG_STATIC_PAGE]       = Config.StaticDisplayName;
    oCfg[CFG_ROTATE]            = Config.RotateDisplays;
    oCfg[CFG_ROTATE_TIME]       = Config.RotateTime;
    oCfg[CFG_POWER_OFF_TIME]    = Config.PowerOffTime;
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

/**
 * Set the cursor to a right alignable text...
 * @param nEnd if -1, the screen width, otherwise, this is the reference point to align...
 * @return the new x cursor postion set
 */
int CDisplay::setRightAlignedTextCursor(const char * pszText, int nEnd) {
    DEBUG_FUNC_START_PARMS("%s,%d",NULL_POINTER_STRING(pszText),nEnd);
    int16_t  nPosY = getCursorY();
    int16_t  nX,nY = 0;
    uint16_t nTextWidth,nTextHeight;

    if(pszText) {
        getTextBounds(pszText,0,nPosY,&nX,&nY,&nTextWidth,&nTextHeight);
        int16_t nRangeWidth = nEnd < 0 ? width() : nEnd;
        nX = (nRangeWidth - nTextWidth);
        if(nX < 0) nX = 0;
        setCursor(nX,nY);
    } 
    DEBUG_FUNC_END_PARMS("%d",nX);
    return(nX);
}

int CDisplay::printRightAligned(const char * pszText, int nEndPos) {
    DEBUG_FUNC_START();
    if(pszText) {
        setRightAlignedTextCursor(pszText,nEndPos);
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

/**
 * Refresh the content of the page, respecting the refresh timer....
 */
void CDisplay::refreshCurrentPage(bool bForce) {
    if(m_pCurrentPage != nullptr) {
        if(bForce || m_oPageRefreshDelay.isDone()) {
            m_pCurrentPage->show(this);
            m_oPageRefreshDelay.restart();
            DEBUG_INFOS(" - next page refresh in : %lu ms",m_oPageRefreshDelay.getRemaining());
        }
    }
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

/// @brief Freeze the page (rotation stops, no actions until explicit a new page is selected)
/// @param bFreeze Default = true;
void CDisplay::freezePage(bool bFreeze) {
    m_bFreezePage = bFreeze;
}

bool CDisplay::activatePage(String &strName) {
    return(activatePage(strName.c_str()));
}


/// @brief activate page and show the content
/// @param pPage pointer to a page instance
/// @return true, if page was activated and shown, false, if pPage is null, or page was already active...
bool CDisplay::activatePage(IDisplayPage * pPage) {
    bool bIsActivated = false;
    if(pPage != nullptr) {
        if(pPage != m_pCurrentPage ){
            hideCurrentPage();
            m_pCurrentPage = pPage;
            m_oPageRefreshDelay.start(m_pCurrentPage->getRefreshTime());
            m_pCurrentPage->show(this);
            bIsActivated = true;
        } 
    }
    return(bIsActivated);
}

/// @brief activate a page by name
/// @param pszName Name of page that should be activated...
/// @return true, if the page was found and is active.
bool CDisplay::activatePage(const char *pszName) {
    DEBUG_FUNC_START_PARMS("%s",pszName);
    bool bIsActivated = m_strCurrentPageName == pszName;
    if(!bIsActivated) {
        IDisplayPage *pNewPage = findPage(pszName);
        if(pNewPage != nullptr) {
            bIsActivated = activatePage(pNewPage);
        }
    }
    // force refresh...
    DEBUG_FUNC_END_PARMS("%d",bIsActivated);
    return(bIsActivated);
}

/// @brief activates the next page of type in queue
/// @param eType type of page that will be respected.
/// @return  true, if the next page was found and could be activated
bool CDisplay::activateNextPage(PageType eType) {
    return(activatePage(findNextPage(eType)));
}




/**
 * Dispatcher, will be called in each loop...
 */
void CDisplay::dispatch() {
    // Power off configured and reached ?
    if(!m_bFreezePage) {
        if(!Config.AlwaysOn) {
            // Show the screen as long as poweroffdelay is still active
            // if already powered off, do nothing...
            if(!m_bPowerOffIsActive && m_oPowerOffDelay.isDone()) {
                this->clearDisplay();
                m_bPowerOffIsActive = true;
            }
        } else {
            // Screens should be displayed, either static or rotate mode...
            if(Config.RotateDisplays) {
                if(m_oRotateDelay.isDone()) {
                    DEBUG_INFO("## -> activate next page of type LOOP");
                    activateNextPage(PageType::LOOP);
                    m_oRotateDelay.restart();
                }
            }
            // Tell the page to refresh the content...
            refreshCurrentPage();
        } 
    }
}