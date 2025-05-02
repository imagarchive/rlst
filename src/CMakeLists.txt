# Copyright (C) 2025 Mattéo Rossillol‑‑Laruelle <beatussum@protonmail.com>
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# this program. If not, see <https://www.gnu.org/licenses/>.


cmake_minimum_required(VERSION 3.30)

set(
    RLST_HPP
    par/cell/Cell.hpp
    par/cell/ports.hpp
    par/cell/types.hpp
    par/cell.hpp
    par/geometry.hpp
    par/placement.hpp
    svg/SVG.hpp
    core.hpp
    par.hpp
)

set(
  RLST_IPP
  par/placement.ipp
)

set(
    RLST_CPP
    par/cell/Cell.cpp
    par/cell/ports.cpp
    par/cell/types.cpp
    par/geometry.cpp
    par/placement.cpp
    svg/SVG.cpp
    main.cpp
)

find_package(Boost REQUIRED)
add_executable(rlst "${RLST_CPP}")

target_link_libraries(rlst PRIVATE Boost::boost)

target_sources(
    rlst

    PUBLIC
        FILE_SET HEADERS
        TYPE HEADERS
        FILES "${RLST_HPP}" "${RLST_IPP}
)

rlst_add_to_doc("${RLST_HPP}")
