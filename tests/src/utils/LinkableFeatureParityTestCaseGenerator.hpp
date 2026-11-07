#pragma once

#include "catch2/catch_test_macros.hpp"
#include "parsing/markdown/fixture/MarkdownFixture.hpp"

namespace tests {

enum class UrlType {
    Url,
    Image,
};

struct LinkableFixture : public MarkdownFixture {
    mutable UrlType testType;
    void urlTest() const {
        testType = UrlType::Url;
        testCase();
    }
    void imageTest() const {
        testType = UrlType::Image;
        testCase();
    }

    std::string getMarkdown(const std::string& content, const std::string& url, bool isRef = false) const {
        std::stringstream out;
        switch (testType) {
        case UrlType::Url:
            break;
        case UrlType::Image:
            out << "!";
            break;
        default:
            FAIL("UrlType not set!");
        }
        out << "[" << content << "]";
        if (isRef) {
            out << "[" << url << "]";
        } else {
            out << "(" << url << ")";
        }
        return out.str();
    }

    std::string getHtmlForLinkable(const std::string& content, const std::string& url) const {
        switch (testType) {
        case UrlType::Url:
            return this->url(content, url);
        case UrlType::Image:
            return this->img(content, url);
        default:
            FAIL("UrlType not set!");
        }
    }
    virtual void testCase() const = 0;
};

}

#define INTERNAL_LINKABLE_FEATURE_PARITY_TEST_CASE(TestIdentifier, Name, ...) \
    struct TestIdentifier : public tests::LinkableFixture {             \
        virtual void testCase() const override;                         \
    };                                                                  \
    TEST_CASE_METHOD(TestIdentifier, "Anchor: " Name, "[markdown][a]" __VA_ARGS__) { \
        urlTest();                                                      \
    }                                                                   \
    TEST_CASE_METHOD(TestIdentifier, "Image: " Name, "[markdown][img]" __VA_ARGS__) { \
        imageTest();                                                    \
    }                                                                   \
    void TestIdentifier::testCase() const

#define LINKABLE_FEATURE_PARITY_TEST_CASE(Name, ...)        \
    INTERNAL_LINKABLE_FEATURE_PARITY_TEST_CASE(             \
        INTERNAL_CATCH_UNIQUE_NAME(VIOLET_URL_TEST_IMPL),   \
        Name,                                               \
        __VA_ARGS__                                         \
    )

