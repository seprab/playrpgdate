#ifndef LOG_H
#define LOG_H

#include <string>
#include "pdcpp/pdnewlib.h"

class Log {
public:
    static void Initialize(PlaydateAPI* api);
    template<typename... Args>
    static void Info(const char* message, Args... args);
    template<typename... Args>
    static void Error(const char* message, Args... args);

private:
    static const char* infoPrefix;
    static const char* errorPrefix;
    static PlaydateAPI* playdateApi;
};

#endif //LOG_H