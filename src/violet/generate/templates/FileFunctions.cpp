#include "FileFunctions.hpp"

#include "IrisManager.hpp"
#include "violet/algorithm/SortMethod.hpp"
#include "violet/conf/Frontmatter.hpp"
#include "violet/data/Constants.hpp"
#include "violet/data/RenderResult.hpp"
#include "violet/exceptions/SyntaxError.hpp"
#include "violet/exceptions/TemplateError.hpp"
#include "violet/generate/templates/ext/SafeCastEnum.hpp"
#include "violet/parsing/Markdown.hpp"
#include "violet/util/ErrorHandling.hpp"
#include <filesystem>

namespace violet {

FileFunctions::FileFunctions(IrisManager& man) : man(man) {
    man.env.add_callback("listPages", 2, [this](iris::Arguments& args) -> nlohmann::json {
        return listPages(args);
    });
    man.env.add_callback("listPagesPaginated",  [this](iris::Arguments& args) -> nlohmann::json {
        return listPagesPaginated(args);
    });
    man.env.add_callback("treePages", 2, [this](iris::Arguments& args) -> nlohmann::json {
        return treePages(args);
    });
    man.env.add_callback("paginatedUrl", 2, [this](iris::Arguments& args) -> nlohmann::json {
        return paginatedUrl(args);
    });
    man.env.add_callback("listPagesByTaxonomy", 2, [this](auto& args) {
        return listPagesByTaxonomy(args);
    });
    man.env.add_callback("loadJson", [this](auto& args) {
        return loadJson(args);
    });
    man.env.add_callback("render", 1, [this](auto& args) {
        return render(args);
    });
}
nlohmann::json listPagesPaginated(iris::Arguments& args);

nlohmann::json FileFunctions::listPagesByTaxonomy(iris::Arguments& args) {
    auto& root = man.fileManager.getRootFolder();
    auto taxonomy = args.at(0)->get<std::string>();
    auto value = args.at(1)->get<std::string>();

    nlohmann::json pages;
    man.fileManager.recursivelyIterateFiles(
        root,
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
            // TODO: this does not handle HTML files without frontmatter. This could probably be resolved by an optional
            // return from loadMetadata
            auto filePath = std::filesystem::relative(
                entryPath,
                man.fileManager.getRootFolder()
            );
            auto& metadata = man.metaCache.loadMetadata(
                filePath
            );

            if (
                metadata.frontmatter.isAsset
                || metadata.frontmatter.hidden
            ) {
                return true;
            }

            const auto& taxonomies = metadata.frontmatter.taxonomies;
            if (auto it = taxonomies.find(taxonomy); it != taxonomies.end()) {
                auto& vec = it->second;
                if (std::find(vec.begin(), vec.end(), value) != vec.end()) {
                    pages.push_back(metadata.frontmatter);
                }
            }
            return true;
        }
    );
    return pages;
}

nlohmann::json FileFunctions::listPages(iris::Arguments& args) {
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
        frontmatter.at(violet::constants::InternalPathRef)
    };
    if (std::filesystem::is_regular_file(path)) {
        path = path.parent_path();
    }
    path /= scope;

    nlohmann::json pages;
    man.fileManager.recursivelyIterateFiles(
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
            // TODO: this does not handle HTML files without frontmatter. This could probably be resolved by an optional
            // return from loadMetadata
            auto filePath = std::filesystem::relative(
                entryPath,
                man.fileManager.getRootFolder()
            );
            auto& metadata = man.metaCache.loadMetadata(
                filePath
            );
            if (!metadata.frontmatter.isAsset && !metadata.frontmatter.hidden) {
                pages.push_back(metadata.frontmatter);
            }
            return true;
        }
    );
    return pages;
}

nlohmann::json FileFunctions::treePages(iris::Arguments& args) {
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
        frontmatter.at(violet::constants::InternalPathRef)
    };
    if (std::filesystem::is_regular_file(path)) {
        path = path.parent_path();
    }
    path /= scope;

    nlohmann::json pages;
    man.fileManager.recursivelyIterateFiles(
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
                man.fileManager.getRootFolder()
            );
            auto& metadata = man.metaCache.loadMetadata(
                filePath
            );

            if (
                metadata.frontmatter.isAsset
                || metadata.frontmatter.hidden
            ) {
                return true;
            }

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
            return true;
        }
    );
    return pages;
}

