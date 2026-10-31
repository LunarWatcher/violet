#pragma once

#include "violet/algorithm/SortMethod.hpp"
#include "violet/conf/Frontmatter.hpp"
namespace violet {

struct TreePagesData {
    std::string folder_name;
    std::vector<Frontmatter> items = {};
    std::unordered_map<std::string, TreePagesData> subfolders = {};

    std::optional<Frontmatter> index = std::nullopt;

    iris::DataObject convert(SortMethod leafSort, SortMethod folderSort);
};

}
