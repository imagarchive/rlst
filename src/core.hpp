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


#ifndef RLST_RLST_CORE_HPP
#  define RLST_RLST_CORE_HPP

/**
 * @file
 *
 * Core utilities and declarations for the RLST library
 *
 * This header defines core functionality and utility macros used throughout the
 * RLST library, including macros to enforce C++ best practices.
 */

/**
 * A macro to enforce the "rule of 4"
 *
 * This macro declares the following class member functions:
 * - copy constructor,
 * - move constructor,
 * - copy assignment operator,
 * - move assignment operator,
 * - destructor.
 *
 * @param[in] T The name of the class.
 *
 * @note All declarations are defaulted, public, constexpr and noexcept.
 */

#define RLST_ENFORCE_RULE_OF_FOUR(T)                     \
  public:                                                \
    constexpr T(const T&) noexcept = default;            \
    constexpr T(T&&) noexcept = default;                 \
    constexpr T& operator=(const T&) noexcept = default; \
    constexpr T& operator=(T&&) noexcept = default;      \
    ~T() noexcept = default

/**
 * A macro to enforce the "rule of 5"
 *
 * This macro declares the following class member functions:
 * - default constructor,
 * - copy constructor,
 * - move constructor,
 * - copy assignment operator,
 * - move assignment operator,
 * - destructor.
 *
 * @param[in] T The name of the class.
 *
 * @note All declarations are defaulted, public, constexpr and noexcept.
 */

#define RLST_ENFORCE_RULE_OF_FIVE(T)                     \
  public:                                                \
    constexpr T() noexcept = default;                    \
    constexpr T(const T&) noexcept = default;            \
    constexpr T(T&&) noexcept = default;                 \
    constexpr T& operator=(const T&) noexcept = default; \
    constexpr T& operator=(T&&) noexcept = default;      \
    ~T() noexcept = default

#endif // RLST_RLST_CORE_HPP
