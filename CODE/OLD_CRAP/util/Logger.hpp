#ifndef _Logger_hpp_
#define _Logger_hpp_

#include <stdio.h>
#include <stdarg.h>
#include <iostream>

class Logger {
private:
    static char sLogBuff[700];
    std::ostream* mLogStream;
    std::string mName;

public:

    void log(char const* fmt, ...);
    Logger(std::string pName, std::ostream& pLogStream = std::cout);
    void outStream(std::ostream& s);
};

#endif
