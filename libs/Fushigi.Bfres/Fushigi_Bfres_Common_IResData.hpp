#pragma once

#include <iostream>

namespace Fushigi_Bfres::Common
{
    class IResData
    {
        virtual void Read(std::basic_istream<uint8_t> &reader);
    };
}
