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


#include "par/geometry.hpp"
#include <boost/functional/hash.hpp>

namespace rlst::par
{
  /* Point */

  std::size_t hash_value(const Point& __input)
  {
    std::size_t seed = 0;

    boost::hash_combine(seed, __input.m_x);
    boost::hash_combine(seed, __input.m_y);
    boost::hash_combine(seed, __input.m_z);

    return seed;
  }

  void swap(Point& __lhs, Point& __rhs) noexcept
  {
    using std::swap;

    swap(__lhs.m_x, __rhs.m_x);
    swap(__lhs.m_y, __rhs.m_y);
    swap(__lhs.m_z, __rhs.m_z);
  }

  /* Segment */

  std::size_t hash_value(const Segment& __input)
  {
    std::size_t seed = 0;

    boost::hash_combine(seed, __input.m_start);
    boost::hash_combine(seed, __input.m_end);

    return seed;
  }

  void swap(Segment& __lhs, Segment& __rhs) noexcept
  {
    using std::swap;

    swap(__lhs.m_start, __rhs.m_start);
    swap(__lhs.m_end, __rhs.m_end);
  }
}
