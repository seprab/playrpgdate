#ifndef LOG_H
#define LOG_H

#include <string>
#include "pdcpp/pdnewlib.h"

class Log {
public:
    template<typename... Args>
    static void Info(const char* message, Args... args);
    template<typename... Args>
    static void Error(const char* message, Args... args);

private:
    static const char* infoPrefix;
    static const char* errorPrefix;
};

#endif //LOG_H