#include "StringUtils.h"

bool StringUtils::startsWith(const char* str, const char* prefix) {
    uint8_t strChar, prefixChar;
    const uint8_t* _prefix = (uint8_t*) prefix;
    const uint8_t* _str = (uint8_t*) str;
    while ((strChar = *_str++) & (prefixChar = *_prefix++)) {
        if ( prefixChar != strChar ) return false;
    }
    return true;
}

bool StringUtils::equals(const char* str1, const char* str2, size_t strlen) {
    for(int i=0; i<strlen; i++) if(str1[i] != str2[i]) return false;
    return true;
}

bool StringUtils::contains(const char* str, const char* substr) {
    return strstr(str, substr) != NULL;
}