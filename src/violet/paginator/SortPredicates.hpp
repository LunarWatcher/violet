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
extern bool byTitle(
    const Frontmatter* a,
    const Frontmatter* b
);

extern bool byCreationDateRef(
    const Frontmatter& a,
    const Frontmatter& b
);
extern bool byLastModifiedDateRef(
    const Frontmatter& a,
    const Frontmatter& b
);
extern bool byTitleRef(
    const Frontmatter& a,
    const Frontmatter& b
);

// NOLINTNEXTLINE(bugprone-throwing-static-initialization): why the fuck would it?
inline std::unordered_map<SortMethod, std::function<bool(const Frontmatter*, const Frontmatter*)>> sortMethods = {
    {SortMethod::ByCreationDate, &byCreationDate},
    {SortMethod::ByLastModifiedDate, &byLastModifiedDate},
    {SortMethod::ByTitle, &byTitle},
};
inline std::unordered_map<SortMethod, std::function<bool(const Frontmatter&, const Frontmatter&)>> sortMethodsRef = {
    {SortMethod::ByCreationDate, &byCreationDateRef},
    {SortMethod::ByLastModifiedDate, &byLastModifiedDateRef},
    {SortMethod::ByTitle, &byTitleRef},
};

}
