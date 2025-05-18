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


#ifndef RLST_CORE_RLST_MCA_PARSER_BLOCK_HPP
#  define RLST_CORE_RLST_MCA_PARSER_BLOCK_HPP

#include <string>
#include <map>

namespace rlst::mca_parser
{
  class Block
  {
  private:
    int posx;
    int posy;
    int posz;
    std::string name;
    std::map<std::string, std::string> properties;
  public:
    Block(int posx, int posy, int posz, std::string name, std::map<std::string, std::string> properties);
    Block();
    void printBlock() const;
    int getPosx() const;
    int getPosy() const;
    int getPosz() const;
    std::string getName() const;
    std::map<std::string, std::string> getProperties() const;
  };
}

#endif // RLST_CORE_RLST_MCA_PARSER_BLOCK_HPP
