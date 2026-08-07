#pragma once

namespace violet {
enum class ProcessedFileType {
    Uninitialized,
    Html,
    Markdown,
    Xml,
    Asset,
    /**
     * Special `.in` filetype  
     */
    InFile,
    // Note: if additional types are added to this file, the bound check in toJsonImpl must be updated as well
};
}
