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


#ifndef RLST_CORE_RLST_MCA_PARSER_CHUNK_HPP
#  define RLST_CORE_RLST_MCA_PARSER_CHUNK_HPP

#include <vector>

extern "C" {
  #include <zlib.h>
}

namespace rlst::mca_parser
{
  class Chunk
  {
  private:
    // true if was not generated
    bool notGenerated;
    int posx;
    int posy;
    std::vector<Bytef> uncompressedData;
    std::vector<unsigned char> compressedData;
  public:
    // compression type is encoded in 1 byte
    static const int compressionTypeLength = 1;
    // the length of the compressed data + the size of the compression
    // type is encoded in 4 bytes
    static const int sizeLength = 4;

    Chunk(int posx, int posy, std::vector<Bytef> uncompressedData,
        std::vector<unsigned char> compressedData);

    Chunk();
    void printChunk() const;
    int getPosx() const;
    int getPosy() const;
    void setNotGenerated(bool notGenerated);
    bool wasNotGenerated() const;
    const std::vector<unsigned char>& getCompressedData() const;
    void setCompressedData(std::vector<unsigned char>& compressedData);
    const std::vector<Bytef>& getUncompressedData() const;
  };
}

#endif // RLST_CORE_RLST_MCA_PARSER_CHUNK_HPP
