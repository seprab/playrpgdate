#include "Log.h"

PlaydateAPI* Log::playdateApi = nullptr;
const char*Log::infoPrefix = "[INFO] ";
const char*Log::errorPrefix = "[ERROR] ";

void Log::Initialize(PlaydateAPI* api) {
    playdateApi = api;
}

template<typename... Args>
void Log::Info(const char* message, Args... args) {
    if (playdateApi)
    {
        const size_t len1 = strlen(infoPrefix);
        const size_t len2 = strlen(message);
        const size_t totalLen = len1 + len2;

        auto result = new char[totalLen + 1];
        strcpy(result, infoPrefix);
        strcat(result, message);
        playdateApi->system->logToConsole(result, args...);
        delete[] result;
    }
}

template<typename... Args>
void Log::Error(const char* message, Args... args) {
    if (playdateApi) {
        size_t len1 = strlen(errorPrefix);
        size_t len2 = strlen(message);
        size_t totalLen = len1 + len2;

        auto result = new char[totalLen + 1];
        strcpy(result, errorPrefix);
        strcat(result, message);
        playdateApi->system->error(result, args...);
        delete[] result;
    }
}

// Explicit template instantiation
template void Log::Info<>(const char*);
template void Log::Info<>(const char*, int);
template void Log::Info<>(const char*, int, int);
template void Log::Info<>(const char*, int, int, int);
template void Log::Info<>(const char*, char const*);
template void Log::Info<>(const char*, char const*, unsigned int);
template void Log::Info<>(const char*, unsigned int);
template void Log::Info<>(const char*, unsigned int, int);
template void Log::Info<>(const char*, unsigned int, char*);
template void Log::Info<>(const char*, char const*, char const*);
template void Log::Info<>(const char*, unsigned int, char*, char*);
template void Log::Info<>(const char*, unsigned int, char*, char*, int);
template void Log::Info<>(const char*, unsigned int, char*, const char*);
template void Log::Info<>(const char*, char const*, int, const char*, const char*);
template void Log::Info<>(char const*, char const*, unsigned long);
template void Log::Info<>(char const*, unsigned long);

template void Log::Error<>(const char*);
template void Log::Error<>(const char*, int);
template void Log::Error<>(const char*, int, int);
template void Log::Error<>(const char*, int, int, int);
template void Log::Error<>(const char*, char const*);
template void Log::Error<>(const char*, char const*, unsigned int);
template void Log::Error<>(const char*, unsigned int);
template void Log::Error<>(const char*, unsigned int, int);
template void Log::Error<>(const char*, unsigned int, char*);
template void Log::Error<>(const char*, char const*, char const*);
template void Log::Error<>(const char*, unsigned int, char*, char*);
template void Log::Error<>(const char*, unsigned int, char*, char*, int);
template void Log::Error<>(const char*, unsigned int, char*, const char*);
template void Log::Error<>(const char*, char const*, int, const char*, const char*);
template void Log::Error<>(char const*, char const*, unsigned long);
template void Log::Error<>(char const*, unsigned long);
