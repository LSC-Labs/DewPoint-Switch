#ifndef DEBUG_LSC_WEBSOCKET
    #undef DEBUGINFOS
#endif
/**
 * WebRoutes.cpp
 * 
 * Is part of PLibESPV1 - CWebServer
 * 
 * Insert your application specific static pages here, also
 * as the defaults and the registration/handling of API's
 * 
 * You have to touch this file, to compile after changing the includes
 */


#include <WebServer.h>

#include <web/index.html.gz.h>
#include <web/app.css.gz.h>
#include <web/_runtime.css.gz.h>
#include <web/settings.js.gz.h>
#include <web/_runtime.js.gz.h>
#include <web/_pages.html.gz.h>
#include <web/_pages.js.gz.h>
#include <web/de.json.gz.h>
#include <web/en.json.gz.h>


void sendGzipResponse(AsyncWebServerRequest *pRequest, AsyncWebServerResponse *pResponse) {
    pResponse->addHeader("Content-Encoding", "gzip");
    pRequest->send(pResponse);
}
 
void registerWebRoutes(CWebServer &oWebServer) {
    DEBUG_FUNC_START();
    oWebServer.registerDefaults();
    oWebServer.registerFileAccess();
    
    oWebServer.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        DEBUG_INFOS("WS: request on : %s",request->url().c_str());
        sendGzipResponse(request,
                         request->beginResponse_P(
                            200,
                            "text/html",
                            index_html_gz, 
                            index_html_gz_len)
                        );
                        
	});

    oWebServer.on("/_pages.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        DEBUG_INFOS("WS: request on : %s",request->url().c_str());
        sendGzipResponse(request,
                         request->beginResponse_P(
                            200,
                            "text/html",
                            _pages_html_gz, 
                            _pages_html_gz_len)
                        );
	});

    oWebServer.on("/css/_runtime.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        DEBUG_INFOS("WS: request on : %s",request->url().c_str());
		sendGzipResponse(request,
                         request->beginResponse_P(
                            200,
                            F("text/css"),
                            _runtime_css_gz, 
                            _runtime_css_gz_len)
                        );
	});


    oWebServer.on("/css/app.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        DEBUG_INFOS("WS: request on : %s",request->url().c_str());
		sendGzipResponse(request,
                         request->beginResponse_P(
                            200,
                            F("text/css"),
                            app_css_gz, 
                            app_css_gz_len)
                        );
	});
    oWebServer.on("/js/settings.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        DEBUG_INFOS("WS: request on : %s",request->url().c_str());
        sendGzipResponse(request,
                         request->beginResponse_P(
                            200,
                            F("text/javascript"),
                            settings_js_gz, 
                            settings_js_gz_len)
                        );
	});

	oWebServer.on("/js/_runtime.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        DEBUG_INFOS("WS: request on : %s",request->url().c_str());
        sendGzipResponse(request,
                         request->beginResponse_P(
                            200,
                            F("text/javascript"),
                            _runtime_js_gz, 
                            _runtime_js_gz_len)
                        );
	});

	oWebServer.on("/js/_pages.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        DEBUG_INFOS("WS: request on : %s",request->url().c_str());
        sendGzipResponse(request,
                         request->beginResponse_P(
                            200,
                            F("text/javascript"),
                            _pages_js_gz, 
                            _pages_js_gz_len)
                        );
	});

    oWebServer.on("/i18n/de.json", HTTP_GET, [](AsyncWebServerRequest *request) {
        DEBUG_INFOS("WS: request on : %s",request->url().c_str());
        sendGzipResponse(request,
                         request->beginResponse_P(
                            200,
                            F("application/json"),
                            de_json_gz, 
                            de_json_gz_len)
                        );
	});
    oWebServer.on("/i18n/en.json", HTTP_GET, [](AsyncWebServerRequest *request) {
        DEBUG_INFOS("WS: request on : %s",request->url().c_str());
        sendGzipResponse(request,
                         request->beginResponse_P(
                            200,
                            F("application/json"),
                            en_json_gz, 
                            en_json_gz_len)
                        );
	});
    DEBUG_FUNC_END();
}
/* touched by page compiler : 1756115908015 */
/* touched by page compiler : 1756471628752 */
/* touched by page compiler : 1756480150845 */
/* touched by page compiler : 1756481381245 */
/* touched by page compiler : 1756566507046 */
/* touched by page compiler : 1756566550025 */
/* touched by page compiler : 1756567540824 */
/* touched by page compiler : 1756567705836 */
/* touched by page compiler : 1756568055334 */
/* touched by page compiler : 1756568143459 */
/* touched by page compiler : 1756568275697 */
/* touched by page compiler : 1756568311299 */
/* touched by page compiler : 1756568375717 */
/* touched by page compiler : 1756575593155 */
/* touched by page compiler : 1756575766213 */
/* touched by page compiler : 1756576193131 */
/* touched by page compiler : 1756576329866 */
/* touched by page compiler : 1756576441118 */
/* touched by page compiler : 1756576883617 */
/* touched by page compiler : 1756577020899 */
/* touched by page compiler : 1756577452660 */
