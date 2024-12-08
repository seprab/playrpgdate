
#include <cstring>
#include <cstdlib>
#include <type_traits>
#include "Utils.h"
#include "Log.h"

char* Utils::Subchar(const char* source, int start, int end)
{
    const int length = end - start;
    //allocating the memory for the name equals the length of the target + 1 for the null terminator
    auto subchar = new char[length+1];
    for(int i = 0; i < length; i++)
    {
        subchar[i] = source[start + i];
    }
    subchar[length] = '\0'; //null terminator
    return subchar;
}

char* Utils::ValueDecoder(char *buffer, jsmntok_t *tokens, int start, int finish, const char *property) {
    char* parseProperty;
    for (int i = start; i < finish; i++)
    {
        if (tokens[i].type != JSMN_STRING)
        {
            continue;
        }
        parseProperty = Subchar(buffer, tokens[i].start, tokens[i].end);
        if(strcmp(parseProperty, property) == 0)
        {
            char* value = Subchar(buffer, tokens[i+1].start, tokens[i+1].end);
            return value;
        }
    }
    Log::Error("Property %s not found", property);
    return nullptr;
}