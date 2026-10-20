#include  "DateTimeFunctions.hpp"
#include "violet/generate/templates/DateTimeFunctions.hpp"
#include "violet/generate/templates/IrisManager.hpp"
#include <format>
#include <chrono>

namespace violet {

DateTimeFunctions::DateTimeFunctions(IrisManager& man) : man(man) {
    man.env.add_callback("formatDate", [this](iris::Arguments& args) {
        return formatDate(args);
    });
    man.env.add_callback("now", 0, [](iris::Arguments&) {
        return (int64_t) std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    });
}

nlohmann::json DateTimeFunctions::formatDate(iris::Arguments& args) {
    auto timestamp = args.at(0)->get<int64_t>();
    auto fmt = args.at(1)->get<std::string>();
    // TODO: site.timezone
    std::string timezone = "UTC";
    if (args.size() == 3) {
        timezone = args.at(2)->get<std::string>();
    } else if (args.size() > 3) {
        // TODO: Create custom error (maybe? or do I just wanwt to handle inja's exceptions better?)
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
