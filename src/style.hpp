#pragma once

#include <unordered_map>
#include <imgui.h>
#include "types.h"

// Hold render settings and simple resources in here
class Style {
public:
    // enum for font use cases
    inline static const u8 FONT_UI = 0;
    inline static const u8 FONT_NODE_GRAPH = 1;
    inline static std::unordered_map<u8, ImFont*> fonts;
};
