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


#ifndef RLST_CORE_RLST_PAR_GEOMETRY_HPP
#  define RLST_CORE_RLST_PAR_GEOMETRY_HPP

#include "core.hpp"

#include <cmath>
#include <cstdint>

/**
 * @file
 * This file contains the definitions of geometric structures and operations
 *
 * It includes the definition of a @ref rlst::par::Point structure and a
 * @ref rlst::par::Segment class, along with their respective constructors,
 * assignment operators, and utility functions.
 */

namespace rlst
{
  using literal_t = std::int64_t; ///< A literal
  using uliteral_t = std::uint64_t; ///< An unsigned literal

  /**
   * User-defined literal for @ref literal_t
   *
   * @param[in] __input The input value to be converted
   * @return The converted @ref literal_t value
   */

  constexpr literal_t operator ""_l(unsigned long long __input) noexcept
    { return static_cast<literal_t>(__input); }

  /**
   * User-defined literal for @ref uliteral_t
   *
   * @param[in] __input The input value to be converted
   * @return The converted @ref uliteral_t value
   */

  constexpr uliteral_t operator ""_ul(unsigned long long __input) noexcept
    { return static_cast<uliteral_t>(__input); }

  /* Size */

  /**
   * A size
   */

  struct Size
  {
    uliteral_t width; ///< The width
    uliteral_t height; ///< The height
  };

  /**
   * Multiply a @ref Size by a scalar in-place
   *
   * @param[in, out] __this The @ref Size to be scaled
   * @param[in] __a The scalar multiplier
   *
   * @return A reference to the scaled @ref Size
   */

  constexpr Size& operator*=(Size& __this, uliteral_t __a) noexcept;

  /**
   * Multiply a @ref Size by a scalar
   *
   * @param[in] __a The scalar multiplier
   * @param[in] __size The @ref Size to be multiplied
   *
   * @return A new @ref Size with dimensions scaled by the scalar
   */

  constexpr Size operator*(uliteral_t __a, Size __size) noexcept
    { return __size *= __a; }

  /**
   * Compare two @ref rlst::par::Size "sizes" for equality
   *
   * @param[in] __lhs The left hand side operand
   * @param[in] __rhs The right hand side operand
   *
   * @return True if the sizes are equal, false otherwise
   */

  constexpr bool operator==(const Size& __lhs, const Size& __rhs) noexcept
    { return (__lhs.width == __rhs.width) && (__lhs.height == __rhs.height); }

  /**
   * Compare two @ref rlst::par::Size "sizes" for inequality
   *
   * @param[in] __lhs The left hand side operand
   * @param[in] __rhs The right hand side operand
   *
   * @return True if the sizes are not equal, false otherwise
   */

  constexpr bool operator!=(const Size& __lhs, const Size& __rhs) noexcept
    { return !(__lhs == __rhs); }

  /**
   * Hash function for @ref rlst::par::Size
   *
   * @param[in] __input The @ref rlst::par::Size to hash
   * @return The hash value of the @ref rlst::par::Size
   */

  std::size_t hash_value(const Size& __input);

  /**
   * Swap two @ref rlst::par::Size "sizes"
   *
   * @param[in, out] __lhs The left hand side operand
   * @param[in, out] __rhs The right hand side operand
   */

  void swap(Size& __lhs, Size& __rhs) noexcept;

  /* Point */

  /**
   * A point
   */

  class Point
  {
    friend constexpr bool operator==(const Point&, const Point&) noexcept;
    friend std::size_t hash_value(const Point&);
    friend void swap(Point& __lhs, Point& __rhs) noexcept;

    RLST_ENFORCE_RULE_OF_FOUR(Point);
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
  public:
    /**
     * Get the x coordinate
     *
     * @return The x coordinate
     */

    constexpr literal_t x() const noexcept { return m_x; }

    /**
     * Get the x coordinate
     *
     * @return The x coordinate
     */

    constexpr literal_t& x() noexcept { return m_x; }

    /**
     * Get the y coordinate
     *
     * @return The y coordinate
     */

    constexpr literal_t y() const noexcept { return m_y; }

    /**
     * Get the y coordinate
     *
     * @return The y coordinate
     */

    constexpr literal_t& y() noexcept { return m_y; }

    /**
     * Get the z coordinate
     *
     * @return The z coordinate
     */

    constexpr literal_t z() const noexcept { return m_z; }

    /**
     * Get the z coordinate
     *
     * @return The z coordinate
     */

    constexpr literal_t& z() noexcept { return m_z; }
  public:
    /**
     * Check if a @ref Point is outside a given @ref Size "frame"
     *
     * @param[in] __frame The given @ref Size "frame"
     * @return if a @ref Point is outside a given @ref Size "frame"
     */

    constexpr bool is_outside(const Size& __frame) const noexcept
    {
      return
        (m_x < 0_l) ||
        (m_y < 0_l) ||
        (m_x >= static_cast<literal_t>(__frame.width)) ||
        (m_y >= static_cast<literal_t>(__frame.height));
    }

    /**
     * Get the Manhattan length of the segment between this point and another
     *
     * @return The Manhattan length of the segment between this point and
     * another
     */

    literal_t manhattan(const Point& __rhs) const noexcept
    {
      return
        std::abs(m_x - __rhs.m_x) +
        std::abs(m_y - __rhs.m_y) +
        std::abs(m_z - __rhs.m_z);
    }
  private:
    literal_t m_x;
    literal_t m_y;
    literal_t m_z;
  };

  /**
   * Compare two @ref rlst::par::Point "points" for equality
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
   * Compare two @ref rlst::par::Point "points" for inequality
   *
   * @param[in] __lhs The left hand side operand
   * @param[in] __rhs The right hand side operand
   *
   * @return True if the points are not equal, false otherwise
   */

  constexpr bool operator!=(const Point& __lhs, const Point& __rhs) noexcept
    { return !(__lhs == __rhs); }

  /**
   * Hash function for @ref rlst::par::Point
   *
   * @param[in] __input The @ref rlst::par::Point to hash
   * @return The hash value of the @ref rlst::par::Point
   */

  std::size_t hash_value(const Point& __input);

  /**
   * Swap two @ref rlst::par::Point "points"
   *
   * @param[in, out] __lhs The left hand side operand
   * @param[in, out] __rhs The right hand side operand
   */

  void swap(Point& __lhs, Point& __rhs) noexcept;
}

#include "geometry.ipp"

#endif // RLST_CORE_RLST_PAR_GEOMETRY_HPP
