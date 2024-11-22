#include "Log.h"

PlaydateAPI* Log::playdateApi = nullptr;

void Log::Initialize(PlaydateAPI* api) {
    playdateApi = api;
}

template<typename... Args>
void Log::Info(const char* message, Args... args) {
    if (playdateApi) {
        message = (std::string("[INFO] ") + message).c_str();
        playdateApi->system->logToConsole(message, args...);
    }
}

template<typename... Args>
void Log::Error(const char* message, Args... args) {
    if (playdateApi) {
        message = (std::string("[ERROR] ") + message).c_str();
        playdateApi->system->error(message, args...);
    }
}

// Explicit template instantiation
template void Log::Info<>(const char*);
template void Log::Error<>(const char*);
template void Log::Info<>(const char*, char const*);
template void Log::Error<>(const char*, char const*);
template void Log::Info<>(const char*, char const*, char const*);
template void Log::Error<>(const char*, char const*, char const*);
template void Log::Info<>(const char*, unsigned int, int);
template void Log::Error<>(const char*, unsigned int, int);
template void Log::Info<>(const char*, char const*, int, const char*, const char*);
template void Log::Error<>(const char*, char const*, int, const char*, const char*);
template void Log::Info<>(const char*, int);
template void Log::Error<>(const char*, int);
template void Log::Info<>(const char*, unsigned int);
template void Log::Error<>(const char*, unsigned int);