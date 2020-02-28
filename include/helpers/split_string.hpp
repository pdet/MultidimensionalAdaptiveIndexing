#ifndef SPLIT_STRING_H
#define SPLIT_STRING_H

#include "table.hpp"
#include "query.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

template <class T>
class SplitString 
{
    public:
        static vector<T> split(const string& s, char delimiter)
        {
            string buf;                         // Have a buffer string
            stringstream ss(s.c_str());         // Insert the string into a stream

            vector<T> tokens;               // Create vector to hold our words

            while (ss >> buf)
                tokens.push_back(atof(buf.c_str()));

            return tokens;
        }
};

#endif // SPLIT_STRING_H
