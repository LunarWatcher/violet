#pragma once

#include "violet/conf/Frontmatter.hpp"
#include "violet/generate/FileManager.hpp"

namespace violet {

class Paginator {
private:
    const Frontmatter& pageFm;
    FileManager& fileMan;

    std::vector<Frontmatter*> fm;
    size_t offset = 0;

    friend class iterator;
public:
    struct iterator {
    public:
        using iterator_category = std::forward_iterator_tag;

        using value_type = std::vector<Frontmatter*>;
        using reference = const std::vector<Frontmatter*>&;
        using pointer = const std::vector<Frontmatter*>*;

        std::vector<Frontmatter*> fm;
    private:
        Paginator& pag;
        size_t offset = 0;
        size_t page;
    public:
        iterator(Paginator& pag, size_t startBatch = 0) : pag(pag), offset(startBatch) {}

        reference operator*() { return fm; }
        pointer operator->() { return &fm; }

        iterator& operator++() {
            // NOLINTNEXTLINE(bugprone-unchecked-optional-access): A page must have a listing to initialize a paginator
            auto pageSize = pag.pageFm.listing->pageSize;
            offset += pageSize;
            ++page;

            fm = {};
            fm.reserve(pageSize);

            for (size_t i = offset; i <= std::min(offset + pageSize, pag.fm.size()); ++i) {
                fm.push_back(
                    pag.fm.at(i)
                );
            }
            return *this;
        }

        bool operator!=(const iterator& other) { return !(*this == other); }
        bool operator==(const iterator& other) {
            // TODO: this is ugly as fuck
            return other.offset == this->offset
                || (
                    // TODO: this will likely cause off by one errors, but there's an off by 1 error we want: page 1
                    // must always be generated, even if there are no pages, as this controls the generation of the
                    // index page for page_lists. This needs to be tested when this->fm is populated
                    other.offset == std::numeric_limits<size_t>::max() && this->offset > fm.size()
                );
        }

        size_t getPage() { return page; }
        size_t getTotalPages() {
            // NOLINTNEXTLINE(bugprone-unchecked-optional-access): A page must have a listing to initialize a paginator
            auto pageSize = pag.pageFm.listing->pageSize;
            // TODO: this isn't a great solution
            return std::max<size_t>(
                1,
                std::ceil(
                    (double) pag.fm.size() / (double) pageSize
                )
            );
        }
    };

    Paginator(
        const Frontmatter& pageFm,
        FileManager& fileMan
    );

    iterator begin() { return iterator(*this, 0); }
    iterator end() { return iterator(*this, std::numeric_limits<size_t>::max()); }
};

}
