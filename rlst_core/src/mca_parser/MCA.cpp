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


#include "mca_parser/MCA.hpp"

#include "mca_parser/Chunk.hpp"

#include <array>
#include <iostream>

extern "C" {
  #include <zlib.h>
}

namespace rlst::mca_parser
{
  MCA::MCA(): mChunks() {}
  std::array<Chunk, MCA::nbChunksInRegion>& MCA::chunks() { return mChunks;  }

  // Reads and uncompresses the data of the chunk corresponding to the given index
  Chunk& readChunkData(std::fstream& file, int chunkIndex)
  {
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
      std::cerr << "The chunk was not generated" << std::endl;
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
    //   throw std::runtime_error("Erreur : Longueur invalide du chunk");
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

  std::array<uint8_t, 3> converts3BytesBigEndian(uint32_t value)
  {
    if (value > 0xFFFFFF) {
      throw std::runtime_error("Value too large");
    }

    return {
      static_cast<uint8_t>((value >> 16) & 0xFF),
      static_cast<uint8_t>((value >> 8)  & 0xFF),
      static_cast<uint8_t>(value & 0xFF)
    };
  }

  uint32_t writeChunkData(std::fstream& file, MCA& mcaFile, int& chunkIndex, uint32_t& chunkStart)
  {
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

    std::array<uint8_t, 3> bigEndianChunkStart = converts3BytesBigEndian(chunkStart);

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
      static_cast<uint8_t>(chunkSize & 0xFF)
    };

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

  void writeData(MCA& mcaFile)
  {
    std::ofstream newFile("newFile.mca", std::ios::binary | std::ios::trunc);
    if (!newFile) {
      throw std::runtime_error("Can not create the new mca file");
    }

    // counting timestamps and offsets
    // = 4096 * 2
    const size_t headerSize = MCA::nbChunksInRegion * 4;

    // set the header (all the chunk's offsets) to 0
    std::array<uint8_t, headerSize / 2> offsets = {};
    newFile.write(reinterpret_cast<const char*>(offsets.data()), offsets.size());

    // set the header (all the chunk's offsets) to 0
    // This will not be modified
    std::array<uint8_t, headerSize / 2> timestamps = {};
    newFile.write(reinterpret_cast<const char*>(timestamps.data()), timestamps.size());

    // close the file to reopen it in read-write mode
    newFile.close();

    std::fstream file("nom.mca", std::ios::in | std::ios::out | std::ios::binary);

    if (!file) {
      throw std::runtime_error("Can not open the new file");
    }

    // for each chunk in the mca file, write the compressed data into the new mca file
    std::array<Chunk, MCA::nbChunksInRegion> chunks = mcaFile.chunks();

    // where to write the first chunk
    uint32_t chunkStart = headerSize;
    for (int i = 0; i < chunks.size(); i++) {
      // if (chunks[i].wasNotGenerated()) {
      //   // TODO : what do we have to do ???
      //   continue;
      // }
      chunkStart = writeChunkData(file, mcaFile, i, chunkStart);
    }
  }

  // Reads a .mca file and returns a MCA object containing the uncompressed data of the chunks
  // This code needs to use r.0.0.mca
  MCA readMcaFile(const std::string &filename)
  {
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
}
