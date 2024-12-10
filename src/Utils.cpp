
#include <cstring>
#include <cstdlib>
#include <type_traits>
#include "Utils.h"
#include "Log.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"

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

char *Utils::ReadBufferFromJSON(const char *fileName, int limitOfTokens, FileStat *fileStat) {
    auto pd = pdcpp::GlobalPlaydateAPI::get();
    Log::Info("Loading data from %s", fileName);
    pd->file->stat(fileName, fileStat);
    SDFile* file = pd->file->open(fileName, kFileRead);
    if(file==nullptr)
    {
        Log::Error("Error opening the file %s: %s", fileName, pd->file->geterr());
        return nullptr;
    }
    //allocate memory for the buffer before reading data into it.
    void* buffer = new char[fileStat->size + 1]; // +1 for the null terminator
    int readResult = pd->file->read(file, buffer, fileStat->size);
    if(readResult < 0)
    {
        Log::Error("Error reading the file %s: %s", fileName, pd->file->geterr());
        return nullptr;
    }
    pd->file->close(file);

    return static_cast<char*>(buffer);
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
