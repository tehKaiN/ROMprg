#include <algorithm>
#include <cctype>
#include <locale>

// Shameless copy from SO:
// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

// trim from start (in place)
static inline void ltrimRef(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrimRef(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trimRef(std::string &s) {
    ltrimRef(s);
    rtrimRef(s);
}

// trim from start (copying)
static inline std::string ltrim(std::string s) {
    ltrimRef(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim(std::string s) {
    rtrimRef(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim(std::string s) {
    trimRef(s);
    return s;
}
