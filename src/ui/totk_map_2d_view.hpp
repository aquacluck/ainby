#pragma once

#include "Fushigi_Bfres_Common_Enum.hpp"
#include "Fushigi_Bfres_Common_Structs.hpp"
#include "Fushigi_Bfres_Texture_AstcFile.hpp"
#include "Fushigi_Bfres_Texture_BntxFile.hpp"
#include "Fushigi_Bfres_Texture_BntxTexture.hpp"
#include "Fushigi_Bfres_Texture_Swizzling_TegraX1Swizzle.hpp"

class TotkMap2DView {
public:
    static void Init();
    static void Draw();
};
