#include "NBTChunkView.hpp"
#include <iostream>
#include <cmath>
#include <cstring>

NBTChunkView::NBTChunkView(Chunk& chunkRef) : chunk(chunkRef) {
    const auto& data = chunk.getUncompressedData();
    root = NBT_Parse(const_cast<uint8_t*>(data.data()), data.size());
    if (!root) {
        std::cerr << "Erreur : parsing NBT échoué." << std::endl;
    }
}

NBTChunkView::~NBTChunkView() {
    if (root) NBT_Free(root);
}

bool NBTChunkView::isValid() const {
    return root != nullptr;
}

NBT* NBTChunkView::findSection(int y) const {
    // NBT* level = NBT_GetChild(root, "Level");
    // if (!level) return nullptr;

    NBT* sections = NBT_GetChild(root, "sections");
    if (!sections || sections->type != TAG_List) return nullptr;

    NBT* section = sections->child;
    // because sections start with Y=-4
    int64_t neededY = (y / 16) - 4;
    if (neededY < 0) {
        // fix a problem with negative integers
        neededY += 256;
    }
    while (section) {
        NBT* yTag = NBT_GetChild(section, "Y");
        if (yTag && yTag->type == TAG_Byte && yTag->value_i == neededY) {
            return section;
        }
        section = section->next;
    }
    return nullptr;
}

NBT* NBTChunkView::getOrCreatePaletteEntry(NBT* section, const Block& block, int& outIndex) {
    const std::string& name = block.getName();
    const auto& props = block.getProperties();

    NBT* blockStates = NBT_GetChild(section, "block_states");
    NBT* palette = NBT_GetChild(blockStates, "palette");

    if (!palette || palette->type != TAG_List) return nullptr;

    int index = 0;
    NBT* entry = palette->child;
    while (entry) {
        NBT* nameTag = NBT_GetChild(entry, "Name");
        if (nameTag && nameTag->type == TAG_String && std::string((char*)nameTag->value_a.value) == name) {
            outIndex = index;
            return palette;
        }
        ++index;
        entry = entry->next;
    }

    // creates the entry to add to the palette
    NBT* newEntry = new NBT{ TAG_Compound, nullptr };
    newEntry->child = nullptr;

    NBT* nameTag = new NBT{ TAG_String, strdup("Name") };
    nameTag->value_a.value = strdup(name.c_str());
    nameTag->value_a.len = static_cast<int32_t>(name.length()) + 1;

    newEntry->child = nameTag;

    // adds block's properties
    if (!props.empty()) {
        NBT* propTag = new NBT{ TAG_Compound, strdup("Properties") };
        propTag->child = nullptr;
        NBT* lastProp = nullptr;
        for (const auto& [k, v] : props) {
            NBT* p = new NBT{ TAG_String, strdup(k.c_str()) };
            p->value_a.value = strdup(v.c_str());
            p->value_a.len = static_cast<int32_t>(v.length()) + 1;
            p->next = nullptr;

            if (!propTag->child) {
                propTag->child = p;
            } else {
                lastProp->next = p;
                p->prev = lastProp;
            }
            lastProp = p;
        }

        nameTag->next = propTag;
        propTag->prev = nameTag;
    }

    // adds to the palette
    newEntry->next = nullptr;
    newEntry->prev = nullptr;

    if (!palette->child) {
        palette->child = newEntry;
    } else {
        NBT* last = palette->child;
        while (last->next) last = last->next;
        last->next = newEntry;
        newEntry->prev = last;
    }

    outIndex = index;
    return palette;
}

void NBTChunkView::setBlock(const Block& block) {
    if (this->chunk.wasNotGenerated()) {
        throw std::runtime_error(
            "The chunk was not generated, impossible to add a block");
    }

    int x = block.getPosx();
    int y = block.getPosy();
    int z = block.getPosz();

    // checks the block's position is included into the chunk
    if (x < 0 || x >= 16 || y < 0 || y >= 256 || z < 0 || z >= 16) return;

    NBT* section = findSection(y);
    
    if (!section) return;

    int paletteIndex = -1;

    NBT* palette = getOrCreatePaletteEntry(section, block, paletteIndex);
    if (!palette || paletteIndex < 0) return;

    NBT* blockStates = NBT_GetChild(section, "block_states");
    NBT* data = NBT_GetChild(blockStates, "data");

    int bitsPerBlock = std::ceil(std::log2(paletteIndex + 1));
    if (bitsPerBlock < 4) bitsPerBlock = 4;

    int blockIndex = (y % 16) * 256 + z * 16 + x;
    int startBit = blockIndex * bitsPerBlock;
    int startLong = startBit / 64;
    int bitOffset = startBit % 64;

    int64_t* longs = (int64_t*)data->value_a.value;

    int64_t mask = ((1LL << bitsPerBlock) - 1);

    longs[startLong] &= ~(mask << bitOffset);
    longs[startLong] |= ((int64_t)paletteIndex << bitOffset);

    if (bitOffset + bitsPerBlock > 64) {
        int spillBits = (bitOffset + bitsPerBlock) - 64;
        longs[startLong + 1] &= ~(mask >> (bitsPerBlock - spillBits));
        longs[startLong + 1] |= ((int64_t)paletteIndex >> (bitsPerBlock - spillBits));
    }

    this->toCompressedData();
}

void NBTChunkView::setBlocks(const std::vector<Block>& blocks) {
    for (const auto& block : blocks) {
        setBlock(block);
    }
}

std::vector<uint8_t> NBTChunkView::toCompressedData() const {
    std::vector<uint8_t> buffer(1 << 20); // 1 MiB
    size_t size = buffer.size();

    int res = NBT_Pack_Opt(root, buffer.data(), &size, NBT_Compression_ZLIB, nullptr);
    if (res != 0) {
        std::cerr << "Error while re-compressing NBT: code " << res << std::endl;
        return {};
    }

    buffer.resize(size);

    std::vector<unsigned char> converted(buffer.begin(), buffer.end());
    this->chunk.setCompressedData(buffer);

    return buffer;
}