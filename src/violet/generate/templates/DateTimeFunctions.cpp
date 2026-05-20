#include  "DateTimeFunctions.hpp"
#include "violet/generate/templates/DateTimeFunctions.hpp"
#include "violet/generate/templates/InjaManager.hpp"
#include <format>
#include <chrono>

namespace violet {

DateTimeFunctions::DateTimeFunctions(InjaManager& man) : man(man) {
    man.env.add_callback("formatDate", [this](inja::Arguments& args) {
        return formatDate(args);
    });
}

nlohmann::json DateTimeFunctions::formatDate(inja::Arguments& args) {
    auto timestamp = args.at(0)->get<int64_t>();
    auto fmt = args.at(1)->get<std::string>();
    // TODO: site.timezone
    std::string timezone = "UTC";
    if (args.size() == 3) {
        timezone = args.at(2)->get<std::string>();
    } else if (args.size() > 3) {
        throw std::runtime_error("formatDate takes 2 or 3 arguments, but got 4");
    }

    auto tp =
        std::chrono::zoned_time(
            timezone,
            std::chrono::system_clock::time_point(
                std::chrono::seconds(timestamp)
            )
        );
    return std::vformat(
        "{:" + fmt + "}",
        std::make_format_args(
            tp
        )
    );
}

}
