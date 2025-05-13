#include "Log.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"

const char*Log::infoPrefix = "[INFO] ";
const char*Log::errorPrefix = "[ERROR] ";

template<typename... Args>
void Log::Info(const char* message, Args... args) {

        const size_t len1 = strlen(infoPrefix);
        const size_t len2 = strlen(message);
        const size_t totalLen = len1 + len2;

        auto result = new char[totalLen + 1];
        strcpy(result, infoPrefix);
        strcat(result, message);
        pdcpp::GlobalPlaydateAPI::get()->system->logToConsole(result, args...);
        delete[] result;
}

template<typename... Args>
void Log::Error(const char* message, Args... args) {
        size_t len1 = strlen(errorPrefix);
        size_t len2 = strlen(message);
        size_t totalLen = len1 + len2;

        auto result = new char[totalLen + 1];
        strcpy(result, errorPrefix);
        strcat(result, message);
        pdcpp::GlobalPlaydateAPI::get()->system->error(result, args...);
        delete[] result;
}

// Explicit template instantiation
template void Log::Info<>(const char*);
template void Log::Info<>(const char*, int);
template void Log::Info<>(const char*, float);
template void Log::Info<>(const char*, int, int);
template void Log::Info<>(const char*, float, float);
template void Log::Info<>(const char*, float, int);
template void Log::Info<>(const char*, float, int, int);
template void Log::Info<>(const char*, int, float);
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
template void Log::Error<>(const char*, float);
template void Log::Error<>(const char*, int, int);
template void Log::Error<>(const char*, float, float);
template void Log::Error<>(const char*, float, int);
template void Log::Error<>(const char*, float, int, int);
template void Log::Error<>(const char*, int, float);
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
