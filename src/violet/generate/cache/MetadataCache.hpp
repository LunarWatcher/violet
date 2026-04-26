#pragma once

#include "violet/conf/Frontmatter.hpp"
#include "violet/generate/FileManager.hpp"
#include <string>
#include <unordered_map>
#include <fstream>

namespace violet {

struct Metadata {
    Frontmatter frontmatter;
    size_t frontmatterByteOffset;
};

struct OpenData {
    const Metadata& metadata;
    std::ifstream file;
};

class MetadataCache {
private:
    std::unordered_map<std::string, std::shared_ptr<Metadata>> fileToMetadataCache;

    FileManager& fileManager;
public:

    MetadataCache(
        FileManager& fileManager
    ) : fileManager(fileManager) {}

    const Metadata& loadMetadata(
        const std::filesystem::path& relPath
    ) {
        if (auto it = fileToMetadataCache.find(relPath); it != fileToMetadataCache.end()) {
            return *it->second;
        } else {
            auto result = openFile(relPath);
            return result.metadata;
        }
    }

    OpenData openFile(
        const std::filesystem::path& relPath
    ) {
        std::ifstream f(fileManager.getRootFolder() / relPath);
        if (!f) {
            throw std::runtime_error(
                "Failed to open " + relPath.string()
            );
        }
        std::shared_ptr<Metadata> metadata;
        if (auto it = fileToMetadataCache.find(relPath); it != fileToMetadataCache.end()) {
            metadata = it->second;
            f.seekg(metadata->frontmatterByteOffset);
        } else {
            nlohmann::json frontmatter = parseFrontmatter(f);
            Frontmatter fm = frontmatter;
            fm.withFilePath(relPath);

            metadata = std::make_shared<Metadata>(
                std::move(fm),
                f.tellg()
            );
            this->fileToMetadataCache[relPath.string()] = metadata;
        }

        return OpenData {
            *metadata,
            std::move(f)
        };
    }

    nlohmann::json parseFrontmatter(std::ifstream& in) {
        // we need to re-do the check already done on HTML, since we allow markdown to not contain frontmatter
        std::string lineBuff;
        std::getline(in, lineBuff);

        if (lineBuff != "---") {
            // Markdown; frontmatter is empty. Reset input stream
            in.seekg(0);
            in.clear();
            return nlohmann::json::object();
        }
        std::stringstream json;
        while (std::getline(in, lineBuff)) {
            if (lineBuff == "---") {
                if (json.tellp() == 0) {
                    // No JSON
                    // This is not particularly robust, but good enough for now.
                    // Parts of this will be substituted by an error once the parsing is error-handled.
                    return nlohmann::json::object();
                }
                // end of frontmatter reached
                nlohmann::json out;
                std::cout << json.str() << std::endl;
                json >> out;
                return out;
            }
            json << lineBuff << std::endl;
        }
        // EOF with no frontmatter terminator. Return null
        // TODO: ... and log an error for the user.
        // TODO: do we also want to output to file, or just log at that point? I'm in favour of CLI errors, but I'm not sure
        // if that scales well if there's hundreds of errors. I am somewhat in favour of a report file of sorts though, that
        // compiles an aggregate set of the errors and where they're at.
        return nullptr;
    }

};

}
