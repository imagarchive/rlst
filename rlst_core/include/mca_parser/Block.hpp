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

#include "geometry.hpp"
#include <unordered_map>

namespace rlst::mca_parser
{
  class Block
  {
  public:
    using properties_type = std::unordered_map<std::string, std::string>;
  public:
    Block() = default;
    Block(const Block& __other) = default;
    Block(Block&& __other) = default;
    ~Block() = default;
  public:
    Block& operator=(const Block& __rhs) = default;
    Block& operator=(Block&& __rhs) noexcept = default;
  public:
    Block(
      Point __position,
      std::string __name,
      std::unordered_map<std::string, std::string> __properties
    )
      : m_name(std::move(__name))
      , m_position(std::move(__position))
      , m_properties(std::move(__properties))
    {}
  public:
    std::string_view name() const noexcept { return m_name; }
    Point position() const noexcept { return m_position; }
    const properties_type& properties() const noexcept { return m_properties; }
  private:
    std::string m_name;
    Point m_position;
    properties_type m_properties;
  };
}

#endif // RLST_CORE_RLST_MCA_PARSER_BLOCK_HPP
