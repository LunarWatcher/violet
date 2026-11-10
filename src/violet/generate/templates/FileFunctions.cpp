#include "FileFunctions.hpp"

#include "IrisManager.hpp"
#include "iris/exceptions.hpp"
#include "violet/algorithm/SortMethod.hpp"
#include "violet/conf/Frontmatter.hpp"
#include "violet/data/Constants.hpp"
#include "violet/data/RenderResult.hpp"
#include "violet/exceptions/SyntaxError.hpp"
#include "violet/generate/templates/ext/SafeCastEnum.hpp"
#include "violet/generate/templates/types/RenderContext.hpp"
#include "violet/generate/templates/types/TreePagesData.hpp"
#include "violet/paginator/SortPredicates.hpp"
#include "violet/parsing/LinkTranslate.hpp"
#include "violet/parsing/Markdown.hpp"
#include "violet/path/path.hpp"
#include "violet/transitional/JsonToDataObject.hpp"
#include "violet/util/ErrorHandling.hpp"
#include <filesystem>

namespace violet {

FileFunctions::FileFunctions(IrisManager& man) : man(man) {
    man.env.add_callback("listPages", [this](iris::Arguments& args, void* udata) {
        return listPages(args, (RenderContext*) udata);
    });
    man.env.add_callback("listPagesPaginated", [this](iris::Arguments& args, void* udata) {
        return listPagesPaginated(args, (RenderContext*) udata);
    });
    man.env.add_callback("treePages", [this](iris::Arguments& args, void* udata) {
        return treePages(args, (RenderContext*) udata);
    });
    man.env.add_callback("paginatedUrl", 2, [this](iris::Arguments& args) {
        return paginatedUrl(args);
    });
    man.env.add_callback("listPagesByTaxonomy", 2, [this](auto& args) {
        return listPagesByTaxonomy(args);
    });
    man.env.add_callback("loadJson", 1, [this](auto& args, void* udata) {
        return loadJson(args, (RenderContext*) udata);
    });
    man.env.add_callback("abspath", [this](auto& args, void* udata) {
        return abspath(args, (RenderContext*) udata);
    });
    man.env.add_callback("render", 1, [this](auto& args) {
        return render(args);
    });
}

iris::DataObject FileFunctions::listPagesByTaxonomy(iris::Arguments& args) {
    auto& root = man.cfg.root();
    auto taxonomy = args.at(0)->get<iris::String>();
    auto value = args.at(1)->get<iris::String>();

    iris::Array pages;
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
                man.cfg.root()
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

iris::DataObject FileFunctions::listPages(iris::Arguments& args, RenderContext* ctx) {
    auto scope = args.at(0)->get<iris::String>();
    auto sortMethod = SortMethod::ByTitle;
    if (args.size() == 2) {
        auto opt = safeCastEnum<SortMethod>(args.at(1)->get<iris::Int>());
        if (opt) {
            sortMethod = *opt;
        } else {
            throw iris::CallbackError(
                "listPages got invalid sort method: make sure you use the constants in violet.sort"
            );
        }
    }

    if (scope.contains("..")) {
        [[unlikely]]
        throw iris::CallbackError(
            "listPages does not accept paths that recurse upwards"
        );
    }

    // TODO: error handling
    bool isAbsPath = scope.starts_with('/');

    auto path = man.cfg.root();
    if (!isAbsPath) {
        path /= ctx->fm.internalPath;
    }

    if (std::filesystem::is_regular_file(path)) {
        path = path.parent_path();
    }

    path = std::filesystem::weakly_canonical(
        path / (
            // I hate all path implementations and I desperately need to write my own rather than deal with this shit
            isAbsPath ? scope.substr(1) : scope
        )
    );

    SingleSortCacheKey cacheKey {
        sortMethod,
        path.string()
    };

    if (auto it = this->listPagesCache.find(cacheKey); it != listPagesCache.end()) {
        [[likely]]
        return it->second;
    }

    std::vector<Frontmatter> fms;
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
                man.cfg.root()
            );
            auto& metadata = man.metaCache.loadMetadata(
                filePath
            );
            if (!metadata.frontmatter.isAsset && !metadata.frontmatter.hidden) {
                fms.push_back(metadata.frontmatter);
            }
            return true;
        }
    );
    std::sort(
        fms.begin(),
        fms.end(),
        sort::sortMethodsRef.at(sortMethod)
    );

    this->listPagesCache[cacheKey] = fms;
    return fms;
}

