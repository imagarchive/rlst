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

  void swap(Point& __lhs, Point& __rhs) noexcept;

  /* SEGMENT */

  /**
   * A segment
   */

  class Segment
  {
    friend void swap(Segment& __lhs, Segment& __rhs) noexcept;
  public:
    /**
     * Constructs a @ref Segment with its parameters
     *
     * @param[in] __start The start @ref Point
     * @param[in] __end   The end @ref Point
     */

    constexpr Segment(Point __start, Point __end) noexcept
      : m_start(__start)
      , m_end(__end)
    {}
  public:
    /**
     * Copy constructor of @ref Segment
     *
     * @param[in] __other The @ref Segment to copy
     */

    constexpr Segment(const Segment& __other) = default;

    /**
     * Move constructor of @ref Segment
     *
     * @param[in, out] __other The @ref Segment to move
     */

    constexpr Segment(Segment&& __other) = default;

    ~Segment() = default; ///< Destructs a @ref Segment
  public:
    /**
     * Copy assignment operator for @ref Segment
     *
     * @param[in] __other The @ref Segment to copy
     * @return A reference to this @ref Segment
     */

    constexpr Segment& operator=(const Segment& __other) = default;

    /**
     * Move assignment operator for @ref Segment
     *
     * @param[in, out] __other The @ref Segment to move
     * @return A reference to this @ref Segment
     */

    constexpr Segment& operator=(Segment&& __other) = default;
  public:
    /**
     * Get the start @ref Point
     *
     * @return The start @ref Point
     */

    constexpr Point start() const noexcept { return m_start; }

    /**
     * Get the end @ref Point
     *
     * @return The end @ref Point
     */

    constexpr Point end() const noexcept { return m_end; }
  private:
    Point m_start; ///< The start point
    Point m_end;   ///< The end point
  };

  /**
   * Swap two @ref Segment "segments"
   *
   * @param[in, out] __lhs The left hand side operand
   * @param[in, out] __rhs The right hand side operand
   */

  void swap(Segment& __lhs, Segment& __rhs) noexcept;
}

#endif // RLST_RLST_PAR_GEOMETRY_HPP
