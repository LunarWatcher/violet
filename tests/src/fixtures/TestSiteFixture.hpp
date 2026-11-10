#pragma once

#include "fixtures/DebugMarkerFixture.hpp"
#include "fixtures/LoadWorkspace.hpp"
#include "violet/data/GenerateOpts.hpp"
#include "violet/generate/SiteGenerator.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <stdexcept>

namespace tests {

// Copied from https://github.com/LunarWatcher/magpie/blob/ac46b65f772e3c9f1fe755d4dd850f3d171ed430/src/magpie/routing/Compile.hpp#L19
// Begin copied block {{{
template <size_t N>
struct FixedString {
    std::array<char, N + 1> data{};
    const size_t size = N;

    constexpr FixedString(const char (&str)[N + 1]) {
        std::copy_n(str, N + 1, std::data(data));
    }

    constexpr explicit FixedString(const char* begin) {
        std::copy_n(begin, N, std::data(data));
        data.back() = '\0';
    }
    constexpr char operator[](size_t idx) const {
        if (idx >= size) {
            throw std::runtime_error("Invalid string index");
        }
        return *(data.data() + idx);
    }

    constexpr auto c_str() const -> char const* { return std::data(data); }
    constexpr std::string string() const {
        return std::string(data.data(), N);
    }
};

template <std::size_t N>
FixedString(const char (&str)[N]) -> FixedString<N - 1>;
// End copied block }}}

template <
    SiteVariant Site,
    bool AutoGen = false,
    FixedString prefix = "",
    FixedString BuildDir = "pages"
>
struct TestSiteFixture : public DebugMarkerFixture {
    tests::LoadWorkspace workspace;
    violet::GenerateOpts opts;
    std::shared_ptr<violet::SiteGenerator> gen;

    TestSiteFixture()
        : workspace(Site, BuildDir.string()),
          opts(
              {
                  .watch = false,
                  .overridePrefixForLocalUse = prefix.string() == "",
                  .overrideOutputFolder = workspace.rootDirectory / BuildDir.string(),
                  .root = workspace.rootDirectory,
                  .overridePrefix = prefix.string() == "" ? std::nullopt : std::optional(prefix.string()),
              }
          )
    {
        std::cout << "Now initializing test site at "
                  << workspace.rootDirectory
                  << std::endl;
        auto result = violet::SiteGenerator::loadWorkspace(
            opts
        );
        REQUIRE(result.has_value());

        this->gen = *result;

        if (AutoGen) {
            generateWithAsserts();
        }
    }

    virtual ~TestSiteFixture() = default;

    violet::SiteGenerator* operator*() { return gen.get(); }
    violet::SiteGenerator* operator->() { return gen.get(); }

    const std::filesystem::path& buildPath() const { return workspace.buildDirectory.folder; }
    const std::filesystem::path& sourcePath() const { return workspace.rootDirectory; }

    void generateWithAsserts() const {
        if (!gen->generate()) {
            throw std::runtime_error(
                std::format(
                    "Generate of test site {} failed", static_cast<int>(Site)
                )
            );
        }
        if (!std::filesystem::exists(buildPath())) {
            throw std::runtime_error(
                std::format(
                    "Generate of test site {} allegedly succeeded, but buildPath() is missing", static_cast<int>(Site)
                )
            );
        }
    }

    std::filesystem::path output() const override {
        return buildPath();
    }
};

using MonolithRealPrefix = tests::TestSiteFixture<tests::SiteVariant::TestMonolith, true, "/prefix", "pages-prefix">;

}
