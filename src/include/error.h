// Imported from lpem project (https://github.com/loudove/lpem) 
// under the MIT license
// Αuthor: Loukas D. Peristeras (l.peristeras@inn.demokritos.gr)

#ifndef ERROR_H
#define ERROR_H

#include <cstdlib>
#include <string>
using namespace std;

#define FNL __FILE__, __LINE__

#define _isnan isnan
#define _copysign copysign

#ifndef NDEBUG
#define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            cerr << "Error: `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << endl; \
            exit(EXIT_FAILURE); \
        } \
    } while (false)
#else
#define ASSERT(condition, message) do { } while (false)
#endif

namespace Utils {

/// Control error and warning messages tod std and err output.
class Error
{
  public:

    Error();
    ~Error();
    /// If condiftion output the \p msg from \p file at \p line and exit.
    static void _assert(const bool &condition, const string &msg, const string &file, const int &line);
    /// Output the \p msg from \p file at \p line and exit.
    static void fatal(const string &msg, const string &file, const int &line);
    /// Output the \p msg and exit.
    static void fatal(const string &msg);
    /// If condiftion output the \p msg from \p file at \p line .
    static void warning(const string &msg, const string &file, const int &line);
    /// Output the \p msg .
    static void warning(const string &msg);
};

}

#endif // UTILS_ERROR_H
