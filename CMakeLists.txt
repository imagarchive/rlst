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
    rlst
    VERSION 0.9.0
    DESCRIPTION "A Redstone Logic Synthesis Tool because Redstone written in VHDL is cool "
    HOMEPAGE_URL "https://github.com/imagarchive/rlst/"
    LANGUAGES CXX
)

option(ENABLE_WERROR "Make all warnings into errors" ON)
option(WITH_DOC "Enable building of documentation" OFF)
option(WITH_TESTS "Enable building of the tests" OFF)

if (ENABLE_WERROR)
    list(APPEND RLST_CXX_GNU_FLAGS -Werror)
endif()

if (WITH_DOC)
    find_package(Doxygen REQUIRED)

    set(
        DOC_DIR
        "${CMAKE_BINARY_DIR}/doc/"
        CACHE PATH
        "The path where the documention is built"
    )

    set(DOXYGEN_AUTOLINK_SUPPORT NO)
    set(DOXYGEN_EXTRACT_STATIC YES)
    set(DOXYGEN_FILE_PATTERNS "*.hpp")
    set(DOXYGEN_INLINE_INHERITED_MEMB YES)
    set(DOXYGEN_OUTPUT_DIRECTORY "${DOC_DIR}")
    set(DOXYGEN_WARN_LOGFILE "${DOC_DIR}/log")
    set(DOXYGEN_WARN_NO_PARAMDOC YES)
endif()

macro (rlst_add_to_doc __headers)
    list(APPEND RLST_DOC_HPP "${__headers}")
endmacro()

if (WITH_TESTS)
    set(
        TEST_RESULT_DIR
        "${CMAKE_BINARY_DIR}/test-results/"
        CACHE PATH
        "The path where the JUnit4 XML report of the unit tests is written"
    )

    find_package(GTest REQUIRED)
    include(GoogleTest)

    enable_testing()

    function (rlst_add_integration_tests)
        set(options)
        set(one_value_args)
        set(multi_value_args TESTS)

        cmake_parse_arguments(
            RLST_ADD_INTEGRATION_TEST
            "${options}"
            "${one_value_args}"
            "${multi_value_args}"
            ${ARGN}
        )

        foreach (test ${RLST_ADD_INTEGRATION_TEST_TESTS})
            add_executable("${test}" "${CMAKE_SOURCE_DIR}/tests/${test}.cpp")

            target_compile_definitions("${test}" PRIVATE RLST_WITH_TESTS)

            target_link_libraries(
                "${test}"

                PRIVATE
                    GTest::gtest
                    GTest::gtest_main
            )

            set_target_properties(
                "${test}"

                PROPERTIES
                    RUNTIME_OUTPUT_DIRECTORY
                        "${CMAKE_BINARY_DIR}/integration-tests/"

                    RUNTIME_OUTPUT_NAME "${scope}"
            )

            gtest_discover_tests(
                "${test}"
                XML_OUTPUT_DIR "${TEST_RESULT_DIR}"
            )
        endforeach()
    endfunction()

    function (rlst_add_unit_tests)
        set(options)
        set(one_value_args)
        set(multi_value_args TESTS)

        cmake_parse_arguments(
            RLST_ADD_UNIT_TEST
            "${options}"
            "${one_value_args}"
            "${multi_value_args}"
            ${ARGN}
        )

        foreach (test ${RLST_ADD_UNIT_TEST_TESTS})
            string(REPLACE "/" "_" target "${test}")
            add_executable("${target}" "${test}_test.cpp")

            target_compile_definitions("${target}" PRIVATE RLST_WITH_TESTS)

            target_link_libraries(
                "${target}"

                PRIVATE
                    GTest::gtest
                    GTest::gtest_main
            )

            target_include_directories(
                "${target}"
                PRIVATE "${CMAKE_SOURCE_DIR}/src"
            )

            set_target_properties(
                "${target}"

                PROPERTIES
                    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/unit-tests/"
                    RUNTIME_OUTPUT_NAME "${scope}"
            )

            gtest_discover_tests(
                "${target}"
                XML_OUTPUT_DIR "${TEST_RESULT_DIR}"
            )
        endforeach()
    endfunction()
else()
    function (rlst_add_integration_tests)
    endfunction()

    function (rlst_add_unit_tests)
    endfunction()
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Ccache MODULE)
enable_ccache()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

list(
    APPEND RLST_CXX_GNU_FLAGS

        -fmessage-length=80
        -fno-operator-names
        -funsigned-char
        -Wall
        -Wconversion
        -Wdouble-promotion
        -Wduplicated-branches
        -Wduplicated-cond
        -Wignored-qualifiers
        -Wimplicit-fallthrough=5
        -Wmissing-field-initializers
        -Wmissing-include-dirs
        -Wno-div-by-zero
        -Wnoexcept
        -Wold-style-cast
        -Woverloaded-virtual
        -Wpedantic
        -Wsuggest-attribute=const
        -Wsuggest-attribute=noreturn
        -Wsuggest-attribute=pure
        -Wsuggest-final-methods
        -Wsuggest-final-types
        -Wsuggest-final-types
        -Wsuggest-override
        -Wsuggest-override
        -Wundef
        -Wuninitialized
)

foreach (flag IN LISTS RLST_CXX_GNU_FLAGS)
    add_compile_options(
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:GNU>>:${flag}>
    )
endforeach ()

add_subdirectory(src)

if (WITH_DOC)
    doxygen_add_docs(
        doc "${RLST_DOC_HPP}" COMMENT "Generating documentation..."
    )
endif()
