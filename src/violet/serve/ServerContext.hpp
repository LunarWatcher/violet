#pragma once
// Must always be first
#include "violet/serve/meta/include-guard.hpp"

#include "violet/data/GenerateOpts.hpp"

#include "magpie/data/CommonData.hpp"

namespace violet {

struct ServerContext : public magpie::data::CommonData {
    GenerateOpts generateOpts;

    ServerContext(
        const GenerateOpts& generateOpts
    ) : generateOpts(
            generateOpts
        )
    {
        this->generateOpts.postParse();
    }
};

}
