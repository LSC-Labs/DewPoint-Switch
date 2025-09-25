#ifndef DEBUG_LSC_DISPLAY_PAGES
    #undef DEBUGINFOS
#endif

#include <DisplayPages.h>
#include <DevelopmentHelper.h>
#include <ProgVersion.h>
#include <Network.h>

void CDisplayAccessPointPage::show(CDisplay *pDisplay) {
  DEBUG_FUNC_START();
    if(pDisplay) {
        struct ip_info oIPInfo;
        wifi_get_ip_info(SOFTAP_IF, &oIPInfo);
		struct softap_config oConf;
		wifi_softap_get_config(&oConf);
        IPAddress ipaddr = IPAddress(oIPInfo.ip.addr);

        pDisplay->setTextColor(COLOR_WHITE);
        pDisplay->setCursor(0, 0);
        pDisplay->setTextSize(2);
        pDisplay->printlnCentered(APP_SHORTNAME);
        pDisplay->setTextSize(1);
        pDisplay->println("");
        pDisplay->printlnCentered("No WiFi...\n");
        pDisplay->printf("WiFi: %s\n",oConf.ssid );
        pDisplay->printf("http://%s\n",ipaddr.toString().c_str());

        pDisplay->display();
        this->isActive = true;
    }
    DEBUG_FUNC_END();
}

void CDisplayLogoPage::show(CDisplay *pDisplay) {
    DEBUG_FUNC_START();
    if(!this->isActive && pDisplay) {
        DEBUG_INFO("Activating LOGO display...");
        pDisplay->setTextColor(COLOR_WHITE);
        pDisplay->setCursor(0, 0);
        pDisplay->setTextSize(2);
        pDisplay->printlnCentered(APP_SHORTNAME);
        pDisplay->setTextSize(1);
        pDisplay->println("");
        pDisplay->printlnCentered("Version " APP_VERSION);
        pDisplay->println("");
        pDisplay->printlnCentered("(c) " APP_COPYRIGHT " " APP_COMPANY);
        pDisplay->printlnCentered(APP_AUTHOR);
        
        pDisplay->display();
        this->isActive = true;
    }
    DEBUG_FUNC_END();
}

void CDisplayDewPointPage::show(CDisplay *pDisplay) {
    DEBUG_FUNC_START();
    if(pDisplay) {
        DEBUG_INFO("Activating DewPoint display...");
        pDisplay->clearDisplay();
        drawPageHeader(pDisplay);
        int16_t nMiddle = pDisplay->width() / 2;
        int16_t nOffsetX = 6;
        int16_t nOffsetY = pDisplay->getCursorY() + 2;
        pDisplay->drawLine(nMiddle,pDisplay->getCursorY(),nMiddle,pDisplay->height(),COLOR_WHITE);
        pDisplay->printAt(nOffsetX +6 ,nOffsetY,"Indoor");
        pDisplay->printAt(nMiddle +6 + nOffsetX,nOffsetY,"Outdoor");
        nOffsetY += 12;
        if(Status.pSI && Status.pSO) {
            char szBuffer[80];
            pDisplay->setTextSize(1,2);
            pDisplay->setCursor(0,nOffsetY);
            // Show indoor / outdoor Dew Points
            sprintfSensorDewPoint(szBuffer,Status.pSI);
            pDisplay->printCentered(szBuffer,0,nMiddle);

            sprintfSensorDewPoint(szBuffer,Status.pSO);
            pDisplay->printCentered(szBuffer,nMiddle);
 
            nOffsetY += 22; 
            pDisplay->setTextSize(1);
            pDisplay->setCursor(nOffsetX,nOffsetY);
            printSensorTemperature(pDisplay,Status.pSI);
            pDisplay->setCursor(nMiddle + nOffsetX,nOffsetY);
            printSensorTemperature(pDisplay,Status.pSO);

            nOffsetY += 8;
            nOffsetX += 12;
            pDisplay->setCursor(nOffsetX,nOffsetY);
            sprintfSensorHumidity(szBuffer,Status.pSI);
            pDisplay->printCentered(szBuffer,0,nMiddle);
            sprintfSensorHumidity(szBuffer,Status.pSO);
            pDisplay->printCentered(szBuffer,nMiddle);

            /*
            printSensorHumidity(pDisplay,Status.pSI);
            pDisplay->setCursor(nMiddle + nOffsetX,nOffsetY);
            printSensorHumidity(pDisplay,Status.pSO);
            */
        }
        pDisplay->display();
        this->isActive = true;
    }
    DEBUG_FUNC_END();
      
}

