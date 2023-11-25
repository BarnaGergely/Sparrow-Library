#ifndef SPARROWCLASS_CPP
#define SPARROWCLASS_CPP

#include <SparrowClass.h>

bool SparrowClass::begin(Client &client, const char *serverUrl, const char *deviceApiKey) {
    this->client = &client;
    this->deviceApiKey = deviceApiKey;
    this->serverUrl = serverUrl;
    setHttpHeader(serverUrl, deviceApiKey);

    // TODO: initialize data quarry
    // TODO: set lastReadStatus
    return true;
}

int SparrowClass::writeField(unsigned int field, int value) {
    char valueString[10];  // int range is -32768 to 32768, so 7 bytes including terminator, plus a little extra
    itoa(value, valueString, 10);

    return writeField(field, valueString);
}

int SparrowClass::writeField(unsigned int field, float value) {
    char valueString[20];
    int status = convertFloatToChar(value, valueString);
    if (status != TS_OK_SUCCESS) return status;

    return writeField(field, valueString);
}

// TODO: egyedivé tenni
int SparrowClass::writeField(unsigned int field, String value) {
    // Invalid field number specified
    if (field < FIELDNUM_MIN || field > FIELDNUM_MAX) return TS_ERR_INVALID_FIELD_NUM;
    // Max # bytes for ThingSpeak field is 255
    if (value.length() > FIELDLENGTH_MAX) return TS_ERR_OUT_OF_RANGE;

    String postMessage = String("field");
    /*
    postMessage.concat(field);
    postMessage.concat("=");
    postMessage.concat(value);
    */

    // return writeRaw(channelNumber, postMessage, writeAPIKey);
}

int SparrowClass::writeRaw(String postMessage) {
    if (!connectServer()) {
        // Failed to connect to ThingSpeak
        return TS_ERR_CONNECT_FAILED;
    }
    /*
        // TODO: Post data to thingspeak
        if (!this->client->print("POST /update HTTP/1.1\r\n")) return abortWriteRaw();
        if (!writeHTTPHeader(writeAPIKey)) return abortWriteRaw();
        if (!this->client->print("Content-Type: application/x-www-form-urlencoded\r\n")) return abortWriteRaw();
        if (!this->client->print("Content-Length: ")) return abortWriteRaw();
        if (!this->client->print(postMessage.length())) return abortWriteRaw();
        if (!this->client->print("\r\n\r\n")) return abortWriteRaw();
        postMessage.concat("&headers=false");
        if (!this->client->print(postMessage)) return abortWriteRaw();

        resetWriteFields();

        return finishWrite();
        */
}

bool SparrowClass::connectServer() {
    bool connectSuccess = false;
    connectSuccess = client->connect(serverUrl, port);
    return connectSuccess;
}

bool SparrowClass::writeHTTPHeader() {
    if (!this->client->print(httpHeader)) return false;

    return true;
}

void SparrowClass::setHttpHeader(const char *serverUrl, const char *deviceApiKey = NULL) {
    String headerSting;
    headerSting += String("Host: ").concat(serverUrl) + String("\r\n");
    headerSting += String("User-Agent: ").concat(TS_USER_AGENT) + String("\r\n");
    if (NULL != deviceApiKey) {
        headerSting += String("X-SparrowApiKey: ").concat(deviceApiKey) + String("\r\n");
    }
    this->httpHeader = headerSting.c_str();
}

#endif