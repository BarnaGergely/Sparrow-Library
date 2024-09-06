# Sparrow - Arduino ESP32 IoT Library

You can easily connect to a Sparrow IoT server, send data, and receive commands with this library.

- Reliable WiFi connecting
- Memory caching, if the wifi is temporarily disconnected
- Easy interface for setup, data sending, and receiving

- Technologies: [Arduino (framework)](https://docs.arduino.cc/programming/), [C++](https://cplusplus.com/)
- Compatible microcontrollers: [ESP32](https://www.espressif.com/en/products/socs/esp32), [ESP8266](https://www.espressif.com/en/products/socs/esp8266)

**⚠️This project is under development, not working right now.⚠️**

## Goal

To create a reliable and easy-to-use data communication library for the Sparrow IoT Platform as the [Blynk Library](https://github.com/blynkkk/blynk-library) or the [ThingSpeak Library](https://github.com/mathworks/thingspeak-arduino). This library manages entirely the WiFi connection and the communication with the server and makes the IoT data logger development much easier.

## Usage

### Simple Setup

```C++
#include <WiFi.h>
#include <WiFiClient.h>
#include <Sparrow.h>

/* Fill-in information from Device Info here */
#define SPARROW_DEVICE_ID           "xxxxxx"
#define SPARROW_AUTH_TOKEN            "YourAuthToken"

/* Comment this out to disable prints and save space */
#define SPARROW_PRINT Serial

// Your WiFi credentials.
// Set password to "" for open networks.
static char ssid[] = "YourNetworkName";
static char pass[] = "YourPassword";

void setup()
{
  // Debug console
  Serial.begin(115200);

  Sparrow.begin(SPARROW_AUTH_TOKEN, ssid, pass, "yourserver.com/api/device");
  // You can also specify port and IP:
  //Sparrow.begin(SPARROW_AUTH_TOKEN, ssid, pass, "yourserver.com/api/device", 80);
  //Sparrow.begin(SPARROW_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);
}

void loop()
{
  Sparrow.run();
  // You can inject your own code or combine it with other sketches.
  // Remember to avoid delay() function!
}
```

### Simple Data Sending Demo

```C++
#include <WiFi.h>
#include <WiFiClient.h>
#include <Sparrow.h>

/* Replace these constants with the server's*/
#define UPTIME_SENSOR 1

/* Fill-in information from Device Info here */
#define SPARROW_DEVICE_ID           "xxxxxx"
#define SPARROW_AUTH_TOKEN            "YourAuthToken"

SparrowTimer timer;

/* Comment this out to disable prints and save space */
#define SPARROW_PRINT Serial

// Your WiFi credentials.
// Set password to "" for open networks.
static char ssid[] = "YourNetworkName";
static char pass[] = "YourPassword";

// This function sends Arduino's uptime every second to Uptime sensor.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Sparrow.virtualWrite(UPTIME_SENSOR, millis() / 1000);
}

void setup()
{
  // Debug console
  Serial.begin(115200);

  Sparrow.begin(SPARROW_AUTH_TOKEN, ssid, pass, "yourserver.com/api/device");
  // You can also specify port and IP:
  //Sparrow.begin(SPARROW_AUTH_TOKEN, ssid, pass, "yourserver.com/api/device", 80);
  //Sparrow.begin(SPARROW_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Sparrow.run();
  // You can inject your own code or combine it with other sketches.
  // Remember to avoid delay() function!
  timer.run()
}
```
