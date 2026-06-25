#include "SortPredicates.hpp"

namespace violet {
bool sort::byCreationDate(
    const Frontmatter* a,
    const Frontmatter* b
) {
    // TODO: Would prefer to sort all the items without a date below the items with date, so they're always pushed to
    // the bottom, but this is difficult enough that I can't be bothered right now (I'm already like 5 tangents deep
    // into making RSS feeds work, and this does technically work)
    if (a->date.has_value() && b->date.has_value() && a->date != b->date) {
        return a->date > b->date;
    }

    std::filesystem::path aPath = a->internalPath,
        bPath = b->internalPath;

    auto aFn = aPath.filename();
    auto bFn = bPath.filename();

    return aFn > bFn;
}
bool sort::byLastModifiedDate(
    const Frontmatter* a,
    const Frontmatter* b
) {
    std::optional<int64_t> aDate = a->last_modified.has_value() ? a->last_modified : a->date;
    std::optional<int64_t> bDate = b->last_modified.has_value() ? b->last_modified : b->date;

    if (aDate.has_value() && bDate.has_value() && aDate != bDate) {
        return aDate > bDate;
    }

    std::filesystem::path aPath = a->internalPath,
        bPath = b->internalPath;

    auto aFn = aPath.filename();
    auto bFn = bPath.filename();

    return aFn > bFn;
}

}
