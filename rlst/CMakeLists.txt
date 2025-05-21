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

set(LINUX UNIX AND NOT APPLE)
set(RLST_INSTALL_GATESDIR "${CMAKE_SOURCE_DIR}/share/gates/")

if (LINUX)
  include(GNUInstallDirs)

  if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(RLST_INSTALL_GATESDIR "${CMAKE_INSTALL_FULL_DATADIR}/gates/")
  endif()
endif()

configure_file(config.hpp.in config.hpp ESCAPE_QUOTES @ONLY)

add_executable(rlst main.cpp)
target_link_libraries(rlst PRIVATE rlst_core)
target_include_directories(rlst PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_sources(
    rlst

    PRIVATE
        FILE_SET HEADERS
        TYPE HEADERS
        BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}"
        FILES "${CMAKE_CURRENT_BINARY_DIR}/config.hpp"
)
