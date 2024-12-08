//
// Created by Sergio Prada on 19/06/24.
//

#ifndef CARDOBLAST_UTILS_H
#define CARDOBLAST_UTILS_H


#include "jsmn.h"

class Utils {
public:
    Utils() = delete;
    static char* Subchar(const char* source, int start, int end);

    static char* ValueDecoder(char *buffer, jsmntok_t *tokens, int start, int finish,  const char* property);
};


#endif //CARDOBLAST_UTILS_H
