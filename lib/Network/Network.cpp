#include "Network.h"

bool connectWiFi(const char *ssid, const char *password) {
    WiFi.begin(ssid, password);
    Serial.printf("Connecting to WiFi: %s\n", ssid);

    int retries = 20;
    while (WiFi.status() != WL_CONNECTED && retries-- > 0) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("\nWiFi connected! IP: %s\n", WiFi.localIP().toString().c_str());
        return true;
    } else {
        Serial.println("\nFailed to connect WiFi.");
        return false;
    }
}
