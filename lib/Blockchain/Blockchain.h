#pragma once
#include <Arduino.h>

struct Avatar
{
    String collection;
    int tokenId;
    bool isValid;
};

String getPicoboundImage(const char *rpcUrl, const char *contract, int tokenId);
Avatar getPrimaryAvatar(const char *rpcUrl, const char *resolverAddress, const char *userAddress);
