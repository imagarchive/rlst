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


#ifndef RLST_RLST_PAR_HPP
#  define RLST_RLST_PAR_HPP

#include <cstdint>

/**
 * @file
 * This file contains the definitions of geometric structures and operations
 *
 * It includes the definition of a @ref Point structure and a @ref Segment
 * class, along with their respective constructors, assignment operators, and
 * utility functions.
 */

namespace par
{
  using literal_t = std::int64_t; ///< A literal

  /* POINT */

  /**
   * A point
   */

  struct Point
  {
    literal_t x; ///< The x-axis component
    literal_t y; ///< The y-axis component
    literal_t z; ///< The z-axis component
  };

  /**
   * Swap two @ref Point "points"
   *
   * @param[in, out] __lhs The left hand side operand
   * @param[in, out] __rhs The right hand side operand
   */

  void swap(Point& __lhs, Point& __rhs);
}

#endif // RLST_RLST_PAR_HPP
