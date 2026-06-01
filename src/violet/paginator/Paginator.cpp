#include "Paginator.hpp"
#include "violet/generate/FileManager.hpp"
#include "violet/generate/cache/MetadataCache.hpp"

namespace violet {

Paginator::Paginator(
    const Frontmatter& pageFm,
    FileManager& fileMan,
    MetadataCache& metaCache
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

    // TODO: support scoping so /index.md can be an index for /posts/*
    // TODO: error handling
    auto path = fileMan.getRootFolder() / std::filesystem::path {
        pageFm.internalPath
    };
    if (std::filesystem::is_regular_file(path)) {
        path = path.parent_path();
    }
    fileMan.recursivelyIterateFiles(
        path,
        [&](auto& entry) {
            const auto& entryPath = entry.path();
            if (!entry.is_regular_file()
                || (
                    entryPath.extension() != ".md"
                    && entryPath.extension() != ".html"
                )
                || entryPath.filename() == "404.html"
                || entryPath.filename() == "README.md"
                || entryPath.filename() == "index.md"
            ) {
                return;
            }
            // TODO: this does not handle HTML files without frontmatter. This could probably be resolved by an optional
            // return from loadMetadata
            auto filePath = std::filesystem::relative(
                entryPath,
                fileMan.getRootFolder()
            );
            auto& metadata = metaCache.loadMetadata(
                filePath
            );
            fm.push_back(&metadata.frontmatter);
        }
    );

    // TODO: custom predicates hard, check that this logic is right. The title logic appears to be right, but the test
    // blog test site currently does not include dates
    std::sort(
        fm.begin(),
        fm.end(),
        [](const Frontmatter* a, const Frontmatter* b) {
            if (a->date.has_value() && b->date.has_value() && a->date != b->date) {
                return a->date > b->date ? 1 : -1;
            }

            std::filesystem::path aPath = a->internalPath,
                bPath = b->internalPath;

            auto aFn = aPath.filename();
            auto bFn = bPath.filename();

            return aFn > bFn ? 1 : -1;
        }
    );
}

}
