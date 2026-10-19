#include "minilog/minilog.hpp"
#include <catch2/catch_session.hpp>

int main(int argc, const char* argv[]) {
    auto sess = Catch::Session();
    minilog::setLevel(minilog::Level::Debug);
    return sess.run(argc, argv);
}
