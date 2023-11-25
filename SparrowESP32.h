#ifndef SPARROWESP32_H
#define SPARROWESP32_H

// Number encoding types
#define STRING 0     // Readable, but bigger
#define BASE64URL 1  // For humans hard to read, but more effective (less memory required and faster upload)

// TODO: Törölni
#include <Arduino.h>
#include <HTTPClient.h>
#include <SparrowClass.h>
#include <SparrowTimer.h>
#include <WiFi.h>

typedef enum {
    SUCCESS = 0,
} errorCodes;

// defin-ok funkciók ki és bekapcsolásához

class SparrowEsp32 {
   private:
    // WIFI Variables
    const char *ssid = "";
    const char *pass = "";
    unsigned int wifiConnAttemps = 0;
    unsigned int wifiLastConnAttemp = 0;
    unsigned int wifiForceReconnect = 10;
    unsigned int wifiReboot = 100;
    bool wifiConnRuning = false;

    WiFiClient wifiClient;
    HTTPClient httpClient;

    // Server Variables
    const char *serverAuth = "abcdefgh";
    const char *serverUrl = "192.168.0.100";
    const char *serverPort = "80";
    const char *serverLogApi = "http://192.168.0.100/api/things/log";
    const char *serverDataApi = "http://192.168.0.100/api/things/data";
    const char *serverDeviceApi = "http://192.168.0.100/api/things/device";

    SparrowTimer timerMain;

   public:
    bool begin(const char *ssid, const char *pass, const char *auth, const char *serverUrl, unsigned long delay = 10000);
    bool run();

    bool wifiInit();
    bool wifiReconnect();

    void wifiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);
    void wifiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
    void wifiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
    friend void callback_wifiStationConnected(arduino_event_id_t event, arduino_event_info_t info);
    friend void callback_wifiGotIP(arduino_event_id_t event, arduino_event_info_t info);
    friend void callback_wifiStationDisconnected(arduino_event_id_t event, arduino_event_info_t info);

    /*
    bool initializeQuarry();
    bool saveDataToQuarry(const int *channel, const int *data);
    bool saveDataToQuarry(const int *channel, const float *data);
    bool saveDataToQuarry(const int *channel, const char *data);
    bool saveDataToQuarry(const int *channel, const String *data);
    bool stageQuarry(unsigned long time = millis(), bool saveTime = true);
    bool uploadQuarry(unsigned long time = millis());
    bool emptyQuarry();
    */

    bool serverIsAllive();
    // bool serverSendStatus(const int *type);
    // bool serverSendData();

    static void mainCallback();

    int convertFloatToBase64(float input, char *encodedString);
    int convertIntToBase64(int input, char *encodedString);
    int encodeBase64(const unsigned char *input, unsigned int inputLength, char *output);
    unsigned char binaryToBase64(unsigned char v);

    void blynk();
};

bool SparrowEsp32::begin(const char *ssid, const char *pass, const char *auth, const char *serverUrl, unsigned long delay) {
    Serial.println("Sparrow ESP32 Library Allive!");
    this->ssid = ssid;
    this->pass = pass;
    this->serverAuth = auth;
    this->serverUrl = serverUrl;

    // timerMain.setInterval(delay, mainCallback);

    // TODO: serial log

    // Start the wifi connection process
    wifiInit();

    return 0;
}

bool SparrowEsp32::run() {
    timerMain.run();

    return 0;
}

