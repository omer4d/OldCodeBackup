#include "Logger.hpp"

char Logger::sLogBuff[700];

void Logger::log(char const* fmt, ...)
{
    int n = snprintf(sLogBuff, 200, "[%s]: ", mName.c_str());

    va_list argp;
    va_start(argp, fmt);
    vsnprintf(&sLogBuff[n], 500, fmt, argp);
    va_end(argp);

    if(mLogStream) (*mLogStream) << sLogBuff;
}

Logger::Logger(std::string pName, std::ostream& pLogStream)
{
    mName = pName;
    mLogStream = &pLogStream;
}

void Logger::outStream(std::ostream& s)
{
    mLogStream = &s;
}