iris::DataObject FileFunctions::treePages(iris::Arguments& args, RenderContext* ctx) {
    auto scope = args.at(0)->get<iris::String>();

    auto sortMethod = SortMethod::ByTitle;
    if (args.size() >= 2) {
        auto opt = safeCastEnum<SortMethod>(args.at(1)->get<iris::Int>());
        if (opt) {
            sortMethod = *opt;
        } else {
            [[unlikely]]
            throw iris::CallbackError(
                "treePages got invalid sort method: make sure you use the constants in violet.sort"
            );
        }
    }
    auto leafSortMethod = sortMethod;
    if (args.size() >= 3) {
        auto opt = safeCastEnum<SortMethod>(args.at(2)->get<iris::Int>());
        if (opt) {
            leafSortMethod = *opt;
        } else {
            [[unlikely]]
            throw iris::CallbackError(
                "treePages got invalid leafSortMethod: make sure you use the constants in violet.sort"
            );
        }
    }

    if (scope.contains("..")) {
        [[unlikely]]
        throw iris::CallbackError(
            "treePages does not accept paths that recurse upwards"
        );
    }

    bool isAbsPath = scope.starts_with('/');

    // TODO: error handling
    auto path = man.cfg.root();
    if (!isAbsPath) {
        path /= ctx->fm.internalPath;
    }

    if (std::filesystem::is_regular_file(path)) {
        path = path.parent_path();
    }

    path = std::filesystem::weakly_canonical(
        path / (
            // I hate all path implementations and I desperately need to write my own rather than deal with this shit
            isAbsPath ? scope.substr(1) : scope
        )
    );

    DualSortCacheKey cacheKey {
        leafSortMethod,
        sortMethod,
        path.string()
    };

    if (auto it = treePagesCache.find(cacheKey); it != treePagesCache.end()) {
        [[likely]]
        return it->second;
    }

    TreePagesData root;

    if (std::filesystem::weakly_canonical(man.cfg.root()) != path) {
        root.folder_name = path.filename();
    }

    bool hasFiles = false;
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
            // This path must only be used for accessing the frontmatter
            auto filePath = std::filesystem::relative(
                entryPath,
                man.cfg.root()
            );
            // This past must only be used for deriving the relative path to the frontmatter, i.e. determining where
            // things go in the output object
            auto rootRelativePath = std::filesystem::relative(
                entryPath,
                path
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

            TreePagesData* next = &root;

            if (rootRelativePath.has_parent_path()) {
                for (auto& segment : rootRelativePath.parent_path()) {
                    if (!next->subfolders.contains(segment)) {
                        next->subfolders[segment] = TreePagesData{
                            segment
                        };
                    }
                    next = &((*next).subfolders[segment]);
                }
            }

            if (filePath.filename() == "README.md" || filePath.filename() == "index.md") {
                (*next).index = metadata.frontmatter;
            } else {
                (*next).items.push_back(metadata.frontmatter);
            }
            hasFiles = true;
            return true;
        }
    );
    if (!hasFiles) {
        return iris::Object();
    }
    auto data = root.convert(
        leafSortMethod,
        sortMethod
    );

    treePagesCache[cacheKey] = data;
    return data;
}

iris::DataObject FileFunctions::paginatedUrl(iris::Arguments& args) {
    auto baseUrl = args.at(0)->get<iris::String>();
    auto page = args.at(1)->get<iris::Int>();
    // This restricts us to half the max uint64_t value, but no one will ever care
    if (page <= 0) {
        throw iris::CallbackError("Invalid page number");
    }

    std::stringstream ss;
    ss << baseUrl;
    if (!baseUrl.ends_with('/')) {
        ss << "/";
    }

    ss << "page/" << page << "/index.html";
    return ss.str();
}
iris::DataObject FileFunctions::abspath(iris::Arguments& args, RenderContext* ctx)
{
    const auto& fm = ctx->fm;
    const auto& conf = this->man.cfg;

    const auto& path = args.at(0)->get<iris::String>();

    violet::path rootPath = path.starts_with('/') ? "/" : fm.internalPath;

    auto rawResult = rootPath.parent_path()
        .join(path);

    bool transform;
    if (args.size() ==  2) {
        transform = args.at(1)->get<bool>();
    } else {
        transform = path.find('%') == std::string::npos;
    }

    if (transform) {
        // TODO: refactor projectBasedTranslator to use violet::path so we don't do all of this here (and to use
        // stc::uri, it does most of the heavy lifting on # and ? parsing that's still left in there manually)
        // We could in theory delegate both these scenarios to the translator for somewhat more consistent code
        return projectBasedTranslator(
            conf.prefix,
            fm.internalPath,
            this->man.cfg.root(),
            rawResult.to_string()
        );
    } else {
        return rawResult
            .concatPrefix(conf.prefix);
    }
}

