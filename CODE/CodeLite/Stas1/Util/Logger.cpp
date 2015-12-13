#include "Logger.hpp"

void Logger::log(char const* fmt, ...)
{
    int n = 0;
    
    if(tagEnabled)
        n = snprintf(logBuff, 200, "[%s]: ", name.c_str());
    
    va_list argp;
    va_start(argp, fmt);
    vsnprintf(&logBuff[n], 500, fmt, argp);
    va_end(argp);

    if(logStream) (*logStream) << logBuff;
}

Logger::Logger(std::string name, std::ostream& logStream)
{
    this->name = name;
    this->logStream = &logStream;
    this->tagEnabled = name.size() ? true : false;
}

void Logger::setOutStream(std::ostream& s)
{
    logStream = &s;
}

void Logger::setTagEnabled(bool b)
{
    tagEnabled = b;
}