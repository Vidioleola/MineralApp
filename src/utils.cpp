
#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>

std::string url_encode(const std::string &value) {

    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        std::string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' || c == '/') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char) c);
        escaped << std::nouppercase;
    }

    return escaped.str();
}

/*
    Escape all occurences of to_escape with escape_with.
*/
std::string str_escape(const std::string &value, char to_escape, char escape_with) {
    std::ostringstream escaped;
    escaped.fill('0');
    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        std::string::value_type c = (*i);
        if (c==to_escape) escaped << escape_with;
        escaped << c;
    }
    return escaped.str();
}

/*
    Convert to lowercase.
*/
std::string str_tolower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

/*
    Remove all unicode from a string.
*/
static bool invalid_char(char c) {
    return c<0;
}
std::string strip_unicode(std::string &str) {
    str.erase(remove_if(str.begin(),str.end(), invalid_char), str.end());
    return str;
}

