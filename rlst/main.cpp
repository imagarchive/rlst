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
#include "parse_source/Parse.hpp"
#include "config.hpp"
#include "parse_source/pTypes.hpp"

#include <iostream>

int main(int __argc, char* __argv[])
{
  auto types = rlst::cell_reader::generate_gates(RLST_INSTALL_GATESDIR);
  initCellTypes(std::move(types));
  parse_v(__argc, __argv);
  std::cout << "Hello world!" << std::endl;
}
