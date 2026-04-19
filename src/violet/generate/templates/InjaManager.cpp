#include "InjaManager.hpp"
#include "inja/environment.hpp"
#include "violet/conf/Frontmatter.hpp"

#include <inja/inja.hpp>

namespace violet {

InjaManager::InjaManager(
    GenerateOpts& opts,
    Config& cfg,
    FileManager& fileManager
) : opts(opts), cfg(cfg), fileManager(fileManager) {
    env.set_include_callback([this](
            const std::filesystem::path& path,
            const std::string& templateName
    ) {
        return env.parse_file(
            this->fileManager.resolvePartial(templateName)
        );
    });
}

std::string InjaManager::renderPage(
    const std::string& fileContent,
    const std::filesystem::path& fileSource,
    const Frontmatter& fm
) {
    nlohmann::json context = {
        {"page", fm},
        {"site", this->cfg.raw},
    };

    inja::Template content = env.parse(fileContent);
    env.include_template("content", content);

    return env.render_file(
        this->fileManager.resolveTemplate(
            fm.type,
            fm.layout
        ),
        context
    );
}

}
