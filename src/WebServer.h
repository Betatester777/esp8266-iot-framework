#ifndef __SERVER_H__
#define __SERVER_H__

#include <ESPAsyncWebServer.h>
#include "ArduinoJson.h"
#include "LittleFS.h"

// Include the header file we create with webpack
#include "generated/html.h"

//Access to other classes for GUI functions
#include <WiFiManager.h>
#include <ConfigManager.h>
#include <Updater.h>
#include <States.h>

class WebServer
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

extern WebServer GUI;

#endif
