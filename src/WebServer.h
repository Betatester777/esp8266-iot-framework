#ifndef __SERVER_H__
#define __SERVER_H__
#include <ESPAsyncWebServer.h>
#include "ArduinoJson.h"
#include "LittleFS.h"
#include <SMA/SMAModbusSlave.h>

// Include the header file we create with webpack
#include "generated/html.h"

//Access to other classes for GUI functions
#include <WiFiManager.h>
#include <ConfigManager.h>
#include <Updater.h>
#include <queue>
class WebServer
{
private:
    static void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
    void bindAll();
    uint8_t desiredOutputStatus;
    String status2json();

protected:
    AsyncWebServer *server;
    AsyncWebSocket *ws;
    void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
    void handleWebSocketMessage(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len);

public:
    void begin();
    void publishStatus();
};

extern WebServer GUI;
extern std::queue<std::function<void()>> serverConnectionTestQueue;

#endif