nlohmann::json FileFunctions::paginatedUrl(iris::Arguments& args) {
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

nlohmann::json FileFunctions::listPagesPaginated(iris::Arguments& args) {
    const auto& frontmatter = *args.at(0);
    const auto scope = args.at(1)->get<std::string>();
    // TODO: this is technically 
    const auto sortMethod = safeCastEnum<SortMethod>(
        args.at(2)->get<int>()
    );

    if (!sortMethod.has_value()) {
        throw std::runtime_error(
            "listPagesPaginated got illegal sortMethod. Make sure you're using one of the values in violet.sort."
        );
    }

    uint64_t pageSize = 50;
    uint64_t pageNumber = 1;
    if (args.size() > 3) {
        pageSize = *args.at(3);
    }
    if (args.size() > 4) {
        pageNumber = *args.at(4);
    }


    Paginator pag {
        frontmatter.at(violet::constants::InternalPathRef),
        this->man.fileManager,
        this->man.metaCache,
        pageSize,
        scope,
        *sortMethod
    };
    if (pageNumber == 0) {
        throw std::runtime_error("Page numbers are 1-indexed");
    }
    --pageNumber;

    if (pageNumber >= pag.getTotalPages()) {
        // Page out of range
        return {};
    }
    nlohmann::json out;
    auto page = pag.page(pageNumber);
    for (const auto* fm : *page) {
        out.push_back(*fm);
    }

    return out;
}

nlohmann::json FileFunctions::loadJson(iris::Arguments& args) {
    nlohmann::json frontmatter = nullptr;
    std::string fn;

    if (args.size() < 1 || args.size() > 2) {
        throw TemplateError(
            std::format(
                "loadJson takes either one or two arguments, but got {}",
                args.size()
            )
        );
    }
    if (!args.at(0)->is_string()) {
        minilog::warn(
            "loadJson(frontmatter, fn) is deprecated. The new signature is loadJson(fn[, frontmatter]), where the "
            "frontmatter is optional if `fn` is an absolute path"
        );
        frontmatter = *args.at(0);
        fn = args.at(1)->get<std::string>();
    } else {
        fn = args.at(0)->get<std::string>();
        if (args.size() == 2) {
            frontmatter = *args.at(1);
        }
    }

    std::filesystem::path p;
    if (fn.starts_with('/')) {
        if (fn.starts_with("//")) {
            throw std::runtime_error("Absolute paths cannot start with two slashes!");
        }
        p = this->man.opts.root / fn.substr(1);
    } else {
        if (frontmatter == nullptr) {
            throw TemplateError(
                "loadJson was called with a non-absolute path, but no frontmatter was provided. To use a relative "
                "path, you must provide the page or site global so the function can tell what the filename is relative "
                "to."
            );
        }
        p = std::filesystem::path {
            frontmatter.at(violet::constants::InternalPathRef).get<std::string>()
        };
        if (!std::filesystem::is_directory(p)) {
            p = p.parent_path();
        }
        p /= fn;
    }

    // This feels bad, but it should work.
    // Should probably turn this into a standard function somewhere
    p = std::filesystem::canonical(p);
    if (!p.string().starts_with(this->man.opts.root.string())) {
        minilog::error(
            "loadJson found possible root escaping: fn = {}, resolved path = {}",
            fn, p.string()
        );
        throw std::runtime_error(
           "loadJson found possible root escaping. See log messages."
        );
    }

    std::ifstream f(p);
    if (!f)  {
        minilog::error(
            "Failed to open {}", p.string()
        );
        throw std::runtime_error("failed to open file");
    }

    nlohmann::json data;
    try {
        f >> data;
    } catch (...) {
        minilog::error("Failed to parse json in {}", p.string());
        throw;
    }
    return data;
}

nlohmann::json FileFunctions::render(iris::Arguments& args) {
    auto fm = args.at(0)->get<Frontmatter>();

    // TODO: desperately need better out of bounds path checking
    if (fm.internalPath.starts_with('/') || fm.internalPath.contains("..")) {
        throw std::runtime_error("Path passed to render breaks project containment");
    }

    auto path = std::filesystem::path(fm.internalPath);
    auto [ metadata, in ] = this->man.metaCache.openFile(
        path
    );

    std::stringstream fileContent;
    fileContent << in.rdbuf();
    RenderResult res;
    std::string rawContent;

    Frontmatter safeFm = metadata.frontmatter;

    switch (safeFm.internalFileType) {
    case ProcessedFileType::Markdown: {
        try {
            auto page = violet::Markdown::parseWithContentPostprocessing(
                fileContent,
                std::bind(
                    projectBasedTranslator,
                    this->man.cfg.prefix,
                    this->man.opts.root,
                    fm.internalPath,
                    std::placeholders::_1
                )
            );
            rawContent = page.parsedContents;
            safeFm.tableOfContents = page.tableOfContents;
        } catch (const SyntaxError& synError) {
            renderSyntaxError(
                fm.internalPath,
                synError,
                fileContent
            );
            throw TemplateError(
                std::format(
                    "Template API error: A nested markdown rendering error occurred while rendering {}",
                    fm.internalPath
                )
            );
        }
    } break;
    case ProcessedFileType::Html:
    case ProcessedFileType::Xml:
    case ProcessedFileType::Asset:
    case ProcessedFileType::InFile:
        rawContent = fileContent.str();
        break;
    case ProcessedFileType::Uninitialized:
        [[unlikely]]
        throw std::runtime_error(
            "Critical engine bug: internalFileType not initialized."
        );
    default:
        [[unlikely]]
        throw std::runtime_error("Illegal value of internalFileType injected");
    }

    res.content = this->man.env.render(
        rawContent,
        this->man.buildCommonContext(safeFm)
    );

    return res;
}

}
