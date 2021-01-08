#ifndef UTIL_H
#define UTIL_H

#include <string.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

static std::string trim(std::string s) {
    int i=0;
    if(s.length() == 0)
        return "";

    while (s[i]==' ') {
        i++;
    }
    s=s.substr(i);
    i=s.size()-1;
    if(i == -1)
        return "";

    while(s[i]==' ') {
        i--;
    }
    s=s.substr(0,i+1);
    return s;
}
static void split(std::string& s, std::string& delim, std::vector< std::string >* ret) {  
    size_t last = 0;
    size_t index = s.find_first_of(delim, last);  
    
    string tmp = "";
    while (index != std::string::npos){
        if(tmp != "")
            tmp = tmp + delim + trim(s.substr(last, index-last));
        else
            tmp = trim(s.substr(last, index-last));
        if(tmp[0] != '"' || tmp[tmp.length()-1] == '"'){
            if(tmp[0] == '"' && tmp[tmp.length()-1] == '"')
                tmp = tmp.substr(1, tmp.length()-2);
            ret->push_back(trim(tmp));
            tmp = "";
        }

        last = index+1;  
        index = s.find_first_of(delim, last);
    }
    if (index-last>0){
        if(tmp != "")
            tmp = tmp + " " + trim(s.substr(last, index-last));
        else
            tmp = trim(s.substr(last, index-last));
        
        if(tmp[0] == '"' && tmp[tmp.length()-1] == '"')
            tmp = tmp.substr(1, tmp.length()-2);
        ret->push_back(trim(tmp));
    }
}


#endif