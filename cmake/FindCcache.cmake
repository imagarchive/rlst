# Copyright (C) 2023-2024 Mattéo Rossillol‑‑Laruelle <beatussum@protonmail.com>
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program. If not, see <https://www.gnu.org/licenses/>.


#[========================================================================[.md:

# Ccache

Finds the Ccache executable and defines convenient functions.

## Result variables

This will define the following variables:

- `Ccache_FOUND`: True if the system has the Ccache executable.

If `Ccache_FOUND` is true, the following are added:

- `Ccache_VERSION`: Version of the Ccache executable.
- `Ccache_VERSION_MAJOR`: Major version of the Ccache executable.
- `Ccache_VERSION_MINOT`: Minor version of the Ccache executable.
- `Ccache_VERSION_PATCH`: Patch version of the Ccache executable.

## Cache variables

The following cache variables are defined:

- `CCACHE_EXECUTABLE`: The Ccache executable.

#]========================================================================]

cmake_minimum_required(VERSION 3.25)

find_program(CCACHE_EXECUTABLE ccache DOC "Fast compiler cache")

if (CCACHE_EXECUTABLE)
    execute_process(
        COMMAND ${CCACHE_EXECUTABLE} -V
        OUTPUT_VARIABLE Ccache_VERSION
    )

    string(
        REGEX MATCH "version ([0-9]+).([0-9]+).([0-9]+)"
        _
        "${Ccache_VERSION}"
    )

    set(Ccache_VERSION_MAJOR ${CMAKE_MATCH_1})
    set(Ccache_VERSION_MINOR ${CMAKE_MATCH_2})
    set(Ccache_VERSION_PATCH ${CMAKE_MATCH_3})

    string(
        JOIN "."
        Ccache_VERSION
        ${Ccache_VERSION_MAJOR}
        ${Ccache_VERSION_MINOR}
        ${Ccache_VERSION_PATCH}
    )
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    Ccache
    REQUIRED_VARS CCACHE_EXECUTABLE
    VERSION_VAR Ccache_VERSION
)

include(FeatureSummary)

set_package_properties(
    Ccache

    PROPERTIES
        URL "https://ccache.dev/"
        DESCRIPTION "Fast compiler cache"
        PURPOSE "Enable support for ccache during build"
)

function (enable_ccache)
    set(options REQUIRED)
    set(one_value_args)
    set(multi_value_args)

    cmake_parse_arguments(
        ENABLE_CCACHE
        "${options}"
        "${one_value_args}"
        "${multi_value_args}"
        ${ARGN}
    )

    if (Ccache_FOUND)
        set_property(
            GLOBAL
            PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_EXECUTABLE}"
        )
    elseif (ENABLE_CCACHE_REQUIRED)
        message(FATAL_ERROR "Ccache cannot be found but is required.")
    else()
        message(WARNING "Ccache cannot be enabled for build (ignoring).")
    endif()
endfunction()
