
/**
 * WebRoutes.cpp
 * 
 * Is part of PLibESPV1 - CWebServer
 * 
 * Insert your application specific static pages here, also
 * as the defaults and the registration/handling of API's
 * 
 * 
 */
#ifndef DEBUG_LSC_WEBROUTES
    #undef DEBUGINFOS
#endif

#include <WebServer.h>
#include <JsonHelper.h>
#include <web/index.html.gz.h>
#include <web/app.css.gz.h>
#include <web/settings.js.gz.h>
#include <web/_runtime.css.gz.h>
#include <web/_runtime.js.gz.h>
#include <web/_pages.html.gz.h>
#include <web/_pages.js.gz.h>
#include <web/de.json.gz.h>
#include <web/en.json.gz.h>


void sendGzipResponse(AsyncWebServerRequest *pRequest, AsyncWebServerResponse *pResponse) {
    DEBUG_FUNC_START();
    DEBUG_INFOS("WEB: %s",pRequest->url().c_str());
    pResponse->addHeader("Content-Encoding", "gzip");
    pRequest->send(pResponse);
    DEBUG_FUNC_END();
}

void sendJsonResponse(JsonDocument &oDoc, AsyncWebServerRequest *pRequest) {
    DEBUG_FUNC_START();
    DEBUG_INFOS("WEB: %s",pRequest->url().c_str());
    DEBUG_JSON_OBJ(oDoc);
    AsyncResponseStream *pStream =  pRequest->beginResponseStream(F("application/json"));
    serializeJson(oDoc, *pStream);
    pRequest->send(pStream);
    DEBUG_FUNC_END();
}



void registerWebRoutes(CWebServer &oWebServer) {
    oWebServer.registerDefaults();
    oWebServer.registerFileAccess();
    
    oWebServer.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        sendGzipResponse(request,
                         request->beginResponse_P(
                            200,
                            F("text/html"),
                            index_html_gz, 
                            index_html_gz_len)
                        );
	});

    oWebServer.on("/_pages.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        sendGzipResponse(request,
                         request->beginResponse_P(
                            200,
                            F("text/html"),
                            _pages_html_gz, 
                            _pages_html_gz_len)
                        );
	});

    oWebServer.on("/css/_runtime.css", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendGzipResponse(request,
                         request->beginResponse_P(
                            200,
                            F("text/css"),
                            _runtime_css_gz, 
                            _runtime_css_gz_len)
                        );
	});


    oWebServer.on("/css/app.css", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendGzipResponse(request,
                         request->beginResponse_P(
                            200,
                            F("text/css"),
                            app_css_gz, 
                            app_css_gz_len)
                        );
	});

    oWebServer.on("/js/settings.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        sendGzipResponse(request,
                         request->beginResponse_P(
                            200,
                            F("text/javascript"),
                            settings_js_gz, 
                            settings_js_gz_len)
                        );
	});


	oWebServer.on("/js/_runtime.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        sendGzipResponse(request,
                         request->beginResponse_P(
                            200,
                            F("text/javascript"),
                            _runtime_js_gz, 
                            _runtime_js_gz_len)
                        );
	});

	oWebServer.on("/js/_pages.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        sendGzipResponse(request,
                         request->beginResponse_P(
                            200,
                            F("text/javascript"),
                            _pages_js_gz, 
                            _pages_js_gz_len)
                        );
	});

    oWebServer.on("/i18n/de.json", HTTP_GET, [](AsyncWebServerRequest *request) {
        sendGzipResponse(request,
                         request->beginResponse_P(
                            200,
                            F("application/json"),
                            de_json_gz, 
                            de_json_gz_len)
                        );
	});
    oWebServer.on("/i18n/en.json", HTTP_GET, [](AsyncWebServerRequest *request) {
        sendGzipResponse(request,
                         request->beginResponse_P(
                            200,
                            F("application/json"),
                            en_json_gz, 
                            en_json_gz_len)
                        );
	});
    
    oWebServer.on("/apiV1/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        JSON_DOC(oStatus,4096);
        Appl.writeStatusTo(oStatus);
        sendJsonResponse(oStatus,request);
	});
    
    oWebServer.on("/apiV1/sysstatus", HTTP_GET, [](AsyncWebServerRequest *request) {
        JSON_DOC(oStatus,2048);
        Appl.writeSystemStatusTo(oStatus);
        sendJsonResponse(oStatus,request);
	});
}

/* touched by page compiler : 1758795107497 */
/* touched by page compiler : 1759502298483 */
/* touched by page compiler : 1759502447398 */
/* touched by page compiler : 1759502600615 */
/* touched by page compiler : 1759502619611 */
/* touched by page compiler : 1759502622190 */
/* touched by page compiler : 1759503003619 */
/* touched by page compiler : 1759503935921 */
/* touched by page compiler : 1759504043425 */
/* touched by page compiler : 1759504061998 */
/* touched by page compiler : 1759504587782 */
/* touched by page compiler : 1759504800949 */
/* touched by page compiler : 1759505025255 */
/* touched by page compiler : 1759505074891 */
/* touched by page compiler : 1759505134754 */
/* touched by page compiler : 1759505227494 */
/* touched by page compiler : 1759505678985 */
/* touched by page compiler : 1760080969825 */
/* touched by page compiler : 1760274639157 */
/* touched by page compiler : 1760274806496 */
/* touched by page compiler : 1760274885485 */
/* touched by page compiler : 1760274953983 */
/* touched by page compiler : 1760277135157 */
/* touched by page compiler : 1760277146149 */
/* touched by page compiler : 1760277152370 */
/* touched by page compiler : 1760277412861 */
/* touched by page compiler : 1760277552248 */
/* touched by page compiler : 1760277557023 */
/* touched by page compiler : 1760342544110 */
/* touched by page compiler : 1761222241267 */
/* touched by page compiler : 1761222429998 */
/* touched by page compiler : 1761227614162 */
/* touched by page compiler : 1761227888404 */
/* touched by page compiler : 1761227994277 */
/* touched by page compiler : 1761228122821 */
/* touched by page compiler : 1761228272147 */
/* touched by page compiler : 1761228477646 */
/* touched by page compiler : 1761228518052 */
/* touched by page compiler : 1761228546510 */
/* touched by page compiler : 1761228884834 */
/* touched by page compiler : 1761228901315 */
/* touched by page compiler : 1761228922604 */
/* touched by page compiler : 1761228954628 */
/* touched by page compiler : 1761228979482 */
/* touched by page compiler : 1761229088094 */
/* touched by page compiler : 1761229267247 */
/* touched by page compiler : 1761229380223 */
/* touched by page compiler : 1761229412351 */
/* touched by page compiler : 1761229552664 */
/* touched by page compiler : 1761230033522 */
/* touched by page compiler : 1761230708423 */
/* touched by page compiler : 1761301304088 */
/* touched by page compiler : 1761301874788 */
/* touched by page compiler : 1761302039690 */
/* touched by page compiler : 1761302076933 */
/* touched by page compiler : 1761302326107 */
/* touched by page compiler : 1761302344306 */
/* touched by page compiler : 1761302361906 */
/* touched by page compiler : 1761302665278 */
/* touched by page compiler : 1761303145333 */
/* touched by page compiler : 1761303533705 */
/* touched by page compiler : 1761303743274 */
/* touched by page compiler : 1761304209972 */
