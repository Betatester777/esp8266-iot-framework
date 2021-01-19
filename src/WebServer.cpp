#include <WebServer.h>

std::queue<std::function<void()>> serverConnectionTestQueue;

void WebServer::begin()
{
    //to enable testing and debugging of the interface
    DefaultHeaders::Instance().addHeader(PSTR("Access-Control-Allow-Origin"), PSTR("*"));

    server = new AsyncWebServer(80);
    ws = new AsyncWebSocket("/ws");

    ws->onEvent(std::bind(&WebServer::onWebSocketEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));

    //ws.onEvent(std::bind(&onEvent));
    server->addHandler(ws);
    server->begin();

    server->serveStatic("/download", LittleFS, "/");

    //handle uploads
    server->on(
        PSTR("/upload"), HTTP_POST, [](AsyncWebServerRequest *request) {}, handleFileUpload);
    bindAll();
}

void WebServer::handleWebSocketMessage(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        data[len] = 0;
        std::string stringData = (char *)data;

        DynamicJsonDocument doc(1024);

        // You can use a String as your JSON input.
        // WARNING: the string in the input  will be duplicated in the JsonDocument.

        deserializeJson(doc, stringData);
        JsonObject obj = doc.as<JsonObject>();

        // You can use a String to get an element of a JsonObject
        // No duplication is done.
        String command = obj[String("command")].as<String>();

        if (command == "ping")
        {
            String param = obj[String("value")].as<String>();
            client->text("{\"type\":\"pong\",\"value\":\"" + param + "\"}");
        }
        else if (command == "get_status")
        {
            client->text(status2json());
        }
        else if (command == "set_operation_mode")
        {
            uint8_t newOperationMode = obj[String("operationMode")].as<uint8_t>();

            Serial.println("command: " + command + ", operationMode=" + String(newOperationMode));
            switch (newOperationMode)
            {
            case OPERATION_MODE_MANUAL:
                fsmOperationMode->trigger(TRIGGER_OPERATION_MODE_MANUAL);
                smaModbusSlave->resetTimer(configManager.server.measureInterval * 1000);
                break;
            case OPERATION_MODE_POWER:
                fsmOperationMode->trigger(TRIGGER_OPERATION_MODE_POWER);
                smaModbusSlave->resetTimer(configManager.server.measureInterval * 1000);
                break;
            }
        }
        else if (command == "set_output_status")
        {
            uint8_t newOutputStatus = obj[String("outputStatus")].as<uint8_t>();

            Serial.println("command: " + command + ", outputStatus=" + String(newOutputStatus));
            if (newOutputStatus == OUTPUT_ON)
            {
                fsmOperationMode->trigger(TRIGGER_ON);
            }
            else if (newOutputStatus == OUTPUT_OFF)
            {
                fsmOperationMode->trigger(TRIGGER_OFF);
            }
        }
    }
}

void WebServer::onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        client->text(status2json());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA:
        handleWebSocketMessage(client, arg, data, len);
        break;
    case WS_EVT_PONG:

    case WS_EVT_ERROR:
        break;
    }
}

String WebServer::status2json()
{
    String JSON;
    StaticJsonDocument<1024> jsonBuffer;
    jsonBuffer["type"] = "status";
    jsonBuffer["deviceName"] = configManager.getDeviceName();
    jsonBuffer["legal"] = (uint8_t)configManager.legal.isComplete;
    jsonBuffer["wifi"] = (uint8_t)configManager.wifi.isComplete;
    jsonBuffer["wifi_test"] = (uint8_t)configManager.wifi_test.isComplete;
    jsonBuffer["time"] = (uint8_t)configManager.time.isComplete;
    jsonBuffer["server"] = (uint8_t)configManager.server.isComplete;
    jsonBuffer["server_test"] = (uint8_t)configManager.server_test.isComplete;
    jsonBuffer["timer"] = (uint8_t)configManager.timer.isComplete;
    jsonBuffer["settings"] = (uint8_t)configManager.settings.isComplete;
    jsonBuffer["operationMode"] = configManager.settings.operationMode;
    jsonBuffer["measuredPower"] = measuredPower;
    jsonBuffer["outputStatus"] = outputStatus;

    serializeJson(jsonBuffer, JSON);
    //Serial.println(JSON);
    return JSON;
}

