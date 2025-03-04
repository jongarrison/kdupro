#ifndef NETWORK_H
#define NETWORK_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h> //See: https://medium.com/@punnyarthabanerjee/esp8266-handling-data-in-json-7c6f62c9062e
#include global.h

namespace network {

    void enable_network();
    void disable_network();

}

#endif // NETWORK_H