#include "RenderResult.hpp"

namespace violet {

RenderResult::operator iris::DataObject() {
    return iris::Object {
        {"content", content}
    };
}

}
