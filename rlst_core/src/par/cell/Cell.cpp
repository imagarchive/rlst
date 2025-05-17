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


#include "par/cell/Cell.hpp"
#include <boost/functional/hash.hpp>

namespace rlst::par::cell
{
  std::size_t hash_value(const Cell& __input)
  {
    std::size_t seed = 0;

    boost::hash_combine(seed, __input.m_position);
    boost::hash_combine(seed, __input.m_type);

    return seed;
  }

  void swap(Cell& __lhs, Cell& __rhs) noexcept
  {
    using std::swap;

    swap(__lhs.m_position, __rhs.m_position);
    swap(__lhs.m_type, __rhs.m_type);
  }
}
