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
#include "par/cell/ports.hpp"

#include <boost/functional/hash.hpp>

namespace rlst::par::cell
{
  /* Port */

  std::size_t hash_value(const Port& __input)
  {
    std::size_t seed = 0;

    boost::hash_combine(seed, __input.position);
    boost::hash_combine(seed, __input.type);

    return seed;
  }

  void swap(Port& __lhs, Port& __rhs) noexcept
  {
    using std::swap;

    swap(__lhs.position, __rhs.position);
    swap(__lhs.type, __rhs.type);
  }

  /* PlacedPort */

  std::size_t hash_value(const PlacedPort& __input)
  {
    std::size_t seed = 0;

    boost::hash_combine(seed, __input.parent);
    boost::hash_combine(seed, __input.port);

    return seed;
  }

  void swap(PlacedPort& __lhs, PlacedPort& __rhs) noexcept
  {
    using std::swap;

    swap(__lhs.parent, __rhs.parent);
    swap(__lhs.port, __rhs.port);
  }

  literal_t absolute_x(const PlacedPort& __placed_port)
  {
    return __placed_port.port.position.x() + __placed_port.parent->position.x();
  }

  literal_t absolute_y(const PlacedPort& __placed_port)
  {
    return __placed_port.port.position.y() + __placed_port.parent->position.y();
  }
}
