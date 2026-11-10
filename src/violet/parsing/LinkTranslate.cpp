#include "LinkTranslate.hpp"
#include "violet/exceptions/FatalError.hpp"
#include "violet/parsing/Escaping.hpp"

#include "stc/uri.hpp"
#include "violet/path/path.hpp"

#include <filesystem>
#include <minilog/minilog.hpp>

bool violet::checkPathValidity(
    const std::filesystem::path& relSourceFilePath,
    const std::string& in,
    const std::filesystem::path& projectRoot
) {
    std::filesystem::path expectedLocation = projectRoot;
    if (in.starts_with('/')) {
        expectedLocation += in;
    } else {
        expectedLocation /= std::filesystem::is_directory(projectRoot / relSourceFilePath)
            ? relSourceFilePath
            : relSourceFilePath.parent_path();
        expectedLocation /= in;
    }

    expectedLocation = std::filesystem::weakly_canonical(expectedLocation);

    if (!std::filesystem::exists(expectedLocation)) {
        if (expectedLocation.filename().string().starts_with('_')) {
            minilog::debug(
                "Possibly dead link: {} links to \"{}\", which was expected at {}. However, this appears to be "
                "a reserved filename or folder (starts with _), so likely fine.",
                relSourceFilePath.string(),
                in,
                expectedLocation.string()
            );
        } else {
            minilog::warn(
                "Possibly dead link: {} links to \"{}\", which was expected at {}",
                relSourceFilePath.string(),
                in,
                expectedLocation.string()
            );
        }
        return false;
    }

    if (std::filesystem::is_directory(expectedLocation)) {
        if (
            !std::filesystem::exists(expectedLocation / "README.md")
            && !std::filesystem::exists(expectedLocation / "index.md")
            && !std::filesystem::exists(expectedLocation / "index.html")
        ) {
            minilog::warn(
                "Possibly dead link: {} links to {}, which is a valid folder, but it doesn't contain anything "
                "that resolves to `index.html` after compilation (found none of: README.md, index.md, index.html)",
                relSourceFilePath.string(),
                in
            );
            return false;
        }
    }
    
    return true;
}

std::string violet::projectBasedTranslator(
    const std::string& pathPrefix,
    const std::filesystem::path& relSourceFilePath,
    const std::optional<std::filesystem::path>& projectRoot,
    const std::string& in
) {
    if (relSourceFilePath.is_absolute()) {
        // This should never be thrown in production; this may occur in incorrectly configured tests, in which case, fix
        // the test to not provide a relSourceFilePath that's absolute
        [[unlikely]]
        throw std::runtime_error(
            std::format(
                "Developer error: projectBasedTranslator got relSourceFilePath as an absPath. This is incorrect. "
                "Got relSourceFilePath = {}. If you are an end user, this is a severe bug. Open an issue",
                relSourceFilePath.string()
            )
        );
    } else if (in == "") {
        minilog::error("Found empty link in {}!", relSourceFilePath.string());
        return in;
    }

    // // We need an absolute path, which we don't (shouldn't) get from the outside explicitly. SiteGenerator runs on
    // // project root-relative paths, so this function matches that.
    // //
    // // If this ends up not being relative, that means std::filesystem::relative fails, because it has nothing to anchor
    // // to. Specifically, std::filesystem::relative(p, ...) fails when `p` is relative, and is NOT relative to the
    // // current working directory. If `--root` is passed, `projectRoot` creates `relSourceFilePaths` that do not exist
    // // relative to the current working directory, thereby causing `std::filesystem::relative to return an empty string
    // if (
    //     in.starts_with("//")
    //     || in.starts_with("http://")
    //     || in.starts_with("https://")
    //     // || !in.contains(".md")
    // ) {
    //     return in;
    // }

    auto uri = stc::uri(in);

    if (
        uri.components().scheme.has_value()
        || uri.components().host.has_value()
        || !uri.components().path.has_value()
    ) {
        return in;
    }

    if (projectRoot.has_value()) {
        checkPathValidity(
            relSourceFilePath,
            uri.components().path.value(),
            *projectRoot
        );
    }

    auto path = *uri.components().path;
    violet::path p(in.starts_with('/') ? path : relSourceFilePath.parent_path().string());

    if (!in.starts_with('/')) {
        p.join(path);
    }

    if (p.filename().ends_with(".md")) {
        if (p.filename() == "README.md") {
            p.replace_filename("index.html");
        } else {
            p.replace_filename(
                renameFile(
                    p.filename()
                )
            );
        }
    }

    std::string rawPath = p.concatPrefix(pathPrefix);
    std::stringstream escapedUrl;
    urlEncode(
        rawPath,
        escapedUrl,
        false
    );

    // Not sure if we really want to do this, 
    if (uri.components().query) {
        escapedUrl << "?";
        urlEncode(
            *uri.components().query,
            escapedUrl,
            false
        );
    }
    if (uri.components().fragment) {
        escapedUrl << "#";
        urlEncode(
            *uri.components().fragment,
            escapedUrl,
            false
        );
    }

    // 0.20.0: redisabled because of \abspath`: lots and lots of things pass through this now, so this adds so much
    // output
    // minilog::debug(
    //     "Rewrote URL {} to {} in file {}",
    //     in,
    //     newPath,
    //     relSourceFilePath.string()
    // );
    return escapedUrl.str();
}

std::string violet::stringifyAndTranslateUrl(
    std::stringstream& url
) {
    std::string out = url.str();

    // TODO: would be cleaner to do this with stream manipulation, but not sure how
    // This is also not perfect, but it's good enough for now
    if (
        !out.starts_with("http://")
        && !out.starts_with("https://")
        && (
            !out.starts_with('/')
            || (out.starts_with('/') && !out.starts_with("//"))
        )
        && out.ends_with(".md")
    ) {
        auto sub = out.substr(0, out.size() - 3);
        return sub + ".html";
    }
    return out;
}

std::string violet::renameFile(std::string_view sourceFileName) {
    auto dot = sourceFileName.rfind('.');
    if (dot == std::string::npos) {
        [[unlikely]]
        throw FatalError(
            std::format(
                "{} is not a markdown file, but was passed to renameFile anyway",
                sourceFileName
            )
        );
    }
    std::stringstream ss;
    for (size_t i = 0; i < dot; ++i) {
        auto ch = sourceFileName.at(i);
        if (
            (ch >= 'a' && ch <= 'z')
            || (ch >= '0' && ch <= '9')
            || ch == '-'
            || ch == '_'
            || ch == '.'
        ) {
            ss << ch;
        } else if (
            (ch >= 'A' && ch <= 'Z')
        ) {
            ss << (char) std::tolower(ch);
        } else if (ch == ' ') {
            ss << "-";
        }
    }

    if (ss.tellp() == 0) {
        throw std::runtime_error(
            std::format(
                "File name {} contains no latin letters, and cannot be translated",
                sourceFileName
            )
        );
    }
    ss << ".html";
    return ss.str();
}
