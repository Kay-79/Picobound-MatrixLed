#include "Network.h"
#include "AppConfig.h"
#include <WiFiManager.h>
#include <Preferences.h>

Preferences preferences;
char walletAddress[66]; // 0x + 64 chars max

void saveConfigCallback()
{
    Serial.println("Should save config");
}

String setupNetwork()
{
    WiFi.mode(WIFI_STA);

    // Load stored wallet
    preferences.begin(AppConfig::Storage::NAMESPACE, true); // Read-only
    String storedWallet = preferences.getString(AppConfig::Storage::KEY_WALLET, AppConfig::Blockchain::DEFAULT_WALLET);
    preferences.end();

    storedWallet.toCharArray(walletAddress, 66);

    WiFiManager wm;

    // Custom parameter for wallet address
    WiFiManagerParameter custom_wallet("wallet", "Wallet Address (0x...)", walletAddress, 64);
    wm.addParameter(&custom_wallet);

    wm.setSaveConfigCallback(saveConfigCallback);
    wm.setConfigPortalTimeout(180); // Timeout after 3 minutes to allow normal operation if stuck

    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name
    bool res = wm.autoConnect(AppConfig::Wifi::AP_NAME, AppConfig::Wifi::AP_PASSWORD);

    if (!res)
    {
        Serial.println("Failed to connect");
        // ESP.restart();
    }
    else
    {
        Serial.println("connected...yeey :)");
    }

    // Save the custom parameter
    String newWallet = custom_wallet.getValue();
    if (newWallet != storedWallet && newWallet.length() > 0)
    {
        Serial.printf("Saving new wallet: %s\n", newWallet.c_str());
        preferences.begin(AppConfig::Storage::NAMESPACE, false); // Read-write
        preferences.putString(AppConfig::Storage::KEY_WALLET, newWallet);
        preferences.end();
        return newWallet;
    }

    return storedWallet;
}

void resetNetworkSettings()
{
    WiFiManager wm;
    wm.resetSettings();

    // Also clear preferences if needed, or just wifi
    // preferences.begin(AppConfig::Storage::NAMESPACE, false);
    // preferences.clear();
    // preferences.end();

    Serial.println("Network settings reset");
}
