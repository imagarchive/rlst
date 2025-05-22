#include "Chunk.hpp"
#include "MCA.hpp"
#include "NBTChunkView.hpp"

#include <array>
#include <iostream>
#include <iomanip>
#include <zlib.h>
#include <map>



MCA::MCA(): mChunks() {}

std::array<Chunk, MCA::nbChunksInRegion>& MCA::chunks() { return mChunks;  }


// Reads and uncompresses the data of the chunk corresponding to the given index
Chunk readChunkData(std::fstream& file, int chunkIndex) {
    std::size_t chunksNb = MCA::nbChunksInRegion;
    // In the header of an .mca file, a chunk location is encoded with 4 bytes. 
    // The first 3 bytes for the position of the chunk's data in the file, it is 
    // a number of sectors (of 4096 bytes). 
    // The fourth byte of the chunk location is the size of the compressed data,
    // it is a number of sectors.

    // finds the chunk location in the header corresponding to the chosen chunk.
    int indexOffset = chunkIndex * 4;

    // Places the cursor to the beginning of the data corresponding 
    // to the corresponding chunk.
    file.seekg(indexOffset);
    
    // Gets the chunk's location in the mca file.
    unsigned char chunkLocation[4];
    file.read(reinterpret_cast<char *>(chunkLocation), 4);

    if (chunkLocation[0] == 0 && chunkLocation[1] == 0 && chunkLocation[2] == 0 && chunkLocation[3] == 0) {
        std::vector<Bytef> uncompressedData;
        std::vector<unsigned char> compressedData;
        // std::cerr << "The chunk was not generated" << std::endl;
        Chunk newChunk(chunkIndex % chunksNb, chunkIndex / chunksNb, uncompressedData, compressedData);
        return newChunk;
    }

    // Computes the offset (in number of sectors) and converts little endian at the same time
    int chunkOffset = ((chunkLocation[0] << 16) | (chunkLocation[1] << 8) | chunkLocation[2]);
    // Converts in bytes
    chunkOffset *= 4096;

    // Gets the size (in number of sectors) of the chosen chunk
    int chunkSizeSectors = chunkLocation[3];
    // Converts in bytes
    int maxChunkSize = chunkSizeSectors * 4096;

    // Places the cursor to the beginning of the data corresponding 
    // to the first chunk.
    file.seekg(chunkOffset);
    unsigned char lengthData[4];

    // The 4 first bytes is the length of the chunk, including 
    // the type of compression and the compressed data.
    // (file.read moves the cursor)
    file.read(reinterpret_cast<char *>(lengthData), 4);

    // Computes length and converts to little endian
    // This length is in bytes
    int chunkLength = (lengthData[0] << 24) | (lengthData[1] << 16) | 
                      (lengthData[2] << 8) | lengthData[3];

    // Checks length is valid
    // if (chunkLength > maxChunkSize || chunkLength <= 0) {
    //     throw std::runtime_error("Erreur : Longueur invalide du chunk");
    // }

    // Reads the compression type (1 byte)
    unsigned char compressionType;
    file.read(reinterpret_cast<char *>(&compressionType), sizeof(compressionType));

    // 2 is Zlib
    if (compressionType != 2) {
        throw std::runtime_error("The compression type is not zlib");
    }

    // Reads the compressed data (chunkLength - 1 because we don't count compression type)
    std::vector<unsigned char> compressedData(chunkLength - 1);
    file.read(reinterpret_cast<char *>(compressedData.data()), chunkLength - 1);

    // estimated size for the decompressed data
    uLongf uncompressedDataSize =  1024 * 100;
    
    // Creates an array to store the uncompressed data of the chunk
    // https://refspecs.linuxbase.org/LSB_3.0.0/LSB-Core-generic/LSB-Core-generic/zlib-uncompress-1.html
    std::vector<Bytef> uncompressedData(uncompressedDataSize);
    int res = uncompress(uncompressedData.data(), &uncompressedDataSize, compressedData.data(), compressedData.size());

    if (res != Z_OK) {
        throw std::runtime_error("Decompression error");
    }

    // Creates the chunk object
    // this position can be computed this way only because the region position is 0 0
    // (Only one region is needed so r.0.0.mca was chosen here)
    Chunk newChunk(chunkIndex % chunksNb, chunkIndex / chunksNb, uncompressedData, compressedData);

    return newChunk;
}

std::array<uint8_t, 3> converts3BytesBigEndian(uint32_t value) {
    if (value > 0xFFFFFF) {
        throw std::runtime_error("Value too large");
    }

    return {static_cast<uint8_t>((value >> 16) & 0xFF),
        static_cast<uint8_t>((value >> 8)  & 0xFF),
        static_cast<uint8_t>(value & 0xFF)};
}

