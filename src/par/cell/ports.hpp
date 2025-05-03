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


#ifndef RLST_RLST_PAR_CELL_PORTS_HPP
#  define RLST_RLST_PAR_CELL_PORTS_HPP

#include "par/geometry.hpp"
#include <memory>

/**
 * @file
 *
 * Define the structures and functions related to ports in a cell
 *
 * This file contains the definitions for @ref rlst::par::cell::Port,
 * @ref rlst::par::cell::PlacedPort, and their associated operations, such as
 * comparison, hashing, and swapping. It also defines the
 * @ref rlst::par::cell::PortType enumeration to specify the type of a port.
 *
 * The structures and functions in this file are part of the
 * @ref rlst::par::cell namespace.
 */

namespace rlst::par::cell
{
  struct Cell;
}

namespace rlst::par::cell
{
  /* PortType */

  /**
   * A @ref Port type
   */

  enum class PortType
  {
    input, ///< The port is an input
    output, ///< The port is an output
    inout ///< The port is an input and output
  };

  /* Port */

  /**
   * A port of a @ref Cell
   */

  struct Port
  {
    Point position; ///< The position of the port within the @ref Cell
    PortType type; ///< The type of the port
  };

  /**
   * Compare two @ref Port objects for equality
   *
   * @param[in] __lhs The left-hand side @ref Port object
   * @param[in] __rhs The right-hand side @ref Port object
   *
   * @return True if both @ref Port objects are the same, false otherwise
   */

  constexpr bool operator==(const Port& __lhs, const Port& __rhs) noexcept
    { return (__lhs.position == __rhs.position) && (__lhs.type == __rhs.type); }

    /**
     * Compare two @ref Port objects for inequality
     *
     * @param[in] __lhs The left-hand side @ref Port object
     * @param[in] __rhs The right-hand side @ref Port object
     *
     * @return True if both @ref Port objects are not the same, false otherwise
     */

  constexpr bool operator!=(const Port& __lhs, const Port& __rhs) noexcept
    { return !(__lhs == __rhs); }

  /**
   * Generate a hash value for a @ref Port object
   *
   * @param[in] __input The @ref Port object to hash
   * @return A hash value representing the @ref Port object
   */

  std::size_t hash_value(const Port& __input);

  /**
   * Swap the contents of two @ref Port objects
   *
   * @param[in, out] __lhs The left-hand side @ref Port object
   * @param[in, out] __rhs The right-hand side @ref Port object
   */

  void swap(Port& __lhs, Port& __rhs) noexcept;

  /* PlacedPort */

  /**
   * A placed port
   *
   * This is a port that has been placed in a @ref Cell
   */

  struct PlacedPort
  {
    std::shared_ptr<const Cell> parent; ///< The parent of the @ref PlacedPort
    Port port; ///< The corresponding @ref Port
  };

  /**
   * Compare two @ref PlacedPort objects for equality
   *
   * @param[in] __lhs The left-hand side @ref PlacedPort object
   * @param[in] __rhs The right-hand side @ref PlacedPort object
   *
   * @return True if both @ref PlacedPort objects are the same, false otherwise
   */

  inline bool operator==(
    const PlacedPort& __lhs,
    const PlacedPort& __rhs
  ) noexcept
    { return (__lhs.parent == __rhs.parent) && (__lhs.port == __rhs.port); }

  /**
   * Compare two @ref PlacedPort objects for inequality
   *
   * @param[in] __lhs The left-hand side @ref PlacedPort object
   * @param[in] __rhs The right-hand side @ref PlacedPort object
   *
   * @return True if both @ref PlacedPort objects are not the same, false otherwise
   */

  inline bool operator!=(
    const PlacedPort& __lhs,
    const PlacedPort& __rhs
  ) noexcept
    { return !(__lhs == __rhs); }

  /**
   * Generate a hash value for a @ref PlacedPort object
   *
   * @param[in] __input The @ref PlacedPort object to hash
   * @return A hash value representing the @ref PlacedPort object
   */

  std::size_t hash_value(const PlacedPort& __input);

  /**
   * Swap the contents of two @ref PlacedPort objects
   *
   * @param[in, out] __lhs The left-hand side @ref PlacedPort object
   * @param[in, out] __rhs The right-hand side @ref PlacedPort object
   */

  void swap(PlacedPort& __lhs, PlacedPort& __rhs) noexcept;

  /**
   * Get the absolute x of a @ref PlacedPort object
   */

  literal_t absolute_x(const PlacedPort& __placed_port);

  /**
   * Get the absolute y of a @ref PlacedPort object
   */

  literal_t absolute_y(const PlacedPort& __placed_port);
}

#endif // RLST_RLST_PAR_CELL_PORTS_HPP
