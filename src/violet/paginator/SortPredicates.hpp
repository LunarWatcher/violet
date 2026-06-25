#pragma once

#include "violet/algorithm/SortMethod.hpp"
#include "violet/conf/Frontmatter.hpp"
namespace violet::sort {

extern bool byCreationDate(
    const Frontmatter* a,
    const Frontmatter* b
);
extern bool byLastModifiedDate(
    const Frontmatter* a,
    const Frontmatter* b
);

inline std::unordered_map<SortMethod, std::function<bool(const Frontmatter*, const Frontmatter*)>> sortMethods = {
    {SortMethod::ByCreationDate, &byCreationDate},
    {SortMethod::ByLastModifiedDate, &byLastModifiedDate},
};

}
