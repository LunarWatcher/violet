#pragma once
// Must always be first
#include "meta/include-guard.hpp"

#include "magpie/App.hpp"
#include "violet/data/GenerateOpts.hpp"
#include "violet/serve/ServerContext.hpp"

#include <filesystem>
#include <memory>

namespace violet {

class SiteServer {
private:
    std::atomic<bool> isBuildValid;

    std::shared_ptr<ServerContext> context;
    std::shared_ptr<magpie::App<ServerContext>> app;
    
    std::thread watchThread;

    const std::unordered_map<std::string, std::string> extContentTypeMap = {
        {".html", "text/html"},
        {".js", "text/javascript"},
        {".mjs", "text/javascript"},
        {".css", "text/css"},
        {".png", "image/png"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".jfif", "image/jpeg"},
        {".svg", "image/svg+xml"},
        {".webp", "image/webp"},
        {".xml", "application/xml"},
        {".rss", "application/rss+xml"},
        {".atom", "application/atom+xml"},
        {".json", "application/json"},
    };

    std::string resolveContentType(const std::filesystem::path &file);

    void handleNotFound(
        const std::filesystem::path &absOutputFolder,
        magpie::Response &res
    );

    void initRoutes();

    void initLogger();

    void watchRoot();

public:
    SiteServer(const GenerateOpts &generateOpts);

    ~SiteServer();

    void serve();
};

}
