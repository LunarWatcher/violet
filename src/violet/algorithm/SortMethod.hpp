#pragma once

namespace violet {

enum class SortMethod {
    Start, // Special: used for range checks. Will disappear after C++26. DO NOT EXPOSE!
    ByCreationDate,
    ByLastModifiedDate,
    ByTitle,
    End, // Special: same as start. Will disappear after C++26. DO NOT EXPOSE!
};

}