iris::DataObject FileFunctions::listPagesPaginated(iris::Arguments& args, RenderContext* ctx) {
    const auto& frontmatter = ctx->fm;
    const auto scope = args.at(0)->get<iris::String>();

    const auto sortMethod = safeCastEnum<SortMethod>(
        args.at(1)->get<iris::Int>()
    );

    if (!sortMethod.has_value()) {
        throw iris::CallbackError(
            "listPagesPaginated got illegal sortMethod. Make sure you're using one of the values in violet.sort."
        );
    }

    uint64_t pageSize = 50;
    uint64_t pageNumber = 1;
    if (args.size() > 2) {
        pageSize = args.at(2)->get<iris::Int>();
    }
    if (args.size() > 3) {
        pageNumber = args.at(3)->get<iris::Int>();
    }


    Paginator pag {
        frontmatter.internalPath,
        this->man.fileManager,
        this->man.metaCache,
        pageSize,
        scope,
        *sortMethod
    };
    if (pageNumber == 0) {
        throw iris::CallbackError("Page numbers are 1-indexed");
    }
    --pageNumber;

    if (pageNumber >= pag.getTotalPages()) {
        // Page out of range
        return {};
    }
    iris::DataObject out = iris::Array{};
    auto page = pag.page(pageNumber);
    for (const auto* fm : *page) {
        out.push_back(*fm);
    }

    return out;
}

iris::DataObject FileFunctions::loadJson(iris::Arguments& args, RenderContext* ctx) {
    std::string fn = args.at(0)->get<iris::String>();
    auto& frontmatter = ctx->fm;

    std::filesystem::path p;
    if (fn.starts_with('/')) {
        if (fn.starts_with("//")) {
            throw iris::CallbackError("Absolute paths cannot start with two slashes!");
        }
        p = this->man.cfg.root() / fn.substr(1);
    } else {
        p = std::filesystem::path {
            this->man.cfg.root() / frontmatter.internalPath
        };
        if (!std::filesystem::is_directory(p)) {
            p = p.parent_path();
        }
        p /= fn;
    }

    // This feels bad, but it should work.
    // Should probably turn this into a standard function somewhere
    p = std::filesystem::weakly_canonical(p);
    if (!p.string().starts_with(
            this->man.cfg.root().string()
    )) {
        minilog::error(
            "loadJson found possible root escaping: fn = {}, resolved path = {}",
            fn, p.string()
        );
        throw iris::CallbackError(
           "loadJson found possible root escaping. See log messages."
        );
    }

    std::ifstream f(p);
    if (!f)  {
        minilog::error(
            "Failed to open {}", p.string()
        );
        throw iris::CallbackError("failed to open file");
    }

    nlohmann::json data;
    try {
        f >> data;
    } catch (...) {
        minilog::error("Failed to parse json in {}", p.string());
        throw;
    }
    return transitional::convertJson(data);
}

iris::DataObject FileFunctions::render(iris::Arguments& args) {
    auto fmObj = args.at(0)->get<iris::Object>();
    auto fm = Frontmatter::fromDataObject(fmObj);

    // TODO: desperately need better out of bounds path checking
    if (fm.internalPath.starts_with('/') || fm.internalPath.contains("..")) {
        throw iris::CallbackError("Path passed to render breaks project containment");
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
                    fm.internalPath,
                    this->man.cfg.root(),
                    std::placeholders::_1
                ),
                this->man.cfg.tocStartLevel,
                this->man.cfg.markdown
            );
            rawContent = page.parsedContents;
            safeFm.tableOfContents = page.tableOfContents;
        } catch (const SyntaxError& synError) {
            renderSyntaxError(
                fm.internalPath,
                synError,
                fileContent
            );
            throw iris::CallbackError(
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
        throw iris::CallbackError(
            "Critical engine bug: internalFileType not initialized."
        );
    default:
        [[unlikely]]
        throw iris::CallbackError("Illegal value of internalFileType injected");
    }

    auto context = this->man.buildCommonContext(safeFm);
    try {
        res.content = this->man.env.render(
            rawContent,
            context,
            &context,
            fm.internalPath
        );
    } catch (const iris::IrisError& e) {
        renderIrisError(
            e,
            path
        );
        throw iris::CallbackError(
            std::format(
                "Failed to render {} due to nested render errors",
                path.string()
            )
        );
    }

    return res;
}

void FileFunctions::clear() {
    this->treePagesCache.clear();
    this->listPagesCache.clear();
}

}
