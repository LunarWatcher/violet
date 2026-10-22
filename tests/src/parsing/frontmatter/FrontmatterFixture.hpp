#pragma once

#include "violet/conf/Frontmatter.hpp"
namespace tests {

struct FrontmatterFixture {
    violet::Frontmatter parse(const std::string& data) {
        nlohmann::json j;
        std::stringstream ss;
        ss << data;
        ss >> j;
        return j.get<violet::Frontmatter>();
    }
};

}
