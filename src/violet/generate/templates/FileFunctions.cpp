#include "FileFunctions.hpp"

#include "InjaManager.hpp"

namespace violet {

FileFunctions::FileFunctions(InjaManager& man) : man(man) {
    man.env.add_callback("listPages", 2, [this](inja::Arguments& args) -> nlohmann::json {
        return listPages(args);
    });
    man.env.add_callback("treePages", 2, [this](inja::Arguments& args) -> nlohmann::json {
        return treePages(args);
    });
    man.env.add_callback("paginatedUrl", 2, [this](inja::Arguments& args) -> nlohmann::json {
        return paginatedUrl(args);
    });
    man.env.add_callback("listPagesByTaxonomy", 2, [this](auto& args) {
        return listPagesByTaxonomy(args);
    });
}

nlohmann::json FileFunctions::listPagesByTaxonomy(inja::Arguments& args) {
    auto& root = man.fileManager.getRootFolder();
    auto taxonomy = args.at(0)->get<std::string>();
    auto value = args.at(1)->get<std::string>();

    nlohmann::json pages;
    man.fileManager.recursivelyIterateFiles(
        root,
        [&](auto& entry) {
            const auto& entryPath = entry.path();
            if (!entry.is_regular_file()
                || (
                    entryPath.extension() != ".md"
                    && entryPath.extension() != ".html"
                )
                || entryPath.filename() == "404.html"
            ) {
                return;
            }
            // TODO: this does not handle HTML files without frontmatter. This could probably be resolved by an optional
            // return from loadMetadata
            auto filePath = std::filesystem::relative(
                entryPath,
                man.fileManager.getRootFolder()
            );
            auto& metadata = man.metaCache.loadMetadata(
                filePath
            );

            const auto& taxonomies = metadata.frontmatter.taxonomies;
            if (auto it = taxonomies.find(taxonomy); it != taxonomies.end()) {
                auto& vec = it->second;
                if (std::find(vec.begin(), vec.end(), value) != vec.end()) {
                    pages.push_back(metadata.frontmatter);
                }
            }
        }
    );
    return pages;
}

nlohmann::json FileFunctions::listPages(inja::Arguments& args) {
    auto& frontmatter = *args.at(0);
    auto scope = args.at(1)->get<std::string>();

    if (scope.contains("..")) {
        throw std::runtime_error(
            "listPages does not accept paths that recurse upwards"
        );
    }

    // TODO: error handling
    // TODO: validation, violet_internals can technically be modified to theoretically allow arbitrary paths, which is
    // bad
    auto path = man.fileManager.getRootFolder() / std::filesystem::path {
        frontmatter.at("violet_internals.path")
    };
    if (std::filesystem::is_regular_file(path)) {
        path = path.parent_path();
    }
    path /= scope;

    nlohmann::json pages;
    man.fileManager.recursivelyIterateFiles(
        path,
        [&](auto& entry) {
            const auto& entryPath = entry.path();
            if (!entry.is_regular_file()
                || (
                    entryPath.extension() != ".md"
                    && entryPath.extension() != ".html"
                )
                || entryPath.filename() == "404.html"
            ) {
                return;
            }
            // TODO: this does not handle HTML files without frontmatter. This could probably be resolved by an optional
            // return from loadMetadata
            auto filePath = std::filesystem::relative(
                entryPath,
                man.fileManager.getRootFolder()
            );
            auto& metadata = man.metaCache.loadMetadata(
                filePath
            );
            pages.push_back(metadata.frontmatter);
        }
    );
    return pages;
}

nlohmann::json FileFunctions::treePages(inja::Arguments& args) {
    auto& frontmatter = *args.at(0);
    auto scope = args.at(1)->get<std::string>();

    if (scope.contains("..")) {
        throw std::runtime_error(
            "listPages does not accept paths that recurse upwards"
        );
    }

    // TODO: error handling
    // TODO: validation, violet_internals can technically be modified to theoretically allow arbitrary paths, which is
    // bad
    auto path = man.fileManager.getRootFolder() / std::filesystem::path {
        frontmatter.at("violet_internals.path")
    };
    if (std::filesystem::is_regular_file(path)) {
        path = path.parent_path();
    }
    path /= scope;

    nlohmann::json pages;
    man.fileManager.recursivelyIterateFiles(
        path,
        [&](auto& entry) {
            const auto& entryPath = entry.path();
            if (!entry.is_regular_file()
                || (
                    entryPath.extension() != ".md"
                    && entryPath.extension() != ".html"
                )
                || entryPath.filename() == "404.html"
            ) {
                return;
            }
            auto filePath = std::filesystem::relative(
                entryPath,
                man.fileManager.getRootFolder()
            );
            auto& metadata = man.metaCache.loadMetadata(
                filePath
            );

            nlohmann::json* next = &pages;

            if (filePath.has_parent_path()) {
                for (auto& segment : filePath.parent_path()) {
                    nlohmann::json* curr = &((*next)["subfolders"][segment]);
                    next = curr;
                }
            }

            if (filePath.filename() == "README.md" || filePath.filename() == "index.md") {
                (*next)["index"] = metadata.frontmatter;
            } else {
                (*next)["items"].push_back(metadata.frontmatter);
            }
        }
    );
    return pages;
}

nlohmann::json FileFunctions::paginatedUrl(inja::Arguments& args) {
    auto baseUrl = args.at(0)->get<std::string>();
    auto page = args.at(1)->get<int64_t>();
    // This restricts us to half the max uint64_t value, but no one will ever care
    if (page <= 0) {
        throw std::runtime_error("Invalid page number");
    }

    std::stringstream ss;
    ss << baseUrl;
    if (!baseUrl.ends_with('/')) {
        ss << "/";
    }

    ss << "page/" << page << "/index.html";
    return ss.str();
}

}
