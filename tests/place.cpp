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


#include "par/placement.hpp"
#include <gtest/gtest.h>

using namespace rlst::par;

TEST(place__test, basic)
{
  auto and_type =
    std::make_shared<cell::CellType>(
      cell::CellType {
        std::list {
          cell::Port { Point(0, 1, 0), cell::PortType::output },
          cell::Port { Point(3, 0, 0), cell::PortType::input },
          cell::Port { Point(3, 2, 0), cell::PortType::input }
        },

        "AND",
        Size { 4_l, 4_l }
      }
    );

  auto or_type =
    std::make_shared<cell::CellType>(
      cell::CellType {
        std::list {
          cell::Port { Point(0, 0, 0), cell::PortType::input },
          cell::Port { Point(4, 0, 0), cell::PortType::input },
          cell::Port { Point(6, 2, 0), cell::PortType::output }
        },

        "OR",
        Size { 8_l, 4_l }
      }
    );

  std::vector cells {
    cell::Cell(Point(), and_type),
    cell::Cell(Point(), or_type),
    cell::Cell(Point(), or_type),
    cell::Cell(Point(), or_type),
    cell::Cell(Point(), and_type),
    cell::Cell(Point(), or_type),
    cell::Cell(Point(), or_type),
    cell::Cell(Point(), or_type)
  };

  std::vector<std::pair<cell::PlacedPort, cell::PlacedPort>> nets {
    std::make_pair(
      cell::PlacedPort {
        cells[0],
        cell::Port { Point(0, 1, 0), cell::PortType::output }
      },

      cell::PlacedPort {
        cells[1],
        cell::Port { Point(0, 0, 0), cell::PortType::input }
      }
    ),

    std::make_pair(
      cell::PlacedPort {
        cells[1],
        cell::Port { Point(6, 2, 0), cell::PortType::output }
      },

      cell::PlacedPort {
        cells[0],
        cell::Port { Point(3, 2, 0), cell::PortType::input }
      }
    )
  };

  place(cells.begin(), cells.end(), nets.begin(), nets.end(), 12_ul);
}
