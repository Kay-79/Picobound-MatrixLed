#include <Arduino.h>
#include "Network.h"
#include "Blockchain.h"
#include "Display.h"

const char *ssid = "Lock B";
const char *password = "0347979595";
const char *rpcUrl = "https://ethereum-sepolia-rpc.publicnode.com";
const char *contractAddress = "0x00000000610d8474af42d87b42f8c54efc05e023";
const int tokenId = 3921;

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting Picobound LED Matrix...");

    displayInit();
    if (!connectWiFi(ssid, password))
    {
        ESP.restart();
    }
}

void loop()
{
    Serial.println("Fetching image...");
    String img = getPicoboundImage(rpcUrl, contractAddress, tokenId);
    Serial.printf("Image data length: %d\n", img.length());
    Serial.println(img);

    if (img.length() == 258)
    {
        img.replace("0x", "");
        drawPixelString(img);
    }
    else
    {
        showTestPattern();
    }

    delay(60 * 60 * 1000);
}
