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


#ifndef RLST_RLST_PAR_CELL_CELL_HPP
#  define RLST_RLST_PAR_CELL_CELL_HPP

#include "par/cell/types.hpp"
#include "par/geometry.hpp"

#include <memory>

/**
 * @file
 *
 * Define the @ref rlst::par::cell::Cell structure and related operations for
 * representing cells in an electronic circuit
 *
 * A cell represents a fundamental building block in an electronic circuit, such
 * as a logic gate or a group of logic gates. This file provides the definition
 * of the @ref rlst::par::cell::Cell structure, along with utility functions for
 * comparing, hashing, and swapping @ref rlst::par::cell::Cell objects.
 *
 * The @ref rlst::par::cell::Cell structure includes
 * - the position of the cell in the circuit,
 * - the type of the cell, which describes its ports.
 */

namespace rlst::par::cell
{
  /**
   * A cell
   */

  struct Cell
  {
    Point position; ///< The position of the @ref Cell
    std::shared_ptr<const CellType> type; ///< The type of the @ref Cell
  };

  /**
   * Compare two @ref Cell objects for equality
   *
   * @param[in] __lhs The left-hand side @ref Cell object
   * @param[in] __rhs The right-hand side @ref Cell object
   *
   * @return True if both @ref Cell objects are the same, false otherwise
   */

  constexpr bool operator==(const Cell& __lhs, const Cell& __rhs) noexcept
    { return (__lhs.position == __rhs.position) && (__lhs.type == __rhs.type); }

  /**
   * Compare two @ref Cell objects for inequality
   *
   * @param[in] __lhs The left-hand side @ref Cell object
   * @param[in] __rhs The right-hand side @ref Cell object
   *
   * @return True if both @ref Cell objects are not the same, false otherwise
   */

  constexpr bool operator!=(const Cell& __lhs, const Cell& __rhs) noexcept
    { return !(__lhs == __rhs); }

  /**
   * Compute the hash value of a given @ref Cell object
   *
   * @param[in] __input The @ref Cell object to compute the hash value for
   * @return The computed hash value as a `std::size_t`
   */

  std::size_t hash_value(const Cell& __input);

  /**
   * Swap two @ref Cell objects
   *
   * @param[in, out] __lhs The first @ref Cell to swap
   * @param[in, out] __rhs The second @ref Cell to swap
   */

  void swap(Cell& __lhs, Cell& __rhs) noexcept;
}

#endif // RLST_RLST_PAR_CELL_CELL_HPP
