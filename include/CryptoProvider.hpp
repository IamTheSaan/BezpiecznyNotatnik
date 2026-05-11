#pragma once
#include <string>

class CryptoProvider {
private:
    std::string serverMasterKey;

public:
    CryptoProvider(std::string key = "TGl0d28hIE9qY3p5em5vIG1vamEhIHR5IGplc3RlxZsgamFrIHpkcm93aWUuCgpJbGUgY2nEmSB0cnplYmEgY2VuacSHLCB0ZW4gdHlsa28gc2nEmSBkb3dpZSwKCkt0byBjacSZIHN0cmFjacWCLiBEemnFmyBwacSZa25vxZvEhyB0d8SFIHcgY2HFgmVqIG96ZG9iaWUKCldpZHrEmSBpIG9waXN1asSZLCBibyB0xJlza25pxJkgcG8gdG9iaWUu")
        : serverMasterKey(key) {}

    std::string encrypt(const std::string data);
    std::string decrypt(const std::string data);
    std::string generateId();
};