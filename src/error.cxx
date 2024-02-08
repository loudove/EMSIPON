// Imported from lpem project (https://github.com/loudove/lpem) 
// under the MIT license
// Αuthor: Loukas D. Peristeras (l.peristeras@inn.demokritos.gr)

#include <iostream>

#include "error.h"

namespace Utils {

Error::Error() {}

Error::~Error() {}

void Error::_assert(const bool &condition, const std::string &msg, const std::string &file, const int &line)
{
    if ( ! condition)
    {
        cout << "error: " << msg << " | file: " << file << " @ " << line << endl;
        exit(1);
    }
}

void Error::fatal(const std::string &msg, const std::string &file, const int &line)
{
    cout << "error: " << msg << " | file: " << file << " @ " << line << endl;
    exit(1);
}


void Error::fatal(const std::string &msg)
{
    cout << "error: " << msg << endl;
    exit(1);
}

void Error::warning(const std::string &msg, const std::string &file, const int &line)
{
    cout << "warning: " << msg << " | file: " << file << " @ " << line << endl;
}

void Error::warning(const std::string &msg)
{
    cout << "warning: " << msg << endl;
}

}

