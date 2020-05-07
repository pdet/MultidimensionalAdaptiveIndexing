#ifndef SPLIT_STRING_H
#define SPLIT_STRING_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

template <class T>
class SplitString 
{
    public:
        static std::vector<T> split(const std::string& s, std::string delimiter)
        {
            size_t pos_start = 0;
            size_t pos_end;
            size_t delim_len = delimiter.length();
            std::string token;
            std::vector<T> res;

            while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
                token = s.substr (pos_start, pos_end - pos_start);
                pos_start = pos_end + delim_len;
                if(token.size() == 0) continue;
                res.push_back (SplitString<T>::from_string(token));
            }

            res.push_back(SplitString<T>::from_string(s.substr (pos_start)));
            return res;
        }

        static T from_string(const std::string &s);
};


template<>
inline int64_t SplitString<int64_t>::from_string(const std::string &s)
{
    return std::stoi(s);
}

template<>
inline size_t SplitString<size_t>::from_string(const std::string &s)
{
    return std::stoi(s);
}

template<>
inline float SplitString<float>::from_string(const std::string &s)
{
    return std::stod(s);
}

#endif //SPLIT_STRING_H
