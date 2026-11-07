#include "SiteServer.hpp"

#include "magpie/config/AppConfig.hpp"
#include "magpie/transfer/StatusCode.hpp"
#include "minilog/logger/ConsoleLogger.hpp"
#include "minilog/minilog.hpp"
#include "violet/generate/watch/WatchRebuildHandler.hpp"
#include <fstream>
#include <sstream>

namespace violet {

SiteServer::SiteServer(const GenerateOpts &generateOpts)
    : context(std::make_shared<ServerContext>(generateOpts)),
      app(
          std::make_shared<magpie::App<ServerContext>>(
              context,
              magpie::AppConfig{
                  .port = 8080,
                  // Beware if you try to patch this IP to accept external connections: magpie has not been tested in
                  // production, and likely has vulnerabilities. Exposing this outside your machine is a risk, and
                  // that's why we don't support it at this time. You have been warned.
                  .bindAddr = "127.0.0.1",
                  .trustXRealIp = false,
                  .adapterFactory = std::make_shared<
                  magpie::Http11AdapterFactory>()
              }
          )
      ),
      isBuildValid(false),
      watcher(std::make_shared<WatchRebuildHandler>(generateOpts, &isBuildValid))
{
    initLogger();
    initRoutes();
    watchThread = std::thread(std::bind(&SiteServer::watchRoot, this));
}

SiteServer::~SiteServer() {
    watchThread.join();
}

void SiteServer::serve() {
    this->app->run();
}

std::string SiteServer::resolveContentType(const std::filesystem::path &file) {
    auto ext = file.extension();

    if (auto it = extContentTypeMap.find(ext); it != extContentTypeMap.end()) {
        return it->second;
    }
    return "text/plain";
}

void SiteServer::handleNotFound(
    const std::filesystem::path &absOutputFolder,
    magpie::Response &res
) {
    const auto notFoundPage = absOutputFolder / "404.html";

    if (std::filesystem::exists(notFoundPage)) {
        std::ifstream f(notFoundPage);
        if (!f) {
            minilog::error("Failed to open {}", notFoundPage.string());
        } else {
            std::stringstream ss;
            ss << f.rdbuf();
            res = magpie::Response(
                magpie::Status::NotFound, ss.str(),
                resolveContentType(notFoundPage)
            );
            return;
        }
    }

    res = magpie::Response(
        magpie::Status::NotFound,
        "Page not found (additionally, no 404.html was found in the output directory)"
    );
}

void SiteServer::initRoutes() {
    app->route<"/{path}", magpie::Method::Get>(
        [this](
            ServerContext *ctx,
            magpie::Request &,
            magpie::Response &res,
            std::string_view path
        ) {
            if (!this->isBuildValid.load()) {
                res = magpie::Response(
                    magpie::Status::InternalServerError,
                    "<html><body>"
                    "<h1>Build error!</h1>"
                    "<p>The site failed to compile (or the first build has not yet completed). See the terminal for more information</p>"
                    "</body></html>",
                    "text/html"
                );
                return;
            }
            // Proper path normalization is an stc TODO
            if (path.contains("../") || path.contains("/..") || path.starts_with('/')) {
                res = magpie::Response(magpie::Status::BadRequest, "Bad");
                return;
            }

            auto absOutputFolder = std::filesystem::canonical(
                std::filesystem::absolute(ctx->generateOpts.root) / ctx->generateOpts.outputFolder
            );
            if (!std::filesystem::exists(absOutputFolder)) {
                handleNotFound(absOutputFolder, res);
            } else {
                auto file = absOutputFolder / path;
                if (std::filesystem::is_directory(file)) {
                    file /= "index.html";
                }
                if (!std::filesystem::exists(file)) {
                    handleNotFound(absOutputFolder, res);
                } else {
                    std::ifstream f(file);
                    if (!f) {
                        minilog::error("Failed to open {}", file.string());
                        handleNotFound(absOutputFolder, res);
                    } else {
                        std::stringstream ss;
                        ss << f.rdbuf();

                        res = magpie::Response(magpie::Status::OK, ss.str(),
                            resolveContentType(file));
                    }
                }
            }
        }
    );
}

void SiteServer::initLogger() {
    static auto logger = minilog::ConsoleLogger("server");
    magpie::logger::config().logger = [](auto level, const auto &msg) {
        switch (level) {
        case magpie::logger::Level::debug:
            logger.debug("{}", msg);
            break;
        case magpie::logger::Level::info:
            logger.info("{}", msg);
            break;
        case magpie::logger::Level::warning:
            logger.warn("{}", msg);
            break;
        case magpie::logger::Level::error:
            logger.error("{}", msg);
            break;
        case magpie::logger::Level::critical:
            logger.critical("{}", msg);
            break;
        }
    };
    minilog::warn(
        "Meow :3 You're using an early preview of violet's serving functionality. It's currently only a "
        "server, and it does not yet have site generation built in. To see what it'll look like when it works, run "
        "`violet generate -p /`. The server reads straight from the filesystem and does not need to be stopped "
        "for changes to be reflected."
    );
    minilog::warn(
        "Note: `violet serve` must not be used in production. Instead, use `violet generate` and point "
        "your favourite reverse proxy at the folder with the generated output. This is exclusively a preview "
        "server, and must be treated as such."
    );
}
void SiteServer::watchRoot() {
    while (true) {
        try {
            if (!this->watcher->watch()) {
                minilog::error("Watch thread shut down unexpectedly!");
                abort();
            }
        } catch (const std::exception& e) {
            minilog::error("Handled uncaught exception: {}", e.what());
        } catch (...) {
            minilog::error("Caught non-exception type");
        }

        minilog::info(
            "waiting 10 seconds before restarting the site generation watcher"
        );
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

}
