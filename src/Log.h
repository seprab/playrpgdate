//
// Created by sergioprada on 11/21/2024.
//

#ifndef LOG_H
#define LOG_H
#include <string>
#include "pdcpp/pdnewlib.h"
#include <iomanip>

class Log
{
public:
    static PlaydateAPI* playdateApi;

    template<typename... Args>
    static void Info(const char* message, Args... args )
    {
        message = (std::string("[INFO] ")+message).c_str();
        playdateApi->system->logToConsole(message, args...);
    }
    template<typename... Args>
    static void Error(const char* message, Args... args )
    {
        message = (std::string("[ERROR] ")+message).c_str();
        playdateApi->system->error(message, args...);
    }
};
PlaydateAPI* Log::playdateApi = nullptr;

#endif //LOG_H
