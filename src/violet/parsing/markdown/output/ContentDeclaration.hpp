#pragma once

#include <string>
#include <unordered_set>
namespace violet::Markdown {

struct ContentDeclaration {
    std::string parsedContents;
    std::string tableOfContents;
};

}
