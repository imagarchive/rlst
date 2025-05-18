#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <memory>
#include <vector>
#include <zlib.h>

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

#endif