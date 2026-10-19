#include "Paginator.hpp"
#include "violet/generate/FileManager.hpp"
#include "violet/generate/cache/MetadataCache.hpp"
#include "violet/paginator/SortPredicates.hpp"

namespace violet {

Paginator::Paginator(
    const Frontmatter& pageFm,
    FileManager& fileMan,
    MetadataCache& metaCache,
    const std::filesystem::path& prefix,
    SortMethod sortMethod
) : Paginator(
    pageFm.internalPath,
    fileMan,
    metaCache,
    // NOLINTNEXTLINE(bugprone-unchecked-optional-access): this constructor signature is only defined for listings
    pageFm.listing->pageSize,
    prefix,
    sortMethod
) {}
Paginator::Paginator(
    const std::filesystem::path& owningFilePath,
    FileManager& fileMan,
    MetadataCache& metaCache,
    size_t pageSize,
    const std::filesystem::path& prefix,
    SortMethod sortMethod
)
    : pageSize(pageSize),
      basePath(owningFilePath),
      fileMan(fileMan)
{
    // No longer the case, only applies to the other signature (where it's hard-enforced with an unchecked optional
    // access :3)
    // if (pageFm.layout != "page_list" || !pageFm.listing.has_value()) {
    //     throw std::runtime_error(
    //         std::format(
    //             "Can only initialize a paginator on a paginated page list. Type = {}, has listing: {}",
    //             pageFm.type,
    //             pageFm.listing.has_value()
    //         )
    //     );
    // }

    // TODO: support scoping so /index.md can be an index for /posts/*
    // TODO: error handling
    auto path = fileMan.getRootFolder() / std::filesystem::path {
        owningFilePath
    };
    if (std::filesystem::is_regular_file(path)) {
        path = path.parent_path();
    }
    path /= prefix;
    fileMan.recursivelyIterateFiles(
        path,
        [&](auto& entry) -> bool {
            const auto& entryPath = entry.path();
            if (!entry.is_regular_file()
                || (
                    entryPath.extension() != ".md"
                    && entryPath.extension() != ".html"
                )
            ) {
                return true;
            }
            auto filePath = std::filesystem::relative(
                entryPath,
                fileMan.getRootFolder()
            );
            // Don't include the owning file. This is necessary to allow non-hidden `index.md`s elsewhere to work
            // properly.
            if (filePath == std::filesystem::relative(owningFilePath, fileMan.getRootFolder())) {
                return true;
            }
            auto& metadata = metaCache.loadMetadata(
                filePath
            );

            if (
                metadata.frontmatter.isAsset
                || metadata.frontmatter.hidden
            ) {
                return true;
            }
            fm.push_back(&metadata.frontmatter);
            return true;
        }
    );

    std::sort(
        fm.begin(),
        fm.end(),
        sort::sortMethods.at(sortMethod)
    );
}

}
