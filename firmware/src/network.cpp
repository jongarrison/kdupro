#include "network.h"

namespace network {

    void enable_network() {


    }

    void disable_network() {
        WiFi.mode(WIFI_OFF);
        WiFi.forceSleepBegin();
        delay(100);    
    }

} // namespace network