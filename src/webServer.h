#ifndef __SERVER_H__
#define __SERVER_H__

#include <ESPAsyncWebServer.h>
#include "ArduinoJson.h"
#include "LittleFS.h"

// Include the header file we create with webpack
#include "generated/html.h"

//Access to other classes for GUI functions
#include "WiFiManager.h"
#include "configManager.h"

#include "updater.h"

#include "states.h"


class webServer
{

private:    
    AsyncWebServer server = AsyncWebServer(80);
    
    static void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
    static void serveProgmem(AsyncWebServerRequest *request);
    void bindAll();

public:
    AsyncWebSocket ws = AsyncWebSocket("/ws");
    void begin();
};

extern webServer GUI;

#endif
