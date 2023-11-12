#pragma once

#include <iostream>
#include <map>
#include <ranges>
#include <string>
#include "Fushigi_Bfres_Common_IResData.hpp"


namespace Fushigi_Bfres::Common
{
    //template<IResData T>
    template<class T>
    class ResDict
    {
    public:
        std::map<std::string, T> resDictMap;

        ResDict() { }

        T operator[](int index)
        {
            auto key = this->GetKey(index);
            if (key != "") {
                return resDictMap[key];
            }
            std::cout << "bad index " << index << "\n";
            throw std::runtime_error("index not found");
        }

        std::string GetKey(int index)
        {
            int i = 0;
            for(std::string key : std::views::keys(resDictMap)) {
                std::cout << "internal texture name " << key << "\n";
                if (i == index) {
                    return key;
                }
                i++;
            }
            return "";
        }

        void Read(std::basic_istream<uint8_t> &reader)
        {
            // XXX unused, or consumed by other reads?
            /*
            reader.ReadUInt32(); //magic
            int numNodes = reader.ReadInt32();

            List<Node> nodes = new List<Node>();

            int i = 0;
            for (; numNodes >= 0; numNodes--)
            {
                nodes.Add(new Node()
                {
                    Reference = reader.ReadUInt32(),
                    IdxLeft = reader.ReadUInt16(),
                    IdxRight = reader.ReadUInt16(),
                    Key = reader.ReadStringOffset(reader.ReadUInt64()),
                });
                i++;
            }

            for (int j = 1; j < nodes.Count; j++)
                this->Add(nodes[j].Key, new T());
            */
        }

    protected:
        class Node
        {
        public:
            uint32_t Reference;
            uint16_t IdxLeft;
            uint16_t IdxRight;
            std::string Key;
            IResData Value;
        };
    };
}
