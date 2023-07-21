#include "stdafx.h"
using namespace std;

namespace json {

    typedef map<string, string> JSON;
    
    namespace {
        
        // Splits string into substrings splitted by delimeter.
        vector<string> split(string str, char delimeter) {
            
            vector<string> strings;
            stringstream ss(str);

            string str2;
            while (getline(ss, str2, delimeter)) {
                strings.push_back(str2);
            }

            return strings;
        }

        // Removes all extra characters in string.
        string removeExtra(string str) {

            string newStr;
            for (int i = 0; i < str.length(); i++) {
                if (str[i] != '{' && str[i] != '}' && str[i] != '"')
                    newStr.push_back(str[i]);
            }

            return newStr;
        }
    }

    int getInt(string needle, JSON object) {

        auto found = object.find(needle);
        if (found == object.end()) {
            printf("Couldn't find object with key %s!\n", needle.c_str());
            return -1;
        }

        return atoi(found->second.c_str());
    }

    string getStr(string needle, JSON object) {

        auto found = object.find(needle);
        if (found == object.end()) {
            printf("Couldn't find object with key %s\n", needle.c_str());
            return "";
        }

        return found->second;
    }   
    
    map<string, string> parse(string str) {
        
        map<string, string> parsed;
        string toSplit = removeExtra(str);
        vector<string> splitted = split(toSplit, ',');

        for (int i = 0; i < splitted.size(); i++) {
            
            vector<string> keyAndValue = split(splitted.at(i), ':');
            parsed.insert({keyAndValue.at(0), keyAndValue.at(1)});
        }
        
        return parsed;
    }

    map<string, string> null() {

        return map<string, string> ();
    }

};