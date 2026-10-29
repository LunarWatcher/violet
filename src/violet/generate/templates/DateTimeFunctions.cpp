#include  "DateTimeFunctions.hpp"
#include "iris/exceptions.hpp"
#include "violet/generate/templates/DateTimeFunctions.hpp"
#include "violet/generate/templates/IrisManager.hpp"
#include "violet/parsing/DateTimeParsing.hpp"
#include <chrono>

namespace violet {

DateTimeFunctions::DateTimeFunctions(IrisManager& man) : man(man) {
    man.env.add_callback("formatDate", [this](iris::Arguments& args) {
        return formatDate(args);
    });
    man.env.add_callback("parseDate", [this](iris::Arguments& args) {
        return parseDate(args);
    });
    man.env.add_callback("now", 0, [](iris::Arguments&) {
        return (int64_t) std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    });
}

iris::DataObject DateTimeFunctions::parseDate(iris::Arguments& args) {
    auto date = args.at(0)->get<iris::String>();

    if (args.size() >= 2) {
        auto fmt = args.at(1)->get<iris::String>();

        return ::violet::parseDate(date, fmt);
    } else {
        return parseISO8601(date);
    }
}

iris::DataObject DateTimeFunctions::formatDate(iris::Arguments& args) {
    if (!args.at(0)->is_int()) {
        throw iris::ContextlessRenderError(
            "user", // TODO: temporary, iris should provide better types
            "formatDate() requires that the first arg is a number, but got a string. "
            "If you have a string source date, use parseDate() on the string to convert it"
        );
    }
    auto timestamp = args.at(0)->get<iris::Int>();
    auto fmt = args.at(1)->get<iris::String>();
    std::string timezone = man.cfg.timezone;
    if (args.size() == 3) {
        timezone = args.at(2)->get<iris::String>();
    } else if (args.size() > 3) {
        // TODO: Create custom error (maybe? or do I just want to handle inja's exceptions better?)
        throw std::runtime_error("formatDate takes 2 or 3 arguments, but got 4");
    }

    return ::violet::formatDate(timestamp, fmt, timezone);
}

}
