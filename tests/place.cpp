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

using namespace rlst;
using namespace rlst::par;

TEST(place__test, basic)
{
  auto and_type =
    std::make_shared<cell::CellType>(
      cell::CellType {
        std::vector {
          std::make_shared<cell::Port>(
            cell::Port { Point(0, 0, 0), cell::PortType::input }),
          std::make_shared<cell::Port>(
            cell::Port { Point(0, 4, 0), cell::PortType::input }),
          std::make_shared<cell::Port>(
            cell::Port { Point(1, 2, 0), cell::PortType::output })
        },

        "AND",
        Size { 2_l, 5_l },
        {}
      }
    );

  auto or_type =
    std::make_shared<cell::CellType>(
      cell::CellType {
        std::vector {
          std::make_shared<cell::Port>(
            cell::Port { Point(0, 0, 0), cell::PortType::input }),
          std::make_shared<cell::Port>(
            cell::Port { Point(8, 0, 0), cell::PortType::input }),
          std::make_shared<cell::Port>(
            cell::Port { Point(4, 1, 0), cell::PortType::output })
        },

        "OR",
        Size { 9_l, 2_l },
        {}
      }
    );

  auto not_type =
    std::make_shared<cell::CellType>(
      cell::CellType {
        std::vector {
          std::make_shared<cell::Port>(
            cell::Port { Point(0, 0, 0), cell::PortType::input }),
          std::make_shared<cell::Port>(
            cell::Port { Point(2, 0, 0), cell::PortType::output })
        },

        "NOT",
        Size { 3_l, 1_l },
        {}
      }
    );

  auto node_type =
    std::make_shared<cell::CellType>(
      cell::CellType {
        std::vector {
          std::make_shared<cell::Port>(
            cell::Port { Point(0, 0, 0), cell::PortType::input }),
          std::make_shared<cell::Port>(
            cell::Port { Point(2, 0, 0), cell::PortType::output })
        },

        "NODE",
        Size { 3_l, 1_l },
        {}
      }
    );

  std::vector cells {
    cell::Cell(Point(), node_type),
    cell::Cell(Point(), node_type),

    cell::Cell(Point(), not_type),
    cell::Cell(Point(), not_type),

    cell::Cell(Point(), and_type),
    cell::Cell(Point(), and_type),

    cell::Cell(Point(), or_type),
    cell::Cell(Point(), node_type)
  };

  std::vector<std::pair<cell::PlacedPort, cell::PlacedPort>> nets {
    std::make_pair(
      cell::PlacedPort { std::make_shared<const cell::Cell>(
        cells[0]), cells[0].type()->ports[1] },
      cell::PlacedPort { std::make_shared<const cell::Cell>(
        cells[2]), cells[2].type()->ports[0] }
    ),

    std::make_pair(
      cell::PlacedPort { std::make_shared<const cell::Cell>(
        cells[0]), cells[0].type()->ports[1] },
      cell::PlacedPort { std::make_shared<const cell::Cell>(
        cells[4]), cells[4].type()->ports[0] }
    ),

    std::make_pair(
      cell::PlacedPort { std::make_shared<const cell::Cell>(
        cells[1]), cells[1].type()->ports[1] },
      cell::PlacedPort { std::make_shared<const cell::Cell>(
        cells[3]), cells[3].type()->ports[0] }
    ),

    std::make_pair(
      cell::PlacedPort { std::make_shared<const cell::Cell>(
        cells[1]), cells[1].type()->ports[1] },
      cell::PlacedPort { std::make_shared<const cell::Cell>(
        cells[5]), cells[5].type()->ports[1] }
    ),

    std::make_pair(
      cell::PlacedPort { std::make_shared<const cell::Cell>(
        cells[2]), cells[2].type()->ports[1] },
      cell::PlacedPort { std::make_shared<const cell::Cell>(
        cells[5]), cells[5].type()->ports[0] }
    ),

    std::make_pair(
      cell::PlacedPort { std::make_shared<const cell::Cell>(
        cells[3]), cells[3].type()->ports[1] },
      cell::PlacedPort { std::make_shared<const cell::Cell>(
        cells[4]), cells[4].type()->ports[1] }
    ),

    std::make_pair(
      cell::PlacedPort { std::make_shared<const cell::Cell>(
        cells[4]), cells[4].type()->ports[2] },
      cell::PlacedPort { std::make_shared<const cell::Cell>(
        cells[6]), cells[6].type()->ports[0] }
    ),

    std::make_pair(
      cell::PlacedPort { std::make_shared<const cell::Cell>(
        cells[5]), cells[5].type()->ports[2] },
      cell::PlacedPort { std::make_shared<const cell::Cell>(
        cells[6]), cells[6].type()->ports[1] }
    ),

    std::make_pair(
      cell::PlacedPort { std::make_shared<const cell::Cell>(
        cells[6]), cells[6].type()->ports[2] },
      cell::PlacedPort { std::make_shared<const cell::Cell>(
        cells[7]), cells[7].type()->ports[0] }
    )
  };

  place(cells.begin(), cells.end(), nets.begin(), nets.end(), 16_ul);
}
