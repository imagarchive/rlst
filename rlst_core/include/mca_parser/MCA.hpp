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


#ifndef RLST_CORE_RLST_MCA_PARSER_MCA_HPP
#  define RLST_CORE_RLST_MCA_PARSER_MCA_HPP

#include "mca_parser/Chunk.hpp"
#include <iostream>
#include <memory>

#include <array>
#include <fstream>
#include <vector>
#include <cstdint>

class MCA
{
    public:
        static constexpr std::size_t nbChunksInRegion  = 32 * 32;
    public:
        MCA();
    public:
        std::array<Chunk, MCA::nbChunksInRegion>& chunks();
    private:
        std::array<Chunk, nbChunksInRegion> mChunks;
};

// Reads and uncompresses the data of the chunk corresponding to the given index
Chunk& readChunkData(std::fstream& file, int chunkIndex);

// writes the compressed data of the chunk corresponding to the given index
void writeData(MCA& mcaFile);

// writes a chunk in the mca file
// chunkStart is where to write the chunk's data in the mca file
// returns the "chunkstart" of the next chunk
uint32_t writeChunkData(std::fstream& file, MCA& mcaFile, int& chunkIndex, uint32_t& chunkStart);

// converts a value to big endian on 3 bytes
std::array<uint8_t, 3> converts3BytesBigEndian(uint32_t value);

// Reads a .mca file and returns a MCA object containing the uncompressed data of the chunks
// This code needs to use r.0.0.mca
MCA readMcaFile(const std::string &filename);

#endif // RLST_CORE_RLST_MCA_PARSER_MCA_HPP
