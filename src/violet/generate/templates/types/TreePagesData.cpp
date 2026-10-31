#include "TreePagesData.hpp"
#include "violet/paginator/SortPredicates.hpp"

namespace violet {

iris::DataObject TreePagesData::convert(SortMethod leafSort, SortMethod folderSort) {
    std::vector<TreePagesData> rawFlatSubfolders;

    std::sort(items.begin(), items.end(), sort::sortMethodsRef.at(leafSort));

    rawFlatSubfolders.reserve(subfolders.size());
    for (const auto& [_, v] : subfolders) {
        rawFlatSubfolders.push_back(v);
    }

    std::sort(
        rawFlatSubfolders.begin(),
        rawFlatSubfolders.end(),
        [&](const TreePagesData& a, const TreePagesData& b) -> bool {
            if (a.index && b.index) {
                return sort::sortMethodsRef.at(folderSort)(*a.index, *b.index);
            } else if (!a.index && !b.index) {
                return a.folder_name < b.folder_name;
            }

            switch (folderSort) {
            case SortMethod::ByCreationDate:
            case SortMethod::ByLastModifiedDate: {
                // Sort index-havers before index havern'ts
                return a.index.has_value();
            }
            case SortMethod::ByTitle: {
                auto aName = a.index ? a.index->title : a.folder_name;
                auto bName = b.index ? b.index->title : b.folder_name;
                // TODO: not optimal
                return aName < bName;
            }
            default:
                std::unreachable();
            }
        }
    );

    std::vector<iris::DataObject> flatSubfolders;
    flatSubfolders.reserve(rawFlatSubfolders.size());

    for (auto& it : rawFlatSubfolders) {
        flatSubfolders.push_back(it.convert(leafSort, folderSort));
    }
    
    return iris::Object {
        {"folder_name", folder_name},
        {"items", items},
        {"subfolders", flatSubfolders},
        {"index", index.has_value() ? iris::DataObject(*index) : iris::DataObject(nullptr) }
    };
}


}
