#pragma once

#include "violet/conf/Frontmatter.hpp"
#include "violet/generate/FileManager.hpp"
#include "violet/generate/cache/MetadataCache.hpp"

namespace violet {

class Paginator {
private:

    std::vector<const Frontmatter*> fm;
    size_t offset = 0;

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
            // NOLINTNEXTLINE(bugprone-unchecked-optional-access): A page must have a listing to initialize a paginator
            auto pageSize = paginator.pageFm.listing->pageSize;
            fm = {};
            fm.reserve(pageSize);

            for (size_t i = offset; i < std::min(offset + pageSize, paginator.fm.size()); ++i) {
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
            // NOLINTNEXTLINE(bugprone-unchecked-optional-access): A page must have a listing to initialize a paginator
            auto pageSize = paginator.pageFm.listing->pageSize;
            offset += pageSize;
            ++page;

            if (page != getTotalPages()) {
                setFm();
            }

            return *this;
        }

        bool operator!=(const iterator& other) { return !(*this == other); }
        bool operator==(const iterator& other) {
            // TODO: this is ugly as fuck
            return other.page == this->page;
        }

        size_t getPage() { return page; }
        size_t getTotalPages() {
            return paginator.getTotalPages();
        }
    };

    Paginator(
        const Frontmatter& pageFm,
        FileManager& fileMan,
        MetadataCache& metaCache
    );

    iterator begin() { return iterator(*this, 0); }
    iterator end() { return iterator(*this, getTotalPages()); }

    size_t getTotalPages() {
        // NOLINTNEXTLINE(bugprone-unchecked-optional-access): A page must have a listing to initialize a paginator
        auto pageSize = pageFm.listing->pageSize;
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
