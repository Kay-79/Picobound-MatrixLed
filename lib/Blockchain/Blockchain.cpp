#include "Blockchain.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

struct Metadata
{
    String treasuryWallet;
    String image;
};

Metadata parseAbiMetadata(String hex)
{
    Metadata md;
    if (hex.startsWith("0x"))
        hex.remove(0, 2);

    String addressHex = hex.substring(24, 64);
    md.treasuryWallet = "0x" + addressHex;

    unsigned long imageOffset = strtoul(hex.substring(64, 128).c_str(), NULL, 16);
    unsigned long urlOffset = strtoul(hex.substring(128, 192).c_str(), NULL, 16);

    unsigned long imageLength = strtoul(hex.substring(imageOffset * 2, imageOffset * 2 + 64).c_str(), NULL, 16);
    md.image = "0x" + hex.substring(imageOffset * 2 + 64, imageOffset * 2 + 64 + imageLength * 2);
    return md;
}

String getPicoboundImage(const char *rpcUrl, const char *contract, uint64_t tokenId)
{
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    http.setTimeout(15000);
    http.begin(client, rpcUrl);
    http.addHeader("Content-Type", "application/json");

    JsonDocument docRequest;
    docRequest["jsonrpc"] = "2.0";
    docRequest["id"] = 1;
    docRequest["method"] = "eth_call";
    JsonArray params = docRequest["params"].to<JsonArray>();
    JsonObject callObj = params.add<JsonObject>();
    callObj["to"] = contract;

    char tokenIdHex[65];
    sprintf(tokenIdHex, "%064llx", tokenId);

    String data = String("0x834d5fac") + tokenIdHex;
    callObj["data"] = data;
    params.add("latest");

    String body;
    serializeJson(docRequest, body);

    Serial.println("=== Sending Request ===");
    Serial.println(body);

    int httpCode = http.POST(body);
    Serial.printf("HTTP code: %d\n", httpCode);

    if (httpCode != 200)
    {
        Serial.printf("HTTP failed: %d\n", httpCode);
        http.end();
        return "";
    }

    String payload = http.getString();
    Serial.println("=== Response ===");
    Serial.println(payload);

    JsonDocument docResponse;
    DeserializationError error = deserializeJson(docResponse, payload);
    http.end();

    if (error)
    {
        Serial.printf("JSON parse error: %s\n", error.c_str());
        return "";
    }

    String hexResult = docResponse["result"] | "";
    String img = parseAbiMetadata(hexResult).image;
    Serial.println("Parsed image string:");
    Serial.println(img);

    return img;
}

Avatar getPrimaryAvatar(const char *rpcUrl, const char *resolverAddress, const char *userAddress)
{
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    http.setTimeout(15000);
    http.begin(client, rpcUrl);
    http.addHeader("Content-Type", "application/json");

    JsonDocument docRequest;
    docRequest["jsonrpc"] = "2.0";
    docRequest["id"] = 1;
    docRequest["method"] = "eth_call";
    JsonArray params = docRequest["params"].to<JsonArray>();
    JsonObject callObj = params.add<JsonObject>();
    callObj["to"] = resolverAddress;

    // Construct data: 0x0bba5952 + padded user address
    String userAddrStr = String(userAddress);
    if (userAddrStr.startsWith("0x"))
        userAddrStr.remove(0, 2);

    // Pad to 64 chars (32 bytes)
    while (userAddrStr.length() < 64)
    {
        userAddrStr = "0" + userAddrStr;
    }

    String data = "0x0bba5952" + userAddrStr;
    callObj["data"] = data;
    params.add("latest");

    String body;
    serializeJson(docRequest, body);

    Serial.println("=== Sending Avatar Request ===");
    // Serial.println(body);

    int httpCode = http.POST(body);
    Avatar avatar = {"", 0, false};

    if (httpCode != 200)
    {
        Serial.printf("HTTP failed: %d\n", httpCode);
        http.end();
        return avatar;
    }

    String payload = http.getString();
    // Serial.println(payload);

    JsonDocument docResponse;
    DeserializationError error = deserializeJson(docResponse, payload);
    http.end();

    if (error)
    {
        Serial.printf("JSON parse error: %s\n", error.c_str());
        return avatar;
    }

    String hexResult = docResponse["result"] | "";
    if (hexResult.startsWith("0x"))
        hexResult.remove(0, 2);

    if (hexResult.length() < 128)
    {
        Serial.println("Invalid response length");
        return avatar;
    }

    // Parse collection address (first 32 bytes / 64 chars)
    String collectionHex = hexResult.substring(24, 64); // Last 20 bytes of the first word
    avatar.collection = "0x" + collectionHex;

    // Parse tokenId (second 32 bytes / 64 chars)
    String tokenIdHex = hexResult.substring(64, 128);
    avatar.tokenId = (uint64_t)strtoull(tokenIdHex.c_str(), NULL, 16);
    avatar.isValid = true;

    Serial.printf("Resolved Avatar: %s #%llu\n", avatar.collection.c_str(), avatar.tokenId);

    return avatar;
}
