#pragma once
#include <Arduino.h>

struct Avatar
{
    String collection;
    uint64_t tokenId;
    bool isValid;
};

String getPicoboundImage(const char *rpcUrl, const char *contract, uint64_t tokenId);
Avatar getPrimaryAvatar(const char *rpcUrl, const char *resolverAddress, const char *userAddress);
