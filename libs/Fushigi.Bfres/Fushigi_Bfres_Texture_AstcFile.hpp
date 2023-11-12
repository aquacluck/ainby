#include <string>
#include <vector>
#include "Fushigi_Bfres_Common_Enum.hpp"

namespace Fushigi_Bfres::Texture
{
    class AstcFile
    {
    public:
        const int MagicFileConstant = 0x5CA1AB13;

        static void SaveAstc(std::string filePath, Fushigi_Bfres::Common::Enum::SurfaceFormat format, uint32_t width, uint32_t height, std::vector<uint8_t> data);
        static void SaveAstc(std::string filePath, uint8_t blockDimX, uint8_t blockDimY, uint32_t width, uint32_t height, std::vector<uint8_t> data);

    private:
        static std::vector<uint8_t> IntTo3Bytes(int value);
    };
}
