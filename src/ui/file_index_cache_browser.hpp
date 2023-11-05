#pragma once

#include <optional>
#include "file_index_cache.hpp"

class FileIndexCacheBrowser {
public:
    static std::optional<FileIndexCacheEntry> Draw(FileIndexCache& index);
};
