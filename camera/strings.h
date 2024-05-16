#ifndef STRINGS_H
#define STRINGS_H
#include <Windows.h>
#include <string>
#include <memory>
#include <vector>
#include <iostream>

struct Strings {

    static std::string toString(const std::wstring &ws)
    {
        std::string s;
        int len = WideCharToMultiByte(CP_ACP, 0, ws.c_str(), -1, NULL, 0, NULL, NULL);
        if (len == 0) {
            return s;
        }
        len += 1;
        std::unique_ptr<char[]> str(new char[len]);
        WideCharToMultiByte(CP_ACP, 0, ws.c_str(), -1, str.get(), len, NULL, NULL);
        s = std::string(str.get());
        return s;
    }

    static std::string format(int totalsize, const char *formatText, ...)
    {
        std::string text(totalsize, 0);
        va_list ap;
        va_start(ap, formatText);
        ::vsprintf((char*)text.c_str(), formatText, ap);
        va_end(ap);
        return text;
    }

    static std::string upper(const std::string &s_)
    {
        std::string s(s_);
        for (size_t i = 0; i < s.size(); i++) {
            if (s[i] >= 97 && s[i] <= 122) {
                s[i] -= 32;
            }
        }
        return s;
    }

    static std::string lower(const std::string &s_)
    {
        std::string s(s_);
        for (std::size_t i = 0; i < s.size(); i++) {
            if (s[i] >= 65 && s[i] <= 90) {
                s[i] += 32;
            }
        }
        return s;
    }

    static std::vector<std::string> split(const std::string& src, const std::string& delim)
    {
        std::vector<std::string> elems;
        std::size_t pos = 0;
        std::size_t len = src.length();
        std::size_t delim_len = delim.length();
        if (delim_len == 0) {
            return elems;
        }
        while (pos < len) {
            int find_pos = src.find(delim, pos);
            if (find_pos < 0) {
                elems.push_back(src.substr(pos, len - pos));
                break;
            }
            elems.push_back(src.substr(pos, find_pos - pos));
            pos = find_pos + delim_len;
        }
        return elems;
    }

    static int stringToInt(const std::string &text)
    {
        return std::atoi(text.c_str());
    }

    static char hexCharToInt4(char c)
    {
        char x = 0;
        if (c >= 'A' && c <= 'Z') {
            x = c - 'A' + 10;
        } else if (c >= 'a' && c <= 'z') {
            x = c - 'a' + 10;
        } else if (c >= '0' && c <= '9') {
            x = c - '0';
        }
        return x;
    }

    static unsigned char hexStringToInt8(const char* hex)
    {
        unsigned char x0 = hexCharToInt4(hex[1]);
        unsigned char x1 = hexCharToInt4(hex[0]);
        return (x1<<4) + x0;
    }

    static unsigned short hexStringToInt16(const std::string &hex)
    {
        unsigned char x0 = hexCharToInt4(hex[3]);
        unsigned char x1 = hexCharToInt4(hex[2]);
        unsigned char x2 = hexCharToInt4(hex[1]);
        unsigned char x3 = hexCharToInt4(hex[0]);
        return (x3<<12) + (x2<<8) + (x1<<4) + x0;
    }

    static bool parseVidPid(const std::string &name, unsigned short &vid, unsigned short &pid)
    {
        std::string desc = Strings::upper(name);
        int offset = -1;
        offset = desc.find("VID_");
        if (offset > 0 && offset + 8 <= desc.size()) {
            std::string vidString = desc.substr(offset + 4, 4);
            vid = Strings::hexStringToInt16(vidString);
        } else {
            return false;
        }
        offset = desc.find("PID_");
        if (offset > 0 && offset + 8 <= desc.size()) {
            std::string pidString = desc.substr(offset + 4, 4);
            pid = Strings::hexStringToInt16(pidString);
        } else {
            return false;
        }
        return true;
    }

    static std::wstring guidToWString(const GUID &guid)
    {
        WCHAR guidString[1024];
        StringFromGUID2(guid, guidString, 1024);
        return std::wstring(guidString);
    }
    static std::string guidToString(const GUID &guid)
    {
        WCHAR guidString[1024];
        StringFromGUID2(guid, guidString, 1024);
        return toString(guidString);
    }

};
#endif // STRINGS_H
