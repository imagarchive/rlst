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


#include "par/cell/types.hpp"

namespace rlst::par::cell
{
  void swap(CellType& __lhs, CellType& __rhs) noexcept
  {
    using std::swap;

    swap(__lhs.ports, __rhs.ports);
    swap(__lhs.name, __rhs.name);
    swap(__lhs.size, __rhs.size);
  }
}