void CDisplayTempPage::show(CDisplay *pDisplay) {
    DEBUG_FUNC_START();
    if(pDisplay) {
        DEBUG_INFO("Activating DewPoint display...");
        pDisplay->clearDisplay();
        drawPageHeader(pDisplay);
        int16_t nMiddle = pDisplay->width() / 2;
        int16_t nOffsetX = 6;
        int16_t nOffsetY = pDisplay->getCursorY() + 2;
        pDisplay->drawLine(nMiddle,pDisplay->getCursorY(),nMiddle,pDisplay->height(),COLOR_WHITE);
        pDisplay->printAt(nOffsetX +6 ,nOffsetY,"Indoor");
        pDisplay->printAt(nMiddle +6 + nOffsetX,nOffsetY,"Outdoor");
        nOffsetY += 12;
        if(Status.pSI && Status.pSO) {
            char szBuffer[80];
            pDisplay->setTextSize(1,2);
            pDisplay->setCursor(0,nOffsetY);
            // Show indoor / outdoor Dew Points
            sprintfSensorTemp(szBuffer,Status.pSI);
            pDisplay->printCentered(szBuffer,0,nMiddle);

            sprintfSensorTemp(szBuffer,Status.pSO);
            pDisplay->printCentered(szBuffer,nMiddle);
 
            nOffsetY += 22; 
            pDisplay->setTextSize(1);
            pDisplay->setCursor(nOffsetX,nOffsetY);
            strcpy(szBuffer,"DP ");
            sprintfSensorDewPoint(&szBuffer[3],Status.pSI);
            pDisplay->printCentered(szBuffer,0,nMiddle);
            sprintfSensorDewPoint(&szBuffer[3],Status.pSO);
            pDisplay->printCentered(szBuffer,nMiddle);

            nOffsetY += 8;
            nOffsetX += 12;
            pDisplay->setCursor(nOffsetX,nOffsetY);
            sprintfSensorHumidity(szBuffer,Status.pSI);
            pDisplay->printCentered(szBuffer,0,nMiddle);
            sprintfSensorHumidity(szBuffer,Status.pSO);
            pDisplay->printCentered(szBuffer,nMiddle);

            /*
            printSensorHumidity(pDisplay,Status.pSI);
            pDisplay->setCursor(nMiddle + nOffsetX,nOffsetY);
            printSensorHumidity(pDisplay,Status.pSO);
            */
        }
        pDisplay->display();
        this->isActive = true;
    }
    DEBUG_FUNC_END();
      
}

void CDisplayHumidityPage::show(CDisplay *pDisplay) {
    DEBUG_FUNC_START();
    if(pDisplay) {
        DEBUG_INFO("Activating DewPoint display...");
        pDisplay->clearDisplay();
        drawPageHeader(pDisplay);
        
        int16_t nMiddle = pDisplay->width() / 2;
        int16_t nOffsetX = 6;
        /*
        
        int16_t nOffsetY = pDisplay->getCursorY() + 2;
        pDisplay->drawLine(nMiddle,pDisplay->getCursorY(),nMiddle,pDisplay->height(),COLOR_WHITE);
        pDisplay->printAt(nOffsetX +6 ,nOffsetY,"Indoor");
        pDisplay->printAt(nMiddle +6 + nOffsetX,nOffsetY,"Outdoor");
        */
        int16_t nOffsetY = drawNumberFrame(pDisplay);
        if(Status.pSI && Status.pSO) {
            char szBuffer[80];

            pDisplay->setTextSize(1,2);
            pDisplay->setCursor(0,nOffsetY);
            // Show indoor / outdoor Dew Points
            sprintfSensorHumidity(szBuffer,Status.pSI);
            pDisplay->printCentered(szBuffer,0,nMiddle);

            sprintfSensorHumidity(szBuffer,Status.pSO);
            pDisplay->printCentered(szBuffer,nMiddle);
 
            nOffsetY += 22; 
            pDisplay->setTextSize(1);
            pDisplay->setCursor(nOffsetX,nOffsetY);
            strcpy(szBuffer,"DP ");
            sprintfSensorDewPoint(&szBuffer[3],Status.pSI);
            pDisplay->printCentered(szBuffer,0,nMiddle);
            sprintfSensorDewPoint(&szBuffer[3],Status.pSO);
            pDisplay->printCentered(szBuffer,nMiddle);

            nOffsetY += 8;
            nOffsetX += 12;
            pDisplay->setCursor(nOffsetX,nOffsetY);
            sprintfSensorTemp(szBuffer,Status.pSI);
            pDisplay->printCentered(szBuffer,0,nMiddle);
            sprintfSensorTemp(szBuffer,Status.pSO);
            pDisplay->printCentered(szBuffer,nMiddle);

            /*
            printSensorHumidity(pDisplay,Status.pSI);
            pDisplay->setCursor(nMiddle + nOffsetX,nOffsetY);
            printSensorHumidity(pDisplay,Status.pSO);
            */
        }
        pDisplay->display();
        this->isActive = true;
    }
    DEBUG_FUNC_END();
      
}

