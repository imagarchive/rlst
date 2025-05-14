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


#ifndef RLST_RLST_TEST_HPP
#  define RLST_RLST_TEST_HPP

#include "par/cell/Cell.hpp"

/**
 * @file
 * Header file for testing utilities in the RLST library.
 *
 * This file provides a collection of utility functions and classes
 * designed to facilitate testing and validation of the RLST library.
 * It includes helper functions for creating and manipulating objects
 * used in the library, ensuring consistent behavior across different
 * components during testing.
 */

/**
 * Contains utility functions and classes for testing purposes.
 *
 * This namespace provides a collection of helper functions and utilities
 * designed to facilitate testing and validation of the RLST library. These
 * utilities are intended to simplify the creation of test cases and ensure
 * consistent behavior across different components of the library.
 */

namespace rlst::test
{
  /**
   * Creates a @ref par::cell::Cell object from the given geometry parameters
   *
   * This function constructs a @ref par::cell::Cell object using the specified
   * position and size parameters. The position is defined by the x and y
   * coordinates, while the size is determined by the width and height.
   *
   * @param[in] __x The x-coordinate of the cell's position
   * @param[in] __y The y-coordinate of the cell's position
   * @param[in] __width The width of the cell
   * @param[in] __height The height of the cell
   *
   * @return A @ref par::cell::Cell object initialized with the given parameters
   */

  par::cell::Cell cell_from_geometry_params(
    par::literal_t __x,
    par::literal_t __y,
    par::uliteral_t __width,
    par::uliteral_t __height
  )
  {
    return
      par::cell::Cell {
        par::Point(__x, __y, 0),

        *std::make_shared<par::cell::CellType>(
          par::cell::CellType {
            std::list<par::cell::Port>(),
            std::string(),
            par::Size {__width, __height}
          }
        )
      };
  }
}

#endif // RLST_RLST_TEST_HPP
