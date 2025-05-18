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


#include <iostream>
#include "mca_parser/Chunk.hpp"

Chunk::Chunk(int posx, int posy, std::vector<Bytef> uncompressedData,
    std::vector<unsigned char> compressedData)
  : posx(posx)
  , posy(posy)
  , uncompressedData(std::move(uncompressedData))
  , compressedData(std::move(compressedData))
  , notGenerated(false)
{}

Chunk::Chunk()
: posx(0)
, posy(0)
, uncompressedData()
{}

bool Chunk::wasNotGenerated() const {
    return notGenerated;
}

void Chunk::setCompressedData(std::vector<unsigned char>& compressedData) {
    this->compressedData = std::move(compressedData);
}

void Chunk::printChunk() const
{
    std::cout << "chunk : position : " << posx << "," << posy << std::endl;
}

int Chunk::getPosx() const
{
    return posx;
}

int Chunk::getPosy() const
{
    return posy;
}

void Chunk::setNotGenerated(bool notGenerated) {
    this->notGenerated = notGenerated;
}

const std::vector<Bytef>& Chunk::getUncompressedData() const
{
    return uncompressedData;
}

const std::vector<unsigned char>& Chunk::getCompressedData() const {
    return compressedData;
}
