#include <WebServer.h>

String WebServer::config2json(){
    String JSON;
    StaticJsonDocument<1024> jsonBuffer;

    jsonBuffer["useNTP"] = configManager.data.useNTP;
    jsonBuffer["operationMode"] = configManager.data.operationMode;
    jsonBuffer["serverProductId"] = configManager.data.serverProductId;
    jsonBuffer["serverIp"] = String(configManager.data.serverIp);
    jsonBuffer["serverPort"] = configManager.data.serverPort;
    jsonBuffer["powerThresholdHigh"] = configManager.data.powerThresholdHigh;
    jsonBuffer["powerThresholdLow"] = configManager.data.powerThresholdLow;
    jsonBuffer["measureInterval"] = configManager.data.measureInterval;
    jsonBuffer["enableStatusLED"] = configManager.data.enableStatusLED;
    serializeJson(jsonBuffer, JSON);
    Serial.println(JSON);
    return JSON;
}

void WebServer::json2config(String configData){
    Serial.println(configData);

    DynamicJsonDocument doc(1024);

    deserializeJson(doc, configData);
    JsonObject obj = doc.as<JsonObject>();

    configManager.data.useNTP= obj[String("useNTP")].as<uint8_t>();
    configManager.data.operationMode= obj[String("operationMode")].as<uint8_t>();
    configManager.data.serverProductId= obj[String("serverProductId")].as<uint16_t>();
    strcpy(configManager.data.serverIp, obj[String("serverIp")].as<String>().c_str());
    configManager.data.serverPort= obj[String("serverPort")].as<uint16_t>();
    configManager.data.powerThresholdHigh= obj[String("powerThresholdHigh")].as<uint32_t>();
    configManager.data.powerThresholdLow= obj[String("powerThresholdLow")].as<uint32_t>();
    configManager.data.measureInterval= obj[String("measureInterval")].as<uint32_t>();
    configManager.data.enableStatusLED= obj[String("enableStatusLED")].as<uint8_t>();
}

String WebServer::status2json(){
    String JSON;
    StaticJsonDocument<1024> jsonBuffer;

    jsonBuffer["operationMode"] = configManager.data.operationMode;
    jsonBuffer["measuredPower"] = measuredPower;
    jsonBuffer["outputStatus"] = outputStatus;
    serializeJson(jsonBuffer, JSON);
    Serial.println(JSON);
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
    server.on(PSTR("/api/wifi/set"), HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200, PSTR("text/html"), ""); //respond first because of wifi change
        WiFiManager.setNewWifi(request->arg("ssid"), request->arg("pass"));
    });

    //update WiFi details with static IP
    server.on(PSTR("/api/wifi/setStatic"), HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200, PSTR("text/html"), ""); //respond first because of wifi change
        WiFiManager.setNewWifi(request->arg("ssid"), request->arg("pass"), request->arg("ip"), request->arg("sub"), request->arg("gw"), request->arg("dns"));
    });

    //update WiFi details
    server.on(PSTR("/api/wifi/forget"), HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200, PSTR("text/html"), ""); //respond first because of wifi change
        WiFiManager.forget();
    });

    //get WiFi details
    server.on(PSTR("/api/wifi/get"), HTTP_GET, [](AsyncWebServerRequest *request) {
        String JSON;
        StaticJsonDocument<200> jsonBuffer;

        jsonBuffer["captivePortal"] = WiFiManager.isCaptivePortal();
        jsonBuffer["ssid"] = WiFiManager.SSID();
        serializeJson(jsonBuffer, JSON);

        request->send(200, PSTR("text/html"), JSON);
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

    //send binary configuration data
    server.on(PSTR("/api/config/get"), HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(200, PSTR("'application/json'"), config2json());
    });

    //receive binary configuration data from body
    server.on(PSTR("/api/config/set"), HTTP_POST,
        [this](AsyncWebServerRequest *request) {
        uint8_t lastOperationMode=configManager.data.operationMode;
        json2config(request->arg("data"));
        configManager.save();

        if(lastOperationMode!=configManager.data.operationMode){
            fsmOperationMode->trigger(TRIGGER_CHANGE_OPERATION_MODE);
        }
        Serial.println("save config success");      

        config2json();        
        request->send(200, PSTR("'text/html'"), "OK");
        }
    );

    //send binary configuration data
    server.on(PSTR("/api/status/get"), HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(200, PSTR("'application/json'"), status2json());
    });

    server.on(PSTR("/api/status/operation_mode/power"), HTTP_GET,
        [this](AsyncWebServerRequest *request) {
            configManager.data.operationMode=OPERATION_MODE_POWER;
            configManager.save();
            fsmOperationMode->trigger(TRIGGER_OPERATION_MODE_POWER);
            Serial.println("change operation mode: POWER"); 
            
            request->send(200, PSTR("'text/html'"), "OK");
        }
    );

    server.on(PSTR("/api/status/operation_mode/manual"), HTTP_GET,
        [this](AsyncWebServerRequest *request) {
            configManager.data.operationMode=OPERATION_MODE_MANUAL;
            configManager.save();
            fsmOperationMode->trigger(TRIGGER_OPERATION_MODE_MANUAL);
            Serial.println("change operation mode: MANUAL");             
            request->send(200, PSTR("'text/html'"), "OK");
        }
    );

    server.on(PSTR("/api/status/output/on"), HTTP_GET,
        [this](AsyncWebServerRequest *request) {
            fsmOperationMode->trigger(TRIGGER_ON);            
            request->send(200, PSTR("'text/html'"), "OK");
        }
    );

    server.on(PSTR("/api/status/output/off"), HTTP_GET,
        [this](AsyncWebServerRequest *request) {
            fsmOperationMode->trigger(TRIGGER_OFF);            
            request->send(200, PSTR("'text/html'"), "OK");
        }
    );
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