/*
 * Copyright (C) 2025 Mattéo Rossillol‑‑Laruelle <beatussum@protonmail.com>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef RLST_CORE_RLST_MCA_PARSER_NBT_CHUNK_VIEW_HPP
#  define RLST_CORE_RLST_MCA_PARSER_NBT_CHUNK_VIEW_HPP

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

#endif // RLST_CORE_RLST_MCA_PARSER_NBT_CHUNK_VIEW_HPP
