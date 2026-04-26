#pragma once

#include <string>
#include <sstream>

#include <vector>

namespace violet {

struct URL {
    std::string path;
    std::string fragment;
    std::string query;

    URL(const std::string& in) {
        std::stringstream ss;
        ss << in;
        std::vector<std::string> segments;
        
        char ch;
        std::stringstream accumulatedComponent;
        while (ss >> ch) {
            if ()
        }
    }
};

}
