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

#include <functional>
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

/**
 * Contains core utilities and traits for the RLST library
 *
 * This namespace provides foundational components such as type traits and
 * utility functions that are essential for the library's functionality. It
 * includes tools for type checking, iterator categorization, and other core
 * operations.
 */

namespace rlst::core
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

  /**
   * A function to calculate the ceiling division of two numbers.
   *
   * This function takes two numbers, \p __a and \p __b, and calculates the
   * ceiling division of \p __a divided by \p __b.
   *
   * @tparam T The type of the dividend
   * @tparam U The type of the divisor
   * @tparam V The type of the result (by default, T and U's common type)
   *
   * @param[in] __a The dividend
   * @param[in] __b The divisor
   *
   * @return The result of the ceiling division
   */

  template <typename T, typename U, typename V = std::common_type_t<T, U>>
  constexpr V ceil_divide(T __a, U __b)
    { return static_cast<V>(__a == 0 ? 0 : (__a - 1) / __b + 1); }

  /**
   * A function to generate a random integer in a range
   *
   * @tparam T The type of the random integer
   *
   * @param[in] __first The first value of the range
   * @param[in] __last The last value of the range
   *
   * @return A random integer in the range [@p __first, @p __last)
   */

  template <typename T>
  T randint(T __first, T __last);

  /**
   * @brief Determine the function signature of a callable type
   *
   * @tparam F The callable type
   */

  template <class F>
  struct function_signature;

#ifndef RLST_DOXYGEN_SHOULD_SKIP_THIS
  template <class R, class T, bool nx, class... Args>
  struct function_signature<R (T::*) (Args...) noexcept(nx)>
      { using type = R(Args...); };

  template <class R, class T, bool nx, class... Args>
  struct function_signature<R (T::*) (Args...) & noexcept(nx)>
      { using type = R(Args...); };

  template <class R, class T, bool nx, class... Args>
  struct function_signature<R (T::*) (Args...) && noexcept(nx)>
      { using type = R(Args...); };

  template <class R, class T, bool nx, class... Args>
  struct function_signature<R (T::*) (Args...) const noexcept(nx)>
      { using type = R(Args...); };

  template <class R, class T, bool nx, class... Args>
  struct function_signature<R (T::*) (Args...) const & noexcept(nx)>
      { using type = R(Args...); };

  template <class R, class T, bool nx, class... Args>
  struct function_signature<R (T::*) (Args...) const && noexcept(nx)>
      { using type = R(Args...); };

  template <class R, class T, bool nx, class... Args>
  struct function_signature<R (T::*) (Args...) volatile noexcept(nx)>
      { using type = R(Args...); };

  template <class R, class T, bool nx, class... Args>
  struct function_signature<R (T::*) (Args...) volatile & noexcept(nx)>
      { using type = R(Args...); };

  template <class R, class T, bool nx, class... Args>
  struct function_signature<R (T::*) (Args...) volatile && noexcept(nx)>
      { using type = R(Args...); };

  template <class R, class T, bool nx, class... Args>
  struct function_signature<R (T::*) (Args...) const volatile noexcept(nx)>
      { using type = R(Args...); };

  template <class R, class T, bool nx, class... Args>
  struct function_signature<R (T::*) (Args...) const volatile & noexcept(nx)>
      { using type = R(Args...); };

  template <class R, class T, bool nx, class... Args>
  struct function_signature<R (T::*) (Args...) const volatile && noexcept(nx)>
      { using type = R(Args...); };
