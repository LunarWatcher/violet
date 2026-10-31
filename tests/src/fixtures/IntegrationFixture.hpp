#pragma once

#include "fixtures/DebugMarkerFixture.hpp"
#include "fixtures/LoadWorkspace.hpp"
#include "violet/data/GenerateOpts.hpp"
#include "violet/generate/SiteGenerator.hpp"
namespace tests {

// Copied from past pain and suffering with much more cursed templating than here:
// https://github.com/LunarWatcher/magpie/blob/ac46b65f772e3c9f1fe755d4dd850f3d171ed430/src/magpie/routing/Compile.hpp#L19
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
    constexpr std::string string () const { return std::string { c_str(), N }; }
};
template <std::size_t N>
FixedString(const char (&str)[N]) -> FixedString<N - 1>;
// End copied block }}}

template <
    SiteVariant Site,
    bool AutoGen = false,
    FixedString BuildDirName = "pages"
>
struct IntegrationFixture : public DebugMarkerFixture {
    const tests::LoadWorkspace workspace;
    const violet::GenerateOpts opts;
    mutable std::shared_ptr<violet::SiteGenerator> gen;

    IntegrationFixture(
    ) : workspace(Site, BuildDirName.string()),
        opts({
            .watch = false,
            .outputFolder = workspace.rootDirectory / BuildDirName.string(),
            .overridePrefixForLocalUse = true,
            .root = workspace.rootDirectory,
        })
    {
        auto result = violet::SiteGenerator::loadWorkspace(
            opts
        );
        REQUIRE(result.has_value());

        this->gen = *result;
        if (AutoGen) {
            generateWithAsserts();
        }
    }

    violet::SiteGenerator* operator*() { return gen.get(); }
    violet::SiteGenerator* operator->() { return gen.get(); }

    const std::filesystem::path& buildPath() const {
        return workspace.buildDirectory.folder;
    }
    const std::filesystem::path& sourcePath() const {
        return workspace.rootDirectory;
    }

    void generateWithAsserts() const {
        REQUIRE_FALSE(std::filesystem::exists(buildPath()));
        REQUIRE_NOTHROW(gen->generate());
        REQUIRE(std::filesystem::exists(buildPath()));
    }

    void assertGenerated() {
        INFO("gen->generate must be called before this section is declared");
        REQUIRE(std::filesystem::exists(buildPath()));
    }

    std::filesystem::path output() const override {
        return buildPath();
    }

    const IntegrationFixture<Site, AutoGen, BuildDirName>& self() const {
        return *this;
    }
};

}
