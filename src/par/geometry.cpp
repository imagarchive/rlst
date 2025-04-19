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

    boost::hash_combine(seed, __input.x);
    boost::hash_combine(seed, __input.y);
    boost::hash_combine(seed, __input.z);

    return seed;
  }

  void swap(Point& __lhs, Point& __rhs) noexcept
  {
    using std::swap;

    swap(__lhs.x, __rhs.x);
    swap(__lhs.y, __rhs.y);
    swap(__lhs.z, __rhs.z);
  }

  /* Segment */

  std::size_t hash_value(const Segment& __input)
  {
    std::size_t seed = 0;

    boost::hash_combine(seed, __input.start);
    boost::hash_combine(seed, __input.end);

    return seed;
  }

  void swap(Segment& __lhs, Segment& __rhs) noexcept
  {
    using std::swap;

    swap(__lhs.start, __rhs.start);
    swap(__lhs.end, __rhs.end);
  }
}