#endif // RLST_DOXYGEN_SHOULD_SKIP_THIS

  /**
   * The function signature of a callable type
   *
   * @tparam F The callable type
   */

  template <class F>
  using function_signature_t = typename function_signature<F>::type;

  /**
   * A helper allowing to transform arguments before applying a plus operation
   *
   * @tparam Transformer The type of the transformer
   */

  template <class Transformer>
  class transformed_binop;

  // TODO: add `noexcept`

  /**
   * A helper allowing to transform arguments before applying a plus operation
   *
   * @tparam R The return type of the transformer
   * @tparam T The type of the argument of the transformer
   */

  template <class R, class T>
  class transformed_binop<R(T)>
  {
    static_assert(std::is_arithmetic_v<R>);
  public:
    /// The type of the result of the transformation
    using result_type = R;

    /// The type of the argument of the transformation
    using argument_type = T;
  public:
    /// The default constructor
    transformed_binop() noexcept = default;

    /**
     * The copy constructor
     *
     * @param[in] __other The object to copy
     */

    transformed_binop(const transformed_binop& __other) = default;

    /**
     * The move constructor
     *
     * @param[in, out] __other The object to move
     */

    transformed_binop(transformed_binop&& __other) noexcept = default;

    /// The destructor
    ~transformed_binop() = default;
  public:
    /**
     * The copy assignment operator
     *
     * @param[in] __other The object to copy
     * @return A reference to this object
     */

    transformed_binop& operator=(const transformed_binop& __rhs) = default;

    /**
     * The move assignment operator
     *
     * @param[in, out] __other The object to move
     * @return A reference to this object
     */

    transformed_binop& operator=(transformed_binop&& __rhs) = default;
  public:
    /**
     * Construct a @ref transformed_binop with its parameters
     *
     * @param[in, out] __transformer The transformer to apply
     */

    template <class Tr>
    transformed_binop(Tr&& __transformer)
      : m_transformer(std::forward<Tr>(__transformer))
    {}
  public:
    /**
     * Apply the transformer to the arguments and return their sum
     *
     * @tparam U The type of the first argument (enable perfect forwarding)
     * @tparam V The type of the second argument (enable perfect forwarding)
     *
     * @param[in] __lhs The first argument
     * @param[in] __rhs The second argument
     *
     * @return The sum of the transformed arguments
     */

    template <class U, class V>
    std::enable_if_t<
      std::conjunction_v<
        std::is_same<std::decay_t<U>, std::decay_t<T>>,
        std::is_same<std::decay_t<V>, std::decay_t<T>>
      >,

      R
    >
    operator()(U&& __lhs, V&& __rhs) const
    {
      return
        m_transformer(std::forward<U>(__lhs)) +
        m_transformer(std::forward<V>(__rhs));
    }

    /**
     * Apply the transformer to the first argument and return its sum with the
     * second argument
     *
     * @tparam U The type of the first argument (enable perfect forwarding)
     *
     * @param[in, out] __lhs The first argument
     * @param[in] __rhs The second argument
     *
     * @return The sum of the transformed first argument and the second
     * argument
     */

    template <class U>
    std::enable_if_t<
      std::is_same_v<std::decay_t<U>, std::decay_t<T>>,
      R
    >
    operator()(U&& __lhs, R __rhs) const
      { return m_transformer(std::forward<U>(__lhs)) + __rhs; }

    /**
     * Apply the transformer to the second argument and return its sum with the
     * first argument
     *
     * @tparam U The type of the second argument (enable perfect forwarding)
     *
     * @param[in] __lhs The first argument
     * @param[in, out] __rhs The second argument
     *
     * @return The sum of the transformed second argument and the first
     * argument
     */

    template <class U>
    std::enable_if_t<
      std::is_same_v<std::decay_t<U>, std::decay_t<T>>,
      R
    >
    operator()(R __lhs, U&& __rhs) const
      { return operator()(std::forward<U>(__rhs), __lhs); }

    /**
     * Apply the transformer to the arguments and return their sum
     *
     * @param[in] __lhs The first argument
     * @param[in] __rhs The second argument
     *
     * @return The sum of the transformed arguments
     */

    R operator()(R __lhs, R __rhs)
      { return __lhs + __rhs; }
  private:
    std::function<R(T)> m_transformer;
  };

  template <class R, class T>
  transformed_binop(R (*)(T)) -> transformed_binop<R(T)>;

  template <class Tr>
  transformed_binop(Tr&&)
    -> transformed_binop<function_signature_t<decltype(&Tr::operator())>>;
}

#include "core.ipp"

#endif // RLST_RLST_CORE_HPP
