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

  class Point
  {
    friend constexpr bool operator==(const Point&, const Point&) noexcept;
    friend std::size_t hash_value(const Point&);
    friend void swap(Point& __lhs, Point& __rhs) noexcept;
  public:
    /**
     * Constructs a @ref Point with its parameters
     *
     * @param[in] __x The x coordinate
     * @param[in] __y The y coordinate
     * @param[in] __z The z coordinate
     */

    constexpr Point(literal_t __x, literal_t __y, literal_t __z) noexcept
      : m_x(__x)
      , m_y(__y)
      , m_z(__z)
    {}
  public:
    /**
     * Default constructor of @ref Point
     */

    constexpr Point() noexcept
      : m_x()
      , m_y()
      , m_z()
    {}

    ~Point() noexcept = default; ///< Destruct a @ref Point

    /**
     * Copy constructor of @ref Point
     *
     * @param[in] __other The @ref Point to copy
     */

    constexpr Point(const Point& __other) noexcept = default;

    /**
     * Move constructor of @ref Point
     *
     * @param[in, out] __other The @ref Point to move
     */

    constexpr Point(Point&& __other) noexcept = default;
  public:
    /**
     * Copy assignment operator for @ref Point
     *
     * @param[in] __other The @ref Point to copy
     * @return A reference to this @ref Point
     */

    constexpr Point& operator=(const Point& __other) noexcept = default;

    /**
     * Move assignment operator for @ref Point
     *
     * @param[in, out] __other The @ref Point to move
     * @return A reference to this @ref Point
     */

    constexpr Point& operator=(Point&& __other) noexcept = default;
  private:
    literal_t m_x;
    literal_t m_y;
    literal_t m_z;
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
      (__lhs.m_x == __rhs.m_x) &&
      (__lhs.m_y == __rhs.m_y) &&
      (__lhs.m_z == __rhs.m_z);
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

  class Segment
  {
    friend constexpr bool operator==(const Segment&, const Segment&) noexcept;
    friend std::size_t hash_value(const Segment&);
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
    Point m_start;
    Point m_end;
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
    { return (__lhs.m_start == __rhs.m_start) && (__lhs.m_end == __rhs.m_end); }

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
