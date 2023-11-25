#ifndef SPARROWCLASS_H
#define SPARROWCLASS_H

#include <Arduino.h>
#include <Client.h>

#define TS_VER "0.0.1"

// TODO: Documentation: https://stackoverflow.com/questions/37284399/doxygen-single-comment-block-for-multiple-functions

// TODO: Debug

// TODO: costumize
#ifdef ARDUINO_ARCH_ESP8266
#define TS_USER_AGENT "tslib-arduino/" TS_VER " (ESP8266)"
#elif defined(ARDUINO_ARCH_ESP32)
#define TS_USER_AGENT "tslib-arduino/" TS_VER " (ESP32)"
#else
#define TS_USER_AGENT "tslib-arduino/" TS_VER " (unknown)"
#endif

#define FIELDNUM_MIN 1
#define FIELDNUM_MAX 8
#define FIELDLENGTH_MAX 255  // Max length for a field in ThingSpeak is 255 bytes (UTF-8)

// TODO:
#define TIMEOUT_MS_SERVERRESPONSE 5000   // Wait up to five seconds for server to respond
#define TS_OK_SUCCESS 200                // OK / Success
#define TS_ERR_BADAPIKEY 400             // Incorrect API key (or invalid ThingSpeak server address)
#define TS_ERR_BADURL 404                // Incorrect API key (or invalid ThingSpeak server address)
#define TS_ERR_OUT_OF_RANGE -101         // Value is out of range or string is too long (> 255 bytes)
#define TS_ERR_INVALID_FIELD_NUM -201    // Invalid field number specified
#define TS_ERR_SETFIELD_NOT_CALLED -210  // setField() was not called before writeFields()
#define TS_ERR_CONNECT_FAILED -301       // Failed to connect to ThingSpeak
#define TS_ERR_UNEXPECTED_FAIL -302      // Unexpected failure during write to ThingSpeak
#define TS_ERR_BAD_RESPONSE -303         // Unable to parse response
#define TS_ERR_TIMEOUT -304              // Timeout waiting for server to respond
#define TS_ERR_NOT_INSERTED -401         // Point was not inserted (most probable cause is the rate limit of once every 15 seconds)

#ifndef NUMBER_ENCODING
#define NUMBER_ENCODING STRING
#endif
#ifndef SPARROW_PORT_NUMBER
#define SPARROW_PORT_NUMBER 80
#endif
#ifndef SPARROW_HTTPS_PORT_NUMBER
#define SPARROW_HTTPS_PORT_NUMBER 443
#endif

// beállítások debug
#ifndef SPARROW_CACHE_SIZE
#define SPARROW_CACHE_SIZE 30
#endif

#ifndef SPARROW_FIELDS_SIZE
#define SPARROW_FIELDS_SIZE 16
#endif

class SparrowClass {
   public:
    bool begin(Client &client, const char *serverUrl, const char *deviceApiKey);

    int writeField(unsigned int field, int value);
    int writeField(unsigned int field, float value);
    int writeField(unsigned int field, String value);

    int setField(unsigned int field, int value);
    int setField(unsigned int field, const int *value, unsigned int size);
    int setField(unsigned int field, float value);
    int setField(unsigned int field, const float *value, unsigned int size);
    int setField(unsigned int field, String value);
    int setField(unsigned int field, const String *value, unsigned int size);
    int setStatus(String status);
    int setTime(String time);
    int setTime(unsigned long time);
    int writeFields();

    int writeSystemStatusTelemetry(int level);
    int writeLog(unsigned int type, String value);
    int writeLog(unsigned int type, String value);

    int writeRaw(String postMessage);
    int abortWriteRaw();

    /* Upcoming features:
    String readStringField(unsigned int field);
    float readFloatField(unsigned int field);
    int readIntField(unsigned int field);
    String readRaw(String suffixURL);
    int readCommands(int* &commands);

    int executeCommands(int* &commands);
    int sendSystemStatus(int level = 0);
    */

    // private:  // TODO: comment it out after Alpha development
    bool connectServer();
    bool writeHTTPHeader();
    int getHTTPResponse(String &response);
    unsigned int getWriteFieldsContentLength();
    int finishWrite();

    // String converter methods
    int convertFloatToChar(float value, char *valueString);

    Client *client = NULL;
    const char *httpHeader = NULL;
    const char *deviceApiKey = NULL;
    const char *serverUrl = NULL;
    unsigned int port = SPARROW_PORT_NUMBER;
    // TODO: data storage variables

    void setHttpHeader(const char *serverUrl, const char *deviceApiKey);
};

#endif