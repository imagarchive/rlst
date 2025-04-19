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


#ifndef RLST_RLST_PAR_GEOMETRY_HPP
#  define RLST_RLST_PAR_GEOMETRY_HPP

#include <cstdint>

/**
 * @file
 * This file contains the definitions of geometric structures and operations
 *
 * It includes the definition of a @ref Point structure and a @ref Segment
 * class, along with their respective constructors, assignment operators, and
 * utility functions.
 */

namespace rlst::par
{
  using literal_t = std::int64_t; ///< A literal

  /* Point */

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
   * Compare two @ref Point "points" for equality
   *
   * @param[in] __lhs The left hand side operand
   * @param[in] __rhs The right hand side operand
   *
   * @return True if the points are equal, false otherwise
   */

  constexpr bool operator==(const Point& __lhs, const Point& __rhs) noexcept
  {
    return
      (__lhs.x == __rhs.x) &&
      (__lhs.y == __rhs.y) &&
      (__lhs.z == __rhs.z);
  }

  /**
   * Compare two @ref Point "points" for inequality
   *
   * @param[in] __lhs The left hand side operand
   * @param[in] __rhs The right hand side operand
   *
   * @return True if the points are not equal, false otherwise
   */

  constexpr bool operator!=(const Point& __lhs, const Point& __rhs) noexcept
    { return !(__lhs == __rhs); }

  /**
   * Hash function for @ref Point
   *
   * @param[in] __input The @ref Point to hash
   * @return The hash value of the @ref Point
   */

  std::size_t hash_value(const Point& __input);

  /**
   * Swap two @ref Point "points"
   *
   * @param[in, out] __lhs The left hand side operand
   * @param[in, out] __rhs The right hand side operand
   */

  void swap(Point& __lhs, Point& __rhs) noexcept;

  /* Segment */

  /**
   * A segment
   */

  struct Segment
  {
    Point start; ///< The start point of the segment
    Point end; ///< The end point of the segment
  };

  /**
   * Compare two @ref Segment "segments" for equality
   *
   * @param[in] __lhs The left hand side operand
   * @param[in] __rhs The right hand side operand
   *
   * @return True if the segments are equal, false otherwise
   */

  constexpr bool operator==(const Segment& __lhs, const Segment& __rhs) noexcept
    { return (__lhs.start == __rhs.start) && (__lhs.end == __rhs.end); }

  /**
   * Compare two @ref Segment "segments" for inequality
   *
   * @param[in] __lhs The left hand side operand
   * @param[in] __rhs The right hand side operand
   *
   * @return True if the segments are not equal, false otherwise
   */

  constexpr bool operator!=(const Segment& __lhs, const Segment& __rhs) noexcept
    { return !(__lhs == __rhs); }

  /**
   * Hash function for @ref Segment
   *
   * @param[in] __input The @ref Segment to hash
   * @return The hash value of the @ref Segment
   */

  std::size_t hash_value(const Segment& __input);

  /**
   * Swap two @ref Segment "segments"
   *
   * @param[in, out] __lhs The left hand side operand
   * @param[in, out] __rhs The right hand side operand
   */

  void swap(Segment& __lhs, Segment& __rhs) noexcept;
}

#endif // RLST_RLST_PAR_GEOMETRY_HPP
