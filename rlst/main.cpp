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

int main(int __argc, char *__argv[])
{
  using namespace rlst;

  auto types = cell_reader::generate_gates(RLST_INSTALL_GATESDIR);
  initCellTypes(std::move(types));

  auto [net, cell_list] = parse_v(__argc, __argv);

  std::vector<par::net_t> net_vec;
  net_vec.reserve(net.size());

  for (par::net_t n : net) {
    net_vec.push_back(std::move(n));
  }

  par::place(
    cell_list.begin(),
    cell_list.end(),
    net.cbegin(),
    net.cend(),
    12_ul
  );

  auto route =
    par::route(
      net_vec,
      Point(),
      par::frame_of(cell_list.cbegin(), cell_list.cend())
    );

  svg::SVG s("output.svg");

  for (const par::cell::Cell& c : cell_list) {
    s.draw_cell(c);
  }

  for (const auto& a : route) {
    for (const auto& b : a) {
      s.draw_point(b.first, "green");
    }
  }
}