uint32_t writeChunkData(std::fstream& file, MCA& mcaFile, int& chunkIndex, uint32_t& chunkStart) {
    Chunk chunk = mcaFile.chunks()[chunkIndex];

    // Writes the chunk offset and size in the header :
    // Total size of the chunk in bytes
    size_t totalChunkSize = chunk.getCompressedData().size() + Chunk::compressionTypeLength 
    + Chunk::sizeLength;

    // not counting timestamps
    // = 4096
    size_t headerSize = MCA::nbChunksInRegion * 4;

    // Computes the size (in sectors)
    // rounded up to the nearest whole number
    size_t sizeInSectors = (totalChunkSize + headerSize - 1) / headerSize;

    
    // 4 bytes per chunk in the header
    int positionInHeader = 4 * chunkIndex;
    
    std::array<uint8_t, 3> bigEndianChunkStart = converts3BytesBigEndian(chunkStart / headerSize);
    
    std::array<uint8_t, 4> chunkInHeader;
    chunkInHeader[0] = bigEndianChunkStart[0];
    chunkInHeader[1] = bigEndianChunkStart[1];
    chunkInHeader[2] = bigEndianChunkStart[2];
    chunkInHeader[3] = static_cast<uint8_t>(sizeInSectors);

    // places the cursor at the right position in the header for the chunk
    file.seekp(positionInHeader, std::ios::beg);
    file.write(reinterpret_cast<const char*>(chunkInHeader.data()), 4);

    // places the cursor at chunk's position in the mca file
    file.seekp(chunkStart, std::ios::beg);

    size_t chunkSize = chunk.getCompressedData().size() + Chunk::compressionTypeLength;

    // Converts size to big endian
    std::array<uint8_t, Chunk::sizeLength> chunkSizeBigEndian = {
        static_cast<uint8_t>((chunkSize >> 24) & 0xFF),
        static_cast<uint8_t>((chunkSize >> 16) & 0xFF),
        static_cast<uint8_t>((chunkSize >> 8)  & 0xFF),
        static_cast<uint8_t>(chunkSize & 0xFF)};

    file.write(reinterpret_cast<const char*>(chunkSizeBigEndian.data()), Chunk::sizeLength);

    // Writes the compression type, 2 is zlib
    uint8_t compressionType = 2;
    file.write(reinterpret_cast<const char*>(&compressionType), 1);

    // Writes the chunk's compressed data
    file.write(reinterpret_cast<const char*>(chunk.getCompressedData().data()), 
    chunk.getCompressedData().size());

    uint32_t newChunkStart = chunkStart + headerSize * sizeInSectors;
    return newChunkStart;
}

void writeData(MCA& mcaFile) {
    std::ofstream newFile("newFile.mca", std::ios::binary | std::ios::trunc);
    if (!newFile) {
        throw std::runtime_error("Can not create the new mca file");
    }

    // counting timestamps and offsets
    // = 4096 * 2
    const size_t headerSize = MCA::nbChunksInRegion * 8;

    // set the header (all the chunk's offsets) to 0
    std::array<uint8_t, headerSize / 2> offsets = {};
    newFile.write(reinterpret_cast<const char*>(offsets.data()), offsets.size());

    // set the header (all the chunk's offsets) to 0
    // This will not be modified
    std::array<uint8_t, headerSize / 2> timestamps = {};
    newFile.write(reinterpret_cast<const char*>(timestamps.data()), timestamps.size());

    // close the file to reopen it in read-write mode
    newFile.close();

    std::fstream f("newFile.mca", std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);

    if (!f) {
        throw std::runtime_error("Can not open the new file");
    }

    // for each chunk in the mca file, write the compressed data into the new mca file
    std::array<Chunk, MCA::nbChunksInRegion> chunks = mcaFile.chunks();
    
    // where to write the first chunk
    uint32_t chunkStart = headerSize;
    for (int i = 0; i < chunks.size(); i++) {
        // if (chunks[i].wasNotGenerated()) {
        //     // TODO : what do we have to do ???
        //     continue;
        // }
        chunkStart = writeChunkData(f, mcaFile, i, chunkStart);
    }
}

// Reads a .mca file and returns a MCA object containing the uncompressed data of the chunks
// This code needs to use r.0.0.mca
MCA readMcaFile(const std::string &filename) {
    // Reads the file in binary mode
    std::fstream file(filename, std::ios::in | std::ios::out | std::ios::binary);
    // error if no file
    if (!file) {
        throw std::runtime_error("Can not open the file " + filename);
    }

    // Creates a new MCA object
    MCA mcaFile {};

    std::size_t chunksNb = MCA::nbChunksInRegion;
    for (int i = 0; i < chunksNb; ++i) {
        // fills the mca object with the chunks data
        mcaFile.chunks()[i] = readChunkData(file, i);
    }

    return mcaFile;
}

// The coordinated of the block have to be relative to the minecraft map,
// not a chunk
void placeBlock(MCA& mcaFile, Block& block) {
    int x = block.getPosx();
    // z and y are inverted
    int y = block.getPosz();
    int z = block.getPosy();

    // find the chunk where the block has to be placed
    int chunkX = x / 16;
    int chunkZ = z / 16;

    if (chunkX > 32 || chunkZ > 32) {
        std::cerr << "The block is not in this region" << std::endl;
        return;
    }

    // Get the corresponding chunk
    int chunkIndex = (chunkX) + (chunkZ) * 32;

    Chunk chunk = mcaFile.chunks()[chunkIndex];

    NBTChunkView view(chunk);

    // Computes the position of the block in the chunk
    int xInChunk = x % 16;
    int yInChunk = y;
    int zInChunk = z % 16;

    block.setPosX(xInChunk);
    block.setPosY(yInChunk);
    block.setPosZ(zInChunk);

    // places the block
    view.setBlock(block);
}

