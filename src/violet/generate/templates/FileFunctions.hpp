#pragma once

#include <nlohmann/json.hpp>
#include <inja/inja.hpp>

namespace violet {

class InjaManager;
class FileFunctions {
private:
    InjaManager& man;
public:
    FileFunctions(InjaManager& man);

    nlohmann::json listPages(inja::Arguments& args);
    nlohmann::json treePages(inja::Arguments& args);

    nlohmann::json listPagesByTaxonomy(inja::Arguments& args);

    nlohmann::json listPagesPaginated(inja::Arguments& args);

    nlohmann::json paginatedUrl(inja::Arguments& args);
};

}
