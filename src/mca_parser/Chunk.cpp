#include <iostream>
#include "Chunk.hpp"

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

