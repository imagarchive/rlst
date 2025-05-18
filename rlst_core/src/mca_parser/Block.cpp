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


#include "mca_parser/Block.hpp"
#include <iostream>

namespace rlst::mca_parser
{
  Block::Block(int posx, int posy, int posz, std::string name, std::map<std::string, std::string> properties)
    : posx(posx)
    , posy(posy)
    , posz(posz)
    , name(name)
    , properties(properties)
  {}

  Block::Block()
    : posx(0)
    , posy(0)
    , posz(0)
    , name("")
    , properties()
  {}

  void Block::printBlock() const
  {
    std::cout << "block : position : " << posx << "," << posy << "," << posz << " name : " << name << std::endl;
  }

  int Block::getPosx() const
  {
    return posx;
  }

  int Block::getPosy() const
  {
    return posy;
  }

  int Block::getPosz() const
  {
    return posz;
  }

  std::string Block::getName() const
  {
    return name;
  }

  std::map<std::string, std::string> Block::getProperties() const
  {
    return properties;
  }
}
