#pragma once

namespace violet {

enum class SortMethod {
    Start, // Special: used for range checks. Will disappear after C++26. MUST NOT BE EXPOSED!
    ByCreationDate,
    ByLastModifiedDate,
    End, // Special: same as start. Will disappear after C++26. DO NOT EXPOSE!
};

}
