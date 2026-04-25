#pragma once

#include <functional>
#include <string>
namespace violet::Markdown {

// TODO: pass stringstream& instead?
using LinkTranslator = std::function<std::string(const std::string& rawUrl)>;

}
