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


#include "cell_reader/cell_reading.hpp"

#include "par/placement.hpp"
#include "par/routing.hpp"
#include "parse_source/Parse.hpp"
#include "parse_source/pTypes.hpp"
#include "svg/SVG.hpp"
#include "config.hpp"
#include "par.hpp"
#include "geometry.hpp"

#include "mca_parser/MCA.hpp"

int main(int __argc, char *__argv[])
{
  using namespace rlst;
  // using namespace rlst::mca_parser;

  auto types = cell_reader::generate_gates(RLST_INSTALL_GATESDIR);
  // initCellTypes(std::move(types));

  // auto [net, cell_list] = parse_v(__argc, __argv);

  // std::vector<par::net_t> net_vec;
  // net_vec.reserve(net.size());

  // for (par::net_t n : net) {
  //   net_vec.push_back(std::move(n));
  // }

  // par::place(
  //   cell_list.begin(),
  //   cell_list.end(),
  //   net.cbegin(),
  //   net.cend(),
  //   12_ul
  // );

  // auto route =
  //   par::route(
  //     net_vec,
  //     Point(),
  //     par::frame_of(cell_list.cbegin(), cell_list.cend())
  //   );

  // DEBUG
  std::shared_ptr<par::cell::CellType> and_type;
  std::shared_ptr<par::cell::CellType> or_type;
  std::shared_ptr<par::cell::CellType> not_type;
  for (auto type : types) {
    if (type.name == "AND") {
      and_type = std::make_shared<par::cell::CellType>(type);
    } else if (type.name == "OR") {
      or_type = std::make_shared<par::cell::CellType>(type);
    } else if (type.name == "NOT") {
      not_type = std::make_shared<par::cell::CellType>(type);
    }
  }
  auto node_type =
    std::make_shared<par::cell::CellType>(
      par::cell::CellType {
        std::vector {
          par::cell::Port { Point(0, 0, 0), par::cell::PortType::input },
          par::cell::Port { Point(2, 0, 0), par::cell::PortType::output }
        },

        "NODE",
        Size { 6_l, 4_l },
        {}
      }
    );

  std::vector cells {
    par::cell::Cell(Point(), node_type),
    par::cell::Cell(Point(), node_type),

    par::cell::Cell(Point(), not_type),
    par::cell::Cell(Point(), not_type),

    par::cell::Cell(Point(), and_type),
    par::cell::Cell(Point(), and_type),

    par::cell::Cell(Point(), or_type),
    par::cell::Cell(Point(), node_type)
  };

  std::vector<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>> nets {
    std::make_pair(
      par::cell::PlacedPort { cells[0], cells[0].type()->ports[1] },
      par::cell::PlacedPort { cells[2], cells[2].type()->ports[0] }
    ),

    std::make_pair(
      par::cell::PlacedPort { cells[0], cells[0].type()->ports[1] },
      par::cell::PlacedPort { cells[4], cells[4].type()->ports[0] }
    ),

    std::make_pair(
      par::cell::PlacedPort { cells[1], cells[1].type()->ports[1] },
      par::cell::PlacedPort { cells[3], cells[3].type()->ports[0] }
    ),

    std::make_pair(
      par::cell::PlacedPort { cells[1], cells[1].type()->ports[1] },
      par::cell::PlacedPort { cells[5], cells[5].type()->ports[1] }
    ),

    std::make_pair(
      par::cell::PlacedPort { cells[2], cells[2].type()->ports[1] },
      par::cell::PlacedPort { cells[5], cells[5].type()->ports[0] }
    ),

    std::make_pair(
      par::cell::PlacedPort { cells[3], cells[3].type()->ports[1] },
      par::cell::PlacedPort { cells[4], cells[4].type()->ports[1] }
    ),

    std::make_pair(
      par::cell::PlacedPort { cells[4], cells[4].type()->ports[2] },
      par::cell::PlacedPort { cells[6], cells[6].type()->ports[0] }
    ),

    std::make_pair(
      par::cell::PlacedPort { cells[5], cells[5].type()->ports[2] },
      par::cell::PlacedPort { cells[6], cells[6].type()->ports[1] }
    ),

    std::make_pair(
      par::cell::PlacedPort { cells[6], cells[6].type()->ports[2] },
      par::cell::PlacedPort { cells[7], cells[7].type()->ports[0] }
    )
  };

  std::cout << "ROUTING" << std::endl;
  par::place(cells.begin(), cells.end(), nets.begin(), nets.end(), 16_ul);
  std::cout << "PLACEMENT" << std::endl;
  auto route =
    par::route(
      nets,
      Point(),
      par::frame_of(cells.cbegin(), cells.cend())
    );

  std::cout << "SVG" << std::endl;
  svg::SVG s("output.svg");

  for (const par::cell::Cell& c : cells) {
    s.draw_cell(c);
  }

  for (const auto& a : route) {
    for (const auto& b : a) {
      if (b.first.z() == 3) {
        s.draw_point(b.first, "green");
      } else if (b.first.z() == 7) {
        s.draw_point(b.first, "orange");
      } else {
        s.draw_point(b.first, "brown");
      }
    }
  }
  // BLOCK PLACEMENT
  std::cout << "MINECRAFT" << std::endl;

  // TODO don't hardcode
  std::string file_name = "r.0.0.mca";

  mca_parser::MCA mca_file = mca_parser::readMcaFile(file_name);

  // routing blocks
  std::vector<mca_parser::Block> route_blocks =
    par::generate_routing_blocks(route, nets);
  for (mca_parser::Block block : route_blocks) {
    // TODO add 5 to z if building above the lowest section isn't implemented
    block.position().z() += 5;
    mca_parser::placeBlock(mca_file, block);
  }

  // gate blocks
  for (par::cell::Cell cell : cells) {
    std::vector<mca_parser::Block> gate_blocks = cell.generate_blocks();
    for (mca_parser::Block block : gate_blocks) {
      // TODO add 5 to z if building above the lowest section isn't implemented
    block.position().z() += 5;
      mca_parser::placeBlock(mca_file, block);
    }
  }

  writeData(mca_file);
}
