#ifndef MCA_HPP
#define MCA_HPP

#include "Chunk.hpp"
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
Chunk readChunkData(std::fstream& file, int chunkIndex);

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

#endif