void WebServer::publishStatus()
{
    String status = status2json();
    Serial.println("Publish new status:" + status);
    ws->textAll(status);
}

void WebServer::bindAll()
{
    //Restart the ESP
    server->on(PSTR("/api/restart"), HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200, PSTR("text/html"), ""); //respond first because of restart
        ESP.restart();
    });

    //update WiFi details
    server->on(PSTR("/api/reset/factory"), HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Factory reset...");
        request->send(200, PSTR("text/html"), ""); //respond first because of wifi change
        configManager.reset(SCOPE_LEGAL | SCOPE_WIFI | SCOPE_WIFI_TEST | SCOPE_SERVER | SCOPE_SERVER_TEST | SCOPE_TIME | SCOPE_TIMER | SCOPE_SETTINGS);
        wifiManager.forget();
    });

    //get file listing
    server->on(PSTR("/api/files/get"), HTTP_GET, [](AsyncWebServerRequest *request) {
        String JSON;
        StaticJsonDocument<1000> jsonBuffer;
        JsonArray files = jsonBuffer.createNestedArray("files");

        //get file listing
        Dir dir = LittleFS.openDir("");
        while (dir.next())
            files.add(dir.fileName());

        //get used and total data
        FSInfo fs_info;
        LittleFS.info(fs_info);
        jsonBuffer["used"] = String(fs_info.usedBytes);
        jsonBuffer["max"] = String(fs_info.totalBytes);

        serializeJson(jsonBuffer, JSON);

        request->send(200, PSTR("text/html"), JSON);
    });

    //remove file
    server->on(PSTR("/api/files/remove"), HTTP_POST, [](AsyncWebServerRequest *request) {
        LittleFS.remove("/" + request->arg("filename"));
        request->send(200, PSTR("text/html"), "");
    });

    //update from LittleFS
    server->on(PSTR("/api/update"), HTTP_POST, [](AsyncWebServerRequest *request) {
        updater.requestStart("/" + request->arg("filename"));
        request->send(200, PSTR("text/html"), "");
    });

    //update status
    server->on(PSTR("/api/update-status"), HTTP_GET, [](AsyncWebServerRequest *request) {
        String JSON;
        StaticJsonDocument<200> jsonBuffer;

        jsonBuffer["status"] = updater.getStatus();
        serializeJson(jsonBuffer, JSON);

        request->send(200, PSTR("text/html"), JSON);
    });

    server->on(PSTR("/api/setup/legal/get"), HTTP_GET, [this](AsyncWebServerRequest *request) {
        String JSON = configManager.getJSONString(SCOPE_LEGAL);
        Serial.println("Get legal setup:" + JSON);
        request->send(200, PSTR("'application/json'"), JSON);
    });

    server->on(PSTR("/api/setup/legal/set"), HTTP_POST,
               [this](AsyncWebServerRequest *request) {
                   configManager.setJSONString(SCOPE_LEGAL, request->arg("data"));
                   Serial.println("save legal success");
                   request->send(200, PSTR("'text/html'"), "OK");
               });

    server->on(PSTR("/api/setup/wifi/get"), HTTP_GET, [this](AsyncWebServerRequest *request) {
        String JSON = configManager.getJSONString(SCOPE_WIFI);
        Serial.println("Get wifi setup:" + JSON);
        request->send(200, PSTR("'application/json'"), JSON);
    });

    server->on(PSTR("/api/setup/wifi/networks/get"), HTTP_GET, [this](AsyncWebServerRequest *request) {
        String JSON = wifiManager.scanNetworks();
        request->send(200, PSTR("'application/json'"), JSON);
    });

    server->on(PSTR("/api/setup/wifi/set"), HTTP_POST, [this](AsyncWebServerRequest *request) {
        configManager.setJSONString(SCOPE_WIFI, request->arg("data"));
        Serial.println("save wifi success");
        request->send(200, PSTR("'text/html'"), "OK");
    });

    server->on(PSTR("/api/setup/wifi_test/get"), HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(200, PSTR("'application/json'"), configManager.getJSONString(SCOPE_WIFI_TEST));
    });

    server->on(PSTR("/api/setup/wifi_test/set"), HTTP_POST,
               [this](AsyncWebServerRequest *request) {
                   configManager.setJSONString(SCOPE_WIFI_TEST, request->arg("data"));
                   request->send(200, PSTR("'text/html'"), "OK");

                   wifiManager.setNewWifi(configManager.setupSsid,
                                          configManager.setupPassword,
                                          configManager.wifi.useDHCP,
                                          configManager.wifi.fixedIp,
                                          configManager.wifi.subnetMask,
                                          configManager.wifi.gatewayIp,
                                          configManager.wifi.dnsServerIp);
               });

    server->on(PSTR("/api/setup/server/get"), HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(200, PSTR("'application/json'"), configManager.getJSONString(SCOPE_SERVER));
    });

    server->on(PSTR("/api/setup/server/set"), HTTP_POST,
               [this](AsyncWebServerRequest *request) {
                   configManager.setJSONString(SCOPE_SERVER, request->arg("data"));
                   Serial.println("save server setup success");
                   if (configManager.server.isComplete)
                   {
                       request->send(200, PSTR("'text/html'"), "OK");
                   }
                   else
                   {
                       request->send(200, PSTR("'text/html'"), "ERROR");
                   }
               });

    server->on(PSTR("/api/setup/server_test/get"), HTTP_GET, [this](AsyncWebServerRequest *request) {
        configManager.testConnectionResult = "{}";
        configManager.testConnection = true;
        serverConnectionTestQueue.push(std::bind(
            [this](AsyncWebServerRequest *request) {
                request->send(200, PSTR("'application/json'"), configManager.testConnectionResult);
            },
            request));
    });

    server->on(PSTR("/api/setup/server_test/set"), HTTP_POST,
               [this](AsyncWebServerRequest *request) {
                   configManager.setJSONString(SCOPE_SERVER_TEST, request->arg("data"));
                   Serial.println("save server test setup success");
                   if (configManager.server.isComplete)
                   {
                       request->send(200, PSTR("'text/html'"), "OK");
                   }
                   else
                   {
                       request->send(200, PSTR("'text/html'"), "ERROR");
                   }
               });

    server->on(PSTR("/api/setup/previous_scope/set"), HTTP_POST,
               [this](AsyncWebServerRequest *request) {
                   configManager.setPreviousSetupScope();
                   Serial.println("previous setup scope success");
                   request->send(200, PSTR("'text/html'"), "OK");
               });

    server->on(PSTR("/api/setup/settings/get"), HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(200, PSTR("'application/json'"), configManager.getJSONString(SCOPE_SETTINGS));
    });

    server->on(PSTR("/api/setup/settings/set"), HTTP_POST,
               [this](AsyncWebServerRequest *request) {
                   configManager.setJSONString(SCOPE_SETTINGS, request->arg("data"));
                   Serial.println("save server setup success");
                   if (configManager.settings.isComplete)
                   {
                       request->send(200, PSTR("'text/html'"), "OK");
                   }
                   else
                   {
                       request->send(200, PSTR("'text/html'"), "ERROR");
                   }
               });

    server->on(PSTR("/api/setup/complete/get"), HTTP_GET, [this](AsyncWebServerRequest *request) {
        String JSON = "{}";
        request->send(200, PSTR("'application/json'"), JSON); });

    //send binary configuration data
    server->on(PSTR("/api/status/get"), HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(200, PSTR("'application/json'"), status2json());
    });

    server->on(PSTR("/api/status/operation_mode/power"), HTTP_GET,
               [this](AsyncWebServerRequest *request) {
                   configManager.settings.operationMode = OPERATION_MODE_POWER;
                   configManager.save(SCOPE_SETTINGS);
                   fsmOperationMode->trigger(TRIGGER_OPERATION_MODE_POWER);
                   Serial.println("change operation mode: POWER");

                   request->send(200, PSTR("'text/html'"), "OK");
               });

    server->on(PSTR("/api/status/operation_mode/manual"), HTTP_GET,
               [this](AsyncWebServerRequest *request) {
                   configManager.settings.operationMode = OPERATION_MODE_MANUAL;
                   configManager.save(SCOPE_SETTINGS);
                   fsmOperationMode->trigger(TRIGGER_OPERATION_MODE_MANUAL);
                   Serial.println("change operation mode: MANUAL");
                   request->send(200, PSTR("'text/html'"), "OK");
               });

    server->on(PSTR("/api/status/output/on"), HTTP_GET,
               [this](AsyncWebServerRequest *request) {
                   fsmOperationMode->trigger(TRIGGER_ON);
                   request->send(200, PSTR("'text/html'"), "OK");
               });

    server->on(PSTR("/api/status/output/off"), HTTP_GET,
               [this](AsyncWebServerRequest *request) {
                   fsmOperationMode->trigger(TRIGGER_OFF);
                   request->send(200, PSTR("'text/html'"), "OK");
               });

    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (wifiManager.isCaptivePortal() && request->host() != CAPTIVE_PORTAL_IP && request->host() != WiFi.softAPIP().toString())
        {

            String redirectAddress = "http://" + String(CAPTIVE_PORTAL_IP);
            Serial.println("Redirect to captive portal [url=" + redirectAddress + "]");

            AsyncWebServerResponse *response = request->beginResponse(307);
            response->addHeader("X-Frame-Options", "deny");
            response->addHeader("Cache-Control", "no-cache");
            response->addHeader("Pragma", "no-cache");
            response->addHeader("Location", redirectAddress);
            request->send(response);
        }
        else
        {
            AsyncWebServerResponse *response = request->beginResponse_P(200, PSTR("text/html"), html, html_len);

            // Tell the browswer the content is Gzipped
            response->addHeader(PSTR("Content-Encoding"), PSTR("gzip"));

            request->send(response);
        }
    });

    server->onNotFound([](AsyncWebServerRequest *request) {
        if (wifiManager.isCaptivePortal() && request->host() != CAPTIVE_PORTAL_IP && request->host() != WiFi.softAPIP().toString())
        {
            String redirectAddress = "http://" + String(CAPTIVE_PORTAL_IP);
            Serial.println("Notfound: Redirect to captive portal [url=" + redirectAddress + "]");

            AsyncWebServerResponse *response = request->beginResponse(307);
            response->addHeader("X-Frame-Options", "deny");
            response->addHeader("Cache-Control", "no-cache");
            response->addHeader("Pragma", "no-cache");
            response->addHeader("Location", redirectAddress);
            request->send(response);
        }
        else
        {
            Serial.println("Notfound: Redirect web-server root");
            AsyncWebServerResponse *response = request->beginResponse_P(200, PSTR("text/html"), html, html_len);

            // Tell the browswer the content is Gzipped
            response->addHeader(PSTR("Content-Encoding"), PSTR("gzip"));

            request->send(response);
        }
    });
}

void WebServer::handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    static File fsUploadFile;

    if (!index)
    {
        Serial.println(PSTR("Start file upload"));
        Serial.println(filename);

        if (!filename.startsWith("/"))
            filename = "/" + filename;

        fsUploadFile = LittleFS.open(filename, "w");
    }

    for (size_t i = 0; i < len; i++)
    {
        fsUploadFile.write(data[i]);
    }

    if (final)
    {
        String JSON;
        StaticJsonDocument<100> jsonBuffer;

        jsonBuffer["success"] = fsUploadFile.isFile();
        serializeJson(jsonBuffer, JSON);

        request->send(200, PSTR("text/html"), JSON);
        fsUploadFile.close();
    }
}

WebServer GUI;