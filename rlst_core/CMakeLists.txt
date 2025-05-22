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


cmake_minimum_required(VERSION 3.28)

project(
    rlst_core
    VERSION 0.9.0
    DESCRIPTION "A Redstone Logic Synthesis Tool because Redstone written in VHDL is cool (core API)"
    HOMEPAGE_URL "https://github.com/imagarchive/rlst/"
    LANGUAGES CXX
)

set(
    RLST_CORE_HPP
    cell_reader/cell_reading.hpp
    core/grid.hpp
    mca_parser/Block.hpp
    mca_parser/Chunk.hpp
    mca_parser/MCA.hpp
    mca_parser/NBTChunkView.hpp
    par/cell/Cell.hpp
    par/cell/ports.hpp
    par/cell/types.hpp
    par/cell.hpp
    par/placement.hpp
    par/routing.hpp
    parse_source/Parse.hpp
    parse_source/pTypes.hpp
    svg/SVG.hpp
    core.hpp
    geometry.hpp
    par.hpp
)

list(TRANSFORM RLST_CORE_HPP PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/include/")

set(
  RLST_CORE_IPP
  core/grid.ipp
  par/placement.ipp
  geometry.ipp
  core.ipp
)

list(TRANSFORM RLST_CORE_IPP PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/include/")

set(
    RLST_CORE_CPP
    cell_reader/cell_reading.cpp
    mca_parser/MCA.cpp
    mca_parser/NBTChunkView.cpp
    par/cell/Cell.cpp
    par/cell/ports.cpp
    par/cell/types.cpp
    par/placement.cpp
    par/routing.cpp
    parse_source/Parse.cpp
    parse_source/pCell.cpp
    parse_source/pModule.cpp
    parse_source/pPort.cpp
    svg/SVG.cpp
    geometry.cpp
)

list(TRANSFORM RLST_CORE_CPP PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/src/")

cmake_policy(SET CMP0167 NEW)
find_package(Boost REQUIRED COMPONENTS headers log)

find_package(ZLIB REQUIRED)

add_library(rlst_core STATIC "${RLST_CORE_CPP}")
target_link_libraries(rlst_core PUBLIC Boost::headers Boost::log nbt ZLIB::ZLIB)

target_include_directories(
    rlst_core
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/"
)

target_sources(
    rlst_core

    PUBLIC
        FILE_SET HEADERS
        TYPE HEADERS
        FILES "${RLST_CORE_HPP}" "${RLST_CORE_IPP}"
)

rlst_add_to_doc("${RLST_CORE_HPP}")

rlst_add_integration_tests(EXTRA_LIBS rlst_core TESTS place)

rlst_add_unit_tests(
    EXTRA_LIBS rlst_core

    TESTS
        core/grid
        par/placement
)
