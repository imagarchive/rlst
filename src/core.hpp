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

#include <random>

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

namespace rlst
{
  /**
   * Contains core utilities and traits for the RLST library
   *
   * This namespace provides foundational components such as type traits and
   * utility functions that are essential for the library's functionality. It
   * includes tools for type checking, iterator categorization, and other core
   * operations.
   */

  inline namespace core
  {
    /**
     * A trait allowing to check if a type is a random access iterator
     *
     * @tparam T The type to test
     */

    template <class T, typename = void>
    struct is_random_access_iterator : std::false_type {};

    template <class T>
    struct is_random_access_iterator<T, void>
      : std::is_base_of<
        std::random_access_iterator_tag,
        typename std::iterator_traits<T>::iterator_category
      >
    {};

    /**
     * Check if a type is a random access iterator
     *
     * @tparam T The type to test
     */

    template <class T>
    constexpr bool is_random_access_iterator_v =
      is_random_access_iterator<T>::value;

    /**
     * Choose a random element from a range
     *
     * @param[in] __begin The beginning of the range
     * @param[in] __end The end of the range
     *
     * @return A random element from the range
     */

    template <class Iterator>
    Iterator choice(Iterator __begin, Iterator __end);
  }
}

#include "core.ipp"

#endif // RLST_RLST_CORE_HPP
