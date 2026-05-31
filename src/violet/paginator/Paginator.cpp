#include "Paginator.hpp"
#include "violet/generate/FileManager.hpp"

namespace violet {

Paginator::Paginator(
    const Frontmatter& pageFm,
    FileManager& fileMan
) : pageFm(pageFm),
    fileMan(fileMan)
{
    if (pageFm.layout != "page_list" || !pageFm.listing.has_value()) {
        throw std::runtime_error(
            std::format(
                "Can only initialize a paginator on a paginated page list. Type = {}, has listing: {}",
                pageFm.type,
                pageFm.listing.has_value()
            )
        );
    }

    // TODO: populate this->fm
}

}
