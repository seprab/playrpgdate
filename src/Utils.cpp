
#include <cstring>
#include <cstdlib>
#include <type_traits>
#include "Utils.h"
#include "Log.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"

std::string Utils::Subchar(const char* source, int start, int end)
{
    const int length = end - start;
    // Use std::string constructor for safety and automatic memory management
    return std::string(source + start, length);
}

std::string Utils::ValueDecoder(char *buffer, jsmntok_t *tokens, int start, int finish, const char *property) {
    for (int i = start; i < finish; i++)
    {
        if (tokens[i].type != JSMN_STRING)
        {
            continue;
        }
        std::string parseProperty = Subchar(buffer, tokens[i].start, tokens[i].end);
        if(parseProperty == property)
        {
            return Subchar(buffer, tokens[i+1].start, tokens[i+1].end);
        }
    }
    Log::Error("Property %s not found", property);
    return "";
}

int Utils::InitializeJSMN(jsmn_parser *parser, char *charBuffer, const size_t len, int tokenLimit, jsmntok_t* t) {
    int calculatedTokens = jsmn_parse(parser, charBuffer, len, t, tokenLimit);
    Log::Info("Number of tokens: %d", calculatedTokens);
    if (calculatedTokens < 0)
    {
        switch (calculatedTokens)
        {
            case jsmnerr::JSMN_ERROR_INVAL:
                Log::Error("bad token, JSON string is corrupted");
                break;
            case jsmnerr::JSMN_ERROR_NOMEM:
            {
                int expectedTokens = jsmn_parse(parser, charBuffer, len, nullptr, 0);
                Log::Error("not enough tokens, JSON string is too large. It should be above: %i", expectedTokens);
                break;
            }
            case jsmnerr::JSMN_ERROR_PART:
                Log::Error("JSON string is too short, expecting more JSON data");
                break;
            default:
                Log::Error("Unknown error parsing JSON");
                break;
        }
        return 0;
    }
    return calculatedTokens;
}