bool SparrowEsp32::wifiInit() {
    Serial.println("[NETWORK] Initializing WiFi");

    // delete old config
    WiFi.disconnect(true);

    if (WiFi.getMode() != WIFI_STA) {
        WiFi.mode(WIFI_STA);
    }
    delay(100);

    // Set Wifi event listeners
    WiFi.onEvent([this](arduino_event_id_t event, arduino_event_info_t info) { wifiStationConnected(event, info); },
                 WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
    WiFi.onEvent([this](arduino_event_id_t event, arduino_event_info_t info) { wifiGotIP(event, info); },
                 WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
    WiFi.onEvent([this](arduino_event_id_t event, arduino_event_info_t info) { wifiStationDisconnected(event, info); },
                 WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    // Inicialize Wifi Library
    if (pass && strlen(pass)) {
        WiFi.begin(ssid, pass);
    } else {
        WiFi.begin(ssid);
    }

    return 0;
}

bool SparrowEsp32::wifiReconnect() {
    if (!wifiConnRuning) {
        wifiConnRuning = true;
        Serial.println("Reconnecting to WiFi");

        // force disconnect
        WiFi.disconnect();

        // Gives 10 seconds waiting before next try
        Serial.print("Waiting 10 sec before reconnect");
        for (size_t i = 0; i < 20; i++) {
            Serial.print(".");
            blynk();
            delay(500);
        }
        Serial.println("");
        delay(100);

        WiFi.reconnect();

        // Gives 10 seconds to connect
        Serial.print("Connecting");
        for (size_t i = 0; (i < 20 && WiFi.status() != WL_CONNECTED); i++) {
            Serial.print(".");
            blynk();
            delay(500);
        }
        Serial.println("");

        WiFi.printDiag(Serial);

        Serial.print("Attemp: ");
        Serial.println(wifiConnAttemps);
        wifiConnAttemps++;

        wifiConnRuning = false;
        return 1;
    }
    return 0;
}

void SparrowEsp32::wifiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.println("wifi disconnected");
    if (WiFi.status() != WL_CONNECTED && !wifiConnRuning) {
        Serial.println("reconnect");
        if (wifiReboot < wifiConnAttemps) {
            // TODO: log
            ESP.restart();
        } else {
            wifiReconnect();
        }
    }
}

void SparrowEsp32::wifiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    // TODO: Logolás
    Serial.println("Sucsessfully connected to Wifi!");
    wifiConnAttemps = 0;
}

void SparrowEsp32::wifiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
    // Test server connection
    if (serverIsAllive()) {
        // TODO: Send full system Status to server
        Serial.println("The server is avaliable.");
    } else {
        Serial.println("The server is not avaliable.");
    }
}

bool SparrowEsp32::serverIsAllive() {
    httpClient.begin(wifiClient, serverDeviceApi);
    httpClient.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = httpClient.GET();
    if (httpResponseCode > 0) {
        if (httpResponseCode == HTTP_CODE_OK) {
            String payload = httpClient.getString();
            httpClient.end();
            return 1;
        } else {
            httpClient.end();
            return 0;
        }
    } else {
        return 0;
    }

    return 0;
}

int SparrowESP32::convertIntToBase64(int input, char *encodedString) {
    unsigned char buffer[4];
    // Convert Int to Byte
    buffer[0] = (input >> 24) & 0xFF;
    buffer[1] = (input >> 16) & 0xFF;
    buffer[2] = (input >> 8) & 0xFF;
    buffer[3] = input & 0xFF;

    return encodeBase64(buffer, 4, encodedString);
}

int SparrowESP32::convertFloatToBase64(float input, char *encodedString) {
    unsigned char buffer[4];

    // Convert Float to Byte
    union float2bytes {
        float f;
        char b[4];
    } number;
    number.f = input;
    for (size_t i = 0; i < 4; i++) {
        buffer[i] = number.b[i];
    }

    return encodeBase64(buffer, 4, encodedString);
}

int SparrowESP32::encodeBase64(const unsigned char *input, unsigned int inputLength, char *output) {
    unsigned int full_sets = inputLength / 3;

    // While there are still full sets of 24 bits...
    for (unsigned int i = 0; i < full_sets; ++i) {
        output[0] = binaryToBase64(input[0] >> 2);
        output[1] = binaryToBase64((input[0] & 0x03) << 4 | input[1] >> 4);
        output[2] = binaryToBase64((input[1] & 0x0F) << 2 | input[2] >> 6);
        output[3] = binaryToBase64(input[2] & 0x3F);

        input += 3;
        output += 4;
    }

    switch (inputLength % 3) {
        case 0:
            output[0] = '\0';
            break;
        case 1:
            output[0] = binaryToBase64(input[0] >> 2);
            output[1] = binaryToBase64((input[0] & 0x03) << 4);
            output[2] = '=';
            output[3] = '=';
            output[4] = '\0';
            break;
        case 2:
            output[0] = binaryToBase64(input[0] >> 2);
            output[1] = binaryToBase64((input[0] & 0x03) << 4 | input[1] >> 4);
            output[2] = binaryToBase64((input[1] & 0x0F) << 2);
            output[3] = '=';
            output[4] = '\0';
            break;
    }

    return TS_OK_SUCCESS;
}

unsigned char SparrowESP32::binaryToBase64(unsigned char v) {
    // Capital letters - 'A' is ascii 65 and base64 0
    if (v < 26) return v + 'A';

    // Lowercase letters - 'a' is ascii 97 and base64 26
    if (v < 52) return v + 71;

    // Digits - '0' is ascii 48 and base64 52
    if (v < 62) return v - 4;

    // '-' is ascii 45 and base64 62
    if (v == 62) return '-';

    // '_' is ascii 95 and base64 62
    if (v == 63) return '_';

    return 64;
}

void SparrowEsp32::blynk() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
}

// Osztály példányosítása TODO: utána járni számít e a static, hogy lehetne optimalizálni, elérni hogy csak egy példányban futhasson

SparrowEsp32 Sparrow;

#endif