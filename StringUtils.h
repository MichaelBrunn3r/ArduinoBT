#ifndef StringUtils_HEADER
#define StringUtils_HEADER

#include <stdint.h>
#include <string.h>

namespace StringUtils {
        bool startsWith(const char* str, const char* prefix);

        bool equals(const char* str1, const char* str2, size_t strlen);

        bool contains(const char* str, const char* substr);
};

#endif // StringUtils_HEADER