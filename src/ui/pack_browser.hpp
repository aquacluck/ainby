#pragma once

#include <optional>
#include "file_formats/sarc.hpp"
#include "file_index_cache.hpp"

class PackBrowser {
public:
    static std::optional<std::string> Draw(SARC &currentSarc);
    static std::optional<std::string> DrawPackTree(const std::vector<std::string> &fileList);
};
