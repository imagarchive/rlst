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
#include "mca_parser/MCA.hpp"
#include "mca_parser/NBTChunkView.hpp" 

int main()
{
    std::cout << "Hello world!" << std::endl;

    std::string mcaFile = "r.0.0.mca";

    MCA file = readMcaFile(mcaFile);

    // gets the first chunk
    NBTChunkView view(file.chunks()[0]);

    // creates blocks
    std::map<std::string, std::string> properties;
    Block newBlock(0, 4, 0, "minecraft:redstone_block", properties);
    Block newBlock2(0, 4, 1, "minecraft:redstone_block", properties);
    Block newBlock3(1, 4, 0, "minecraft:redstone_block", properties);
    std::vector<Block> blocks = {newBlock, newBlock2, newBlock3};

    view.setBlocks(blocks);

    // //places a block
    // view.setBlock(newBlock);

    writeData(file);

    return 0;
}
