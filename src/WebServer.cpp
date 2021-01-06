#include <WebServer.h>

std::queue<std::function<void()>> serverConnectionTestQueue;

String WebServer::status2json()
{
    String JSON;
    StaticJsonDocument<1024> jsonBuffer;
    jsonBuffer["operationMode"] = configManager.settings.operationMode;
    jsonBuffer["measuredPower"] = measuredPower;
    jsonBuffer["outputStatus"] = outputStatus;
    serializeJson(jsonBuffer, JSON);
    //Serial.println(JSON);
    return JSON;
}

void WebServer::begin()
{
    //to enable testing and debugging of the interface
    DefaultHeaders::Instance().addHeader(PSTR("Access-Control-Allow-Origin"), PSTR("*"));

    server.addHandler(&ws);
    server.begin();

    server.serveStatic("/download", LittleFS, "/");

    server.onNotFound(serveProgmem);

    //handle uploads
    server.on(
        PSTR("/upload"), HTTP_POST, [](AsyncWebServerRequest *request) {}, handleFileUpload);
    bindAll();
}

void WebServer::bindAll()
{
    //Restart the ESP
    server.on(PSTR("/api/restart"), HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200, PSTR("text/html"), ""); //respond first because of restart
        ESP.restart();
    });

    //update WiFi details
    server.on(PSTR("/api/reset/factory"), HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Factory reset...");
        request->send(200, PSTR("text/html"), ""); //respond first because of wifi change
        configManager.reset(SCOPE_LEGAL | SCOPE_WIFI | SCOPE_SERVER | SCOPE_SERVER_TEST | SCOPE_TIME | SCOPE_TIMER | SCOPE_SETTINGS);
        wifiManager.forget();
    });

    //get file listing
    server.on(PSTR("/api/files/get"), HTTP_GET, [](AsyncWebServerRequest *request) {
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
    server.on(PSTR("/api/files/remove"), HTTP_POST, [](AsyncWebServerRequest *request) {
        LittleFS.remove("/" + request->arg("filename"));
        request->send(200, PSTR("text/html"), "");
    });

    //update from LittleFS
    server.on(PSTR("/api/update"), HTTP_POST, [](AsyncWebServerRequest *request) {
        updater.requestStart("/" + request->arg("filename"));
        request->send(200, PSTR("text/html"), "");
    });

    //update status
    server.on(PSTR("/api/update-status"), HTTP_GET, [](AsyncWebServerRequest *request) {
        String JSON;
        StaticJsonDocument<200> jsonBuffer;

        jsonBuffer["status"] = updater.getStatus();
        serializeJson(jsonBuffer, JSON);

        request->send(200, PSTR("text/html"), JSON);
    });

    server.on(PSTR("/api/setup/status/get"), HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(200, PSTR("'application/json'"), configManager.getSetupStatusJSONString());
    });

    server.on(PSTR("/api/setup/legal/get"), HTTP_GET, [this](AsyncWebServerRequest *request) {
        String JSON = configManager.getJSONString(SCOPE_LEGAL);
        Serial.println("Get legal setup:" + JSON);
        request->send(200, PSTR("'application/json'"), JSON);
    });

    server.on(PSTR("/api/setup/legal/set"), HTTP_POST,
              [this](AsyncWebServerRequest *request) {
                  configManager.setJSONString(SCOPE_LEGAL, request->arg("data"));
                  Serial.println("save legal success");
                  request->send(200, PSTR("'text/html'"), "OK");
              });

    server.on(PSTR("/api/setup/wifi/get"), HTTP_GET, [this](AsyncWebServerRequest *request) {
        String JSON = configManager.getJSONString(SCOPE_WIFI);
        Serial.println("Get wifi setup:" + JSON);
        request->send(200, PSTR("'application/json'"), JSON);
    });

    server.on(PSTR("/api/setup/wifi/networks/get"), HTTP_GET, [this](AsyncWebServerRequest *request) {
        String JSON = wifiManager.scanNetworks();
        request->send(200, PSTR("'application/json'"), JSON);
    });

    server.on(PSTR("/api/setup/wifi/set"), HTTP_POST, [this](AsyncWebServerRequest *request) {
        configManager.setJSONString(SCOPE_WIFI, request->arg("data"));
        Serial.println("save wifi success");
        request->send(200, PSTR("'text/html'"), "OK");
        wifiManager.setNewWifi(configManager.setupSsid,
                               configManager.setupPassword,
                               configManager.wifi.useDHCP,
                               configManager.wifi.fixedIp,
                               configManager.wifi.subnetMask,
                               configManager.wifi.gatewayIp,
                               configManager.wifi.dnsServerIp);
    });

    server.on(PSTR("/api/setup/server/get"), HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(200, PSTR("'application/json'"), configManager.getJSONString(SCOPE_SERVER));
    });

    server.on(PSTR("/api/setup/server/set"), HTTP_POST,
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

    server.on(PSTR("/api/setup/server_test/get"), HTTP_GET, [this](AsyncWebServerRequest *request) {
        configManager.testConnectionResult = "{}";
        configManager.testConnection = true;
        serverConnectionTestQueue.push(std::bind(
            [this](AsyncWebServerRequest *request) {
                request->send(200, PSTR("'application/json'"), configManager.testConnectionResult);
            },
            request));
    });

    server.on(PSTR("/api/setup/server_test/set"), HTTP_POST,
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

    server.on(PSTR("/api/setup/settings/get"), HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(200, PSTR("'application/json'"), configManager.getJSONString(SCOPE_SETTINGS));
    });

    server.on(PSTR("/api/setup/settings/set"), HTTP_POST,
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

    server.on(PSTR("/api/setup/complete/get"), HTTP_GET, [this](AsyncWebServerRequest *request) {
        String JSON = "{}";
        request->send(200, PSTR("'application/json'"), JSON); });

    //send binary configuration data
    /*
    server.on(PSTR("/api/config/get"), HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(200, PSTR("'application/json'"), config2json());
    });

    //receive binary configuration data from body
    server.on(PSTR("/api/config/set"), HTTP_POST,
              [this](AsyncWebServerRequest *request) {
                  uint8_t lastOperationMode = configManager.settings.operationMode;


                  if (lastOperationMode != configManager.settings.operationMode)
                  {
                      fsmOperationMode->trigger(TRIGGER_CHANGE_OPERATION_MODE);
                  }
                  Serial.println("save config success");

                  config2json();
                  request->send(200, PSTR("'text/html'"), "OK");
              });
*/
    //send binary configuration data
    server.on(PSTR("/api/status/get"), HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(200, PSTR("'application/json'"), status2json());
    });

    server.on(PSTR("/api/status/operation_mode/power"), HTTP_GET,
              [this](AsyncWebServerRequest *request) {
                  configManager.settings.operationMode = OPERATION_MODE_POWER;
                  configManager.save(SCOPE_SETTINGS);
                  fsmOperationMode->trigger(TRIGGER_OPERATION_MODE_POWER);
                  Serial.println("change operation mode: POWER");

                  request->send(200, PSTR("'text/html'"), "OK");
              });

    server.on(PSTR("/api/status/operation_mode/manual"), HTTP_GET,
              [this](AsyncWebServerRequest *request) {
                  configManager.settings.operationMode = OPERATION_MODE_MANUAL;
                  configManager.save(SCOPE_SETTINGS);
                  fsmOperationMode->trigger(TRIGGER_OPERATION_MODE_MANUAL);
                  Serial.println("change operation mode: MANUAL");
                  request->send(200, PSTR("'text/html'"), "OK");
              });

    server.on(PSTR("/api/status/output/on"), HTTP_GET,
              [this](AsyncWebServerRequest *request) {
                  fsmOperationMode->trigger(TRIGGER_ON);
                  request->send(200, PSTR("'text/html'"), "OK");
              });

    server.on(PSTR("/api/status/output/off"), HTTP_GET,
              [this](AsyncWebServerRequest *request) {
                  fsmOperationMode->trigger(TRIGGER_OFF);
                  request->send(200, PSTR("'text/html'"), "OK");
              });
}

// Callback for the html
void WebServer::serveProgmem(AsyncWebServerRequest *request)
{
    // Dump the byte array in PROGMEM with a 200 HTTP code (OK)
    AsyncWebServerResponse *response = request->beginResponse_P(200, PSTR("text/html"), html, html_len);

    // Tell the browswer the content is Gzipped
    response->addHeader(PSTR("Content-Encoding"), PSTR("gzip"));

    request->send(response);
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