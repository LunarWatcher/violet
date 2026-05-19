#pragma once

#include <sstream>

namespace violet {

struct LockStreamPos {
    std::stringstream& ss;
    size_t pos;

    [[nodiscard("Discarding this object will immediately reset the stream")]]
    LockStreamPos(std::stringstream& ss) : ss(ss) {
        pos = ss.tellg();
    }

    ~LockStreamPos() { revert(); }

    void revert() {
        if (pos != std::numeric_limits<size_t>::max()) {
            ss.clear();
            ss.seekg(pos);
            commit();
        }
    }

    void commit() {
        pos = std::numeric_limits<size_t>::max();
    }
};

}
