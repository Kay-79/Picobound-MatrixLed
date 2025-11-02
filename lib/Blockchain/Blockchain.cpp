#include "Blockchain.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

String parseAbiEncodedString(String hexResult) {
    if (hexResult.startsWith("0x")) hexResult.remove(0, 2);
    String lengthHex = hexResult.substring(64, 128);
    unsigned long length = strtoul(lengthHex.c_str(), NULL, 16);
    if (length == 0 || length > 512) return "";

    String dataHex = hexResult.substring(128, 128 + (length * 2));
    String result = "";
    for (int i = 0; i < length * 2; i += 2) {
        char c = (char)strtol(dataHex.substring(i, i + 2).c_str(), NULL, 16);
        if (c == 0) break;
        result += c;
    }
    return result;
}

String getPicoboundImage(const char *rpcUrl, const char *contract, int tokenId) {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    http.setTimeout(15000);
    http.begin(client, rpcUrl);
    http.addHeader("Content-Type", "application/json");

    DynamicJsonDocument docRequest(768);
    docRequest["jsonrpc"] = "2.0";
    docRequest["id"] = 1;
    docRequest["method"] = "eth_call";
    JsonArray params = docRequest.createNestedArray("params");
    JsonObject callObj = params.createNestedObject();
    callObj["to"] = contract;

    char tokenIdHex[65];
    sprintf(tokenIdHex, "%064x", tokenId);
    callObj["data"] = String("0xb4b3f359") + tokenIdHex;
    params.add("latest");

    String body;
    serializeJson(docRequest, body);

    int httpCode = http.POST(body);
    if (httpCode != 200) {
        Serial.printf("HTTP failed: %d\n", httpCode);
        http.end();
        return "";
    }

    DynamicJsonDocument docResponse(8192);
    DeserializationError error = deserializeJson(docResponse, http.getString());
    http.end();

    if (error) {
        Serial.printf("JSON parse error: %s\n", error.c_str());
        return "";
    }

    String hexResult = docResponse["result"] | "";
    return parseAbiEncodedString(hexResult);
}
