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

set(
    RLST_HPP
    mca_parser/Block.hpp
    mca_parser/Chunk.hpp
    mca_parser/MCA.hpp
    mca_parser/NBTChunkView.hpp
    par/cell/Cell.hpp
    par/cell/ports.hpp
    par/cell/types.hpp
    par/cell.hpp
    par/geometry.hpp
    par/placement.hpp
    par/routing.hpp
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
    mca_parser/Block.cpp
    mca_parser/Chunk.cpp
    mca_parser/MCA.cpp
    mca_parser/NBTChunkView.cpp
    par/cell/Cell.cpp
    par/cell/ports.cpp
    par/cell/types.cpp
    par/geometry.cpp
    par/placement.cpp
    par/routing.cpp
    svg/SVG.cpp
    main.cpp
)

include(FetchContent)

FetchContent_Declare(
    zlib_repo
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_BRANCH main
)

FetchContent_MakeAvailable(zlib_repo)

find_package(Boost REQUIRED)
add_executable(rlst "${RLST_CPP}")
target_link_libraries(rlst PRIVATE Boost::boost nbt zlib)

target_sources(
    rlst

    PUBLIC
        FILE_SET HEADERS
        TYPE HEADERS
        FILES "${RLST_HPP}" "${RLST_IPP}"
)

rlst_add_to_doc("${RLST_HPP}")

rlst_add_unit_tests(
    TESTS
        par/placement
)
