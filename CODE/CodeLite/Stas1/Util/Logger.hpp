#ifndef _Logger_hpp_
#define _Logger_hpp_

#include <stdio.h>
#include <stdarg.h>
#include <iostream>

class Logger {
private:
    char logBuff[700];
    std::ostream* logStream;
    std::string name;
    bool tagEnabled;

public:

    void log(char const* fmt, ...);
    Logger(std::string name="", std::ostream& logStream = std::cout);
    void setOutStream(std::ostream& s);
    void setTagEnabled(bool b);
};

#endif
