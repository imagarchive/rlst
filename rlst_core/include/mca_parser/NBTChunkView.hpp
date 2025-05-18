#ifndef NBTCHUNKVIEW_HPP
#define NBTCHUNKVIEW_HPP

#include <vector>
#include <cstdint>
#include <string>
#include "mca_parser/Chunk.hpp"
#include "nbt.h"
#include "mca_parser/Block.hpp"

class NBTChunkView {
public:
    explicit NBTChunkView(Chunk& chunk);
    ~NBTChunkView();

    bool isValid() const;

    std::string getBlockName(int x, int y, int z) const;
    void setBlock(const Block& block);                  // version qui prend un block
    void setBlocks(const std::vector<Block>& blocks);   // version qui prend une liste de blocs

    std::vector<uint8_t> toCompressedData() const;

private:
    NBT* root = nullptr;
    Chunk& chunk;

    NBT* findSection(int y) const;
    NBT* getOrCreatePaletteEntry(NBT* section, const Block& block, int& outIndex); // modifié pour recevoir Block
    void setBlockStateIndex(NBT* section, int index, int value);
    int getBlockStateIndex(NBT* section, int index, int bitsPerBlock) const;
};

#endif // NBTCHUNKVIEW_HPP
