#include "FileReader.hpp"

#include <sstream>

std::string tests::readFile(std::ifstream& f) {
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}
