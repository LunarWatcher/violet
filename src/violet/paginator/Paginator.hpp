#pragma once

#include "violet/algorithm/SortMethod.hpp"
#include "violet/conf/Frontmatter.hpp"
#include "violet/generate/FileManager.hpp"
#include "violet/generate/cache/MetadataCache.hpp"

namespace violet {

class Paginator {
private:

    std::vector<const Frontmatter*> fm;
    size_t offset = 0;
    const size_t pageSize;

    friend class iterator;
public:
    const Frontmatter& pageFm;
    FileManager& fileMan;

    struct iterator {
    public:
        using iterator_category = std::forward_iterator_tag;

        using value_type = std::vector<const Frontmatter*>;
        using reference = const std::vector<const Frontmatter*>&;
        using pointer = const std::vector<const Frontmatter*>*;

        std::vector<const Frontmatter*> fm;
        Paginator& paginator;
    private:
        size_t offset = 0;
        size_t page;

        void setFm() {
            fm = {};
            fm.reserve(paginator.pageSize);

            for (size_t i = offset; i < std::min(offset + paginator.pageSize, paginator.fm.size()); ++i) {
                fm.push_back(
                    paginator.fm.at(i)
                );
            }
        }
    public:
        iterator(Paginator& pag, size_t startBatch = 0) : paginator(pag), page(startBatch) {
            setFm();
        }

        reference operator*() { return fm; }
        pointer operator->() { return &fm; }

        iterator& operator++() {
            offset += paginator.pageSize;
            ++page;

            if (page != getTotalPages()) {
                setFm();
            }

            return *this;
        }

        bool operator!=(const iterator& other) { return !(*this == other); }
        bool operator==(const iterator& other) { return other.page == this->page; }

        size_t getPage() { return page; }
        size_t getTotalPages() {
            return paginator.getTotalPages();
        }
    };

    Paginator(
        const Frontmatter& pageFm,
        FileManager& fileMan,
        MetadataCache& metaCache,
        const std::filesystem::path& prefix = ".",
        SortMethod sortMethod = SortMethod::ByCreationDate
    );

    Paginator(
        const Frontmatter& pageFm,
        FileManager& fileMan,
        MetadataCache& metaCache,
        size_t pageSize,
        const std::filesystem::path& prefix = ".",
        SortMethod sortMethod = SortMethod::ByCreationDate
    );

    iterator begin() { return iterator(*this, 0); }
    iterator end() { return iterator(*this, getTotalPages()); }

    iterator page(size_t startPage) {
        if (startPage >= getTotalPages()) {
            throw std::runtime_error("Illegal start page");
        }
        return iterator(*this, startPage);
    }

    size_t getTotalPages() {
        // TODO: this isn't a great solution
        return std::max<size_t>(
            1,
            std::ceil(
                (double) fm.size() / (double) pageSize
            )
        );
    }
};

}
