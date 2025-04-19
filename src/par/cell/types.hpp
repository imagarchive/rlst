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


#ifndef RLST_RLST_PAR_CELL_TYPES_HPP
#  define RLST_RLST_PAR_CELL_TYPES_HPP

#include <boost/functional/hash.hpp>
#include <list>

/**
 * @file
 *
 * Define the type of a cell, which consists of a logical gate or a group of
 * logical gates
 *
 * This file provides the definition of the CellType structure, which is used to
 * factorize common elements of all cells of the same type. For example, all AND
 * cells have 3 ports (2 inputs and 1 output).
 *
 * The file also includes utility functions and operators for comparing,
 * hashing, and swapping @ref rlst::par::cell::CellType objects.
 */

namespace rlst::par
{
  class Port;
}

namespace rlst::par::cell
{
  /**
   * A cell type
   */

  struct CellType
  {
    std::list<Port> ports; ///< The ports of the @ref CellType
  };

  /**
   * Equality operator for comparing two @ref CellType objects
   *
   * @param[in] __lhs The left-hand side @ref CellType object
   * @param[in] __rhs The right-hand side @ref CellType object
   *
   * @return True if both objects have the same memory address, false otherwise
   */

  constexpr bool operator==(
    const CellType& __lhs,
    const CellType& __rhs
  ) noexcept
    { return (&__lhs == &__rhs); }

  /**
   * Inequality operator for comparing two @ref CellType objects
   *
   * @param[in] __lhs The left-hand side @ref CellType object
   * @param[in] __rhs The right-hand side @ref CellType object
   *
   * @return True if the objects do not have the same memory address, false
   * otherwise
   */

  constexpr bool operator!=(
    const CellType& __lhs,
    const CellType& __rhs
  ) noexcept
    { return !(__lhs == __rhs); }

  /**
   * Hash function for @ref CellType objects
   *
   * This function computes a hash value for a given @ref CellType object.
   * The hash is based on the memory address of the object.
   *
   * @param[in] __input The @ref CellType object to hash
   * @return A hash value representing the @ref CellType object
   */

  inline std::size_t hash_value(const CellType& __input)
    { return boost::hash<const CellType *>()(&__input); }

  /**
   * Swap function for @ref CellType objects
   *
   * This function swaps the contents of two @ref CellType objects
   *
   * @param[in, out] __lhs The left-hand side @ref CellType object
   * @param[in, out] __rhs The right-hand side @ref CellType object
   */

  void swap(CellType& __lhs, CellType& __rhs) noexcept;
}

#endif // RLST_RLST_PAR_CELL_TYPES_HPP
