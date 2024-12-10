//
// Created by Sergio Prada on 19/06/24.
//

#ifndef CARDOBLAST_UTILS_H
#define CARDOBLAST_UTILS_H


#include <pd_api/pd_api_file.h>
#include "jsmn.h"

class Utils {
public:
    Utils() = delete;
    static char* Subchar(const char* source, int start, int end);
    static char* ValueDecoder(char *buffer, jsmntok_t *tokens, int start, int finish,  const char* property);
    static char* ReadBufferFromJSON(const char* fileName, int limitOfTokens, FileStat* fileStat);
    static int InitializeJSMN(jsmn_parser *parser, char *charBuffer, const size_t len, int tokenLimit, jsmntok_t* t);
};


#endif //CARDOBLAST_UTILS_H
