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


#ifndef RLST_RLST_PAR_PLACEMENT_HPP
#  define RLST_RLST_PAR_PLACEMENT_HPP

#include "par/cell/ports.hpp"
#include "core.hpp"

#include <boost/contract.hpp>
#include <numeric>

/**
 * This namespace describes all utilities related to Place And Route
 */

namespace rlst::par
{
  namespace details {}

  /* iteration_t */

  using iteration_t = std::uint8_t; ///< The type of the number of iterations

  /**
   * User-defined literal for @ref rlst::par::iteration_t
   *
   * @param[in] __input The input value
   * @return The input value as a @ref rlst::par::iteration_t
   */

  constexpr iteration_t operator ""_it(unsigned long long __input) noexcept
    { return static_cast<iteration_t>(__input); }

  /* real_t */

  using real_t = long double; ///< A real number

  /**
   * User-defined literal for @ref rlst::par::real_t
   *
   * @param[in] __input The input value
   * @return The input value as a @ref rlst::par::real_t
   */

  constexpr real_t operator ""_r(long double __input) noexcept
    { return static_cast<real_t>(__input); }

  /**
   * The probabilistic acceptance function
   *
   * It is called when the cost is increased by a perturbation. It determines
   * whether to accept a move or not, depending on the cost increase and the
   * temperature.
   *
   * Usually it is the exponential function, but it can be any other function.
   *
   * @param[in] __delta_c The cost increase
   * @param[in] __t The temperature
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  inline real_t accept(real_t __delta_c, real_t __t)
    { return std::exp(-__delta_c / __t); }

  /**
   * Generate a random number in the range [0, 1]
   *
   * @return A random number in the range [0, 1]
   */

  real_t uniform();

  /* acceptance */

  /**
   * The acceptance rate target
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  class acceptance_rate_target
  {
    RLST_ENFORCE_RULE_OF_FOUR(acceptance_rate_target);
  public:
    /**
     * Constructs an acceptance rate target with its parameters
     */

    constexpr acceptance_rate_target(
      real_t __alpha = 50._r,
      iteration_t __max_iterations = 120_it
    ) noexcept
      : m_alpha(__alpha)
      , m_max_iterations(__max_iterations)
    {}

  public:
    /**
     * The acceptance rate target
     *
     * @param[in] __i The current iteration
     * @return The acceptance rate target
     */

    constexpr real_t operator()(iteration_t __i) const
    {
      return
        m_alpha *

        (
          1 -
          (static_cast<real_t>(__i) / static_cast<real_t>(m_max_iterations))
        );
    }
  private:
    real_t m_alpha;
    iteration_t m_max_iterations;
  };

  /**
   * Get the next acceptance scale
   *
   * Due to the wide variety of the circuits to be placed, a fixed temperature
   * schedule does not always produce an appropriate value of the rate of
   * acceptance of new configurations. It was observed that the ideal acceptance
   * rate was 50% in the beginning (@f$ i = 0 @f$) and was reduced to zero at
   * low temperatures (@f$i = i_{max}@f$). To achieve this acceptance
   * rate profile, negative feedback control has been provided.
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  class acceptance_scale
  {
    RLST_ENFORCE_RULE_OF_FOUR(acceptance_scale);
  public:
    /**
     * Constructs an acceptance scale with its parameters
     *
     * @param[in] __acceptance_rate_target The acceptance rate target
     * @param[in] __beta The beta parameter
     */

    constexpr acceptance_scale(
      acceptance_rate_target __acceptance_rate_target = {},
      real_t __alpha = 50._r
    ) noexcept
      : m_acceptance_rate_target(std::move(__acceptance_rate_target))
      , m_alpha(__alpha)
    {}
  public:
    /**
     * Get the next acceptance scale
     *
     * @param[in] __i The current iteration
     * @param[in] __acceptance_rate The current acceptance rate
     * @param[in] __acceptance_scale The current acceptance scale
     *
     * @return The new acceptance scale
     */

    constexpr real_t operator()(
      iteration_t __i,
      real_t __acceptance_rate,
      real_t __acceptance_scale
    ) const
    {
      return
        __acceptance_scale *

        (
          1 +

          (__acceptance_rate - m_acceptance_rate_target(__i)) /
          m_alpha
        );
    }
  private:
    acceptance_rate_target m_acceptance_rate_target;
    real_t m_alpha;
  };

  class scaled_cost
  {
    RLST_ENFORCE_RULE_OF_FOUR(scaled_cost);
  public:
    /**
     * Constructs a scaled cost with its parameters
     *
     * @param[in] __acceptance_scale The acceptance scale
     */

    constexpr scaled_cost(acceptance_scale __acceptance_scale = {}) noexcept
      : m_acceptance_scale(std::move(__acceptance_scale))
    {}
  public:
    constexpr real_t operator()(
      iteration_t __i,
      real_t __acceptance_rate,
      real_t __acceptance_scale,
      real_t __cost
    ) const
    {
      return
        m_acceptance_scale(__i, __acceptance_rate, __acceptance_scale) *
        __cost;
    }
  private:
    acceptance_scale m_acceptance_scale;
  };

  /* temperature */

  /**
   * The temperature schedule
   *
   * It gives the next temperature as a function of the number of iterations or
   * the previous temperature.
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  class temperature_schedule
  {
    RLST_ENFORCE_RULE_OF_FOUR(temperature_schedule);
  public:
    /**
     * Constructs a temperature schedule with its parameter
     *
     * @param[in] __alpha The alpha parameter
     */

    constexpr temperature_schedule(real_t __alpha = .98_r) noexcept
      : m_alpha(__alpha)
    {}
  public:
    /**
     * Get the next temperature
     *
     * @param[in] __t The current temperature
     * @return The next temperature
     */

    constexpr real_t operator()(real_t __t) const noexcept
      { return m_alpha * __t; }
  private:
    real_t m_alpha;
  };

  /* overlap */

  namespace details
  {
    // TODO: add `noexcept` qualifiers

    template <class RowIterator>
    class overlap_grid_iterator
    {
      RLST_ENFORCE_RULE_OF_FOUR(overlap_grid_iterator);
    public:
      using row_iterator_type = RowIterator;

      using column_iterator_type = typename RowIterator::value_type::iterator;

      using const_column_iterator_type =
        typename RowIterator::value_type::const_iterator;

      using value_type =
        typename std::iterator_traits<column_iterator_type>::value_type;

      using difference_type =
        std::common_type_t<
          typename std::iterator_traits<column_iterator_type>::difference_type,
          typename std::iterator_traits<row_iterator_type>::difference_type
        >;

      using pointer =
        typename std::iterator_traits<column_iterator_type>::pointer;

      using reference =
        typename std::iterator_traits<column_iterator_type>::reference;

      using iterator_category = std::random_access_iterator_tag;
    public:
      template <class R>
      friend bool operator==(
        const overlap_grid_iterator<R>&,
        const overlap_grid_iterator<R>&
      );

      template <class R>
      friend bool operator!=(
        const overlap_grid_iterator<R>&,
        const overlap_grid_iterator<R>&
      );

      template <class R>
      friend bool operator<(
        const overlap_grid_iterator<R>&,
        const overlap_grid_iterator<R>&
      );

      template <class R>
      friend bool operator>(
        const overlap_grid_iterator<R>&,
        const overlap_grid_iterator<R>&
      );

      template <class R>
      friend bool operator<=(
        const overlap_grid_iterator<R>&,
        const overlap_grid_iterator<R>&
      );

      template <class R>
      friend bool operator>=(
        const overlap_grid_iterator<R>&,
        const overlap_grid_iterator<R>&
      );

      template <class R>
      friend typename overlap_grid_iterator<R>::difference_type operator-(
        const overlap_grid_iterator<R>&,
        const overlap_grid_iterator<R>&
      );

      template <class R>
      friend void swap(overlap_grid_iterator<R>&, overlap_grid_iterator<R>&);

#ifdef RLST_WITH_TESTS
      template <class R>
      friend void PrintTo(
        const overlap_grid_iterator<R>& __it,
        std::ostream* __os
      );
#endif
    public:
      constexpr overlap_grid_iterator()
        : m_column_iterator()
        , m_row_iterator()
        , m_offset(0)
        , m_row_size(0)
      {}

      explicit constexpr overlap_grid_iterator(
        RowIterator&& __row_iterator,
        difference_type __column_index,
        difference_type __row_size,
        difference_type __offset = 0
      )
        : m_column_iterator(__row_iterator->begin() + __offset + __column_index)
        , m_row_iterator(std::forward<RowIterator>(__row_iterator))
        , m_offset(__offset)
        , m_row_size(__row_size)
      {}
    public:
      constexpr reference operator*() const
        { return *m_column_iterator; }

      constexpr pointer operator->() const
        { return m_column_iterator.operator->(); }

      constexpr overlap_grid_iterator& operator++();
      constexpr overlap_grid_iterator operator++(int);

      overlap_grid_iterator operator--();
      overlap_grid_iterator operator--(int);

      constexpr overlap_grid_iterator& operator+=(difference_type __n);

      constexpr overlap_grid_iterator& operator-=(difference_type __n)
        { return *this += -__n; }

      constexpr overlap_grid_iterator operator-(difference_type __n) const;

      constexpr reference operator[](difference_type __n)
        { return *(*this + __n); }
    public:
      constexpr row_iterator_type row_iterator() const
        { return m_row_iterator; }
    public:
      constexpr difference_type column_index() const
        { return m_column_iterator - column_begin(); }

      constexpr const_column_iterator_type column_begin() const
        { return m_row_iterator->begin() + m_offset; }

      constexpr column_iterator_type column_begin()
        { return m_row_iterator->begin() + m_offset; }

      constexpr const_column_iterator_type column_cbegin() const
        { return m_row_iterator->cbegin() + m_offset; }

      constexpr const_column_iterator_type column_end() const
        { return m_row_iterator->begin() + m_row_size + m_offset; }

      constexpr column_iterator_type column_end()
        { return m_row_iterator->begin() + m_row_size + m_offset; }

      constexpr const_column_iterator_type column_cend() const
        { return m_row_iterator->cbegin() + m_row_size + m_offset; }
    private:
      column_iterator_type m_column_iterator;
      row_iterator_type m_row_iterator;

      difference_type m_offset;
      difference_type m_row_size;
    };

    template <class R>
    bool operator==(
      const overlap_grid_iterator<R>& __lhs,
      const overlap_grid_iterator<R>& __rhs
    );

    template <class R>
    bool operator!=(
      const overlap_grid_iterator<R>& __lhs,
      const overlap_grid_iterator<R>& __rhs
    );

    template <class R>
    bool operator<(
      const overlap_grid_iterator<R>& __lhs,
      const overlap_grid_iterator<R>& __rhs
    );

    template <class R>
    bool operator>(
      const overlap_grid_iterator<R>& __lhs,
      const overlap_grid_iterator<R>& __rhs
    );

    template <class R>
    bool operator<=(
      const overlap_grid_iterator<R>& __lhs,
      const overlap_grid_iterator<R>& __rhs
    );

    template <class R>
    bool operator>=(
      const overlap_grid_iterator<R>& __lhs,
      const overlap_grid_iterator<R>& __rhs
    );

    template <class R>
    constexpr overlap_grid_iterator<R> operator+(
      overlap_grid_iterator<R> __lhs,
      typename overlap_grid_iterator<R>::difference_type __n
    )
      { return __lhs += __n; }

    template <class R>
    typename overlap_grid_iterator<R>::difference_type operator-(
      const overlap_grid_iterator<R>& __lhs,
      const overlap_grid_iterator<R>& __rhs
    );

    template <class R>
    void swap(overlap_grid_iterator<R>& __lhs, overlap_grid_iterator<R>& __rhs);
  }

  /**
   * The overlap grid
   *
   * This data structure aims to provide an easy way to compute the overlap
   * score.
   *
   *
   * @tparam T The type of the data stored in the grid
   * @tparam bin_size The bin size of the grid
   * @tparam Allocator The allocator type used for the grid
   */

  template <typename T, std::uint8_t bin_size>
  class overlap_grid
  {
    template <typename U, std::uint8_t b>
    friend bool operator==(
      const overlap_grid<U, b>&,
      const overlap_grid<U, b>&
    );

    template <typename U, std::uint8_t b>
    friend void swap(overlap_grid<U, b>&, overlap_grid<U, b>&) noexcept;
  public:
    /// The type of the data stored in the grid
    using value_type = T;
  private:
    /// The underlying type of a line of the grid
    using line_type = std::vector<value_type>;

    /// The underlying type of the grid
    using grid_type = std::vector<line_type>;
  public:
    /// A reference to an element of the container
    using reference = std::add_lvalue_reference_t<T>;

    /// A constant reference to an element of the container
    using const_reference = std::add_const_t<std::add_lvalue_reference_t<T>>;

    /// The type of the iterator
    using iterator =
      details::overlap_grid_iterator<typename grid_type::iterator>;

    /// The type of the const iterator
    using const_iterator =
      details::overlap_grid_iterator<typename grid_type::const_iterator>;

    /// The `difference_type` of the underlying iterator types
    using difference_type = typename iterator::difference_type;

    static_assert(
      std::is_same_v<
        typename iterator::difference_type,
        typename const_iterator::difference_type
      >,

      "The difference types of the iterators must be the same"
    );

    /// The type used to represent the size of the container
    using size_type =
      std::common_type_t<
        typename grid_type::size_type,
        typename grid_type::value_type::size_type
      >;
  public:
    /// The type of the reverse iterator
    using reverse_iterator = std::reverse_iterator<iterator>;

    /// The type of the constant reverse iterator
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  public:
    /// The default constructor
    overlap_grid() = default;

    /**
     * Copy constructor
     *
     * @param[in] __other The overlap grid to copy from
     */

    overlap_grid(const overlap_grid& __other) = default;

    /**
     * Move constructor
     *
     * @param[in, out] __other The overlap grid to move from
     */

    overlap_grid(overlap_grid&& __other) = default;

    /// The destructor
    ~overlap_grid() = default;
  public:
    /**
     * Constructs an overlap grid with its parameters
     *
     * @param[in] __width The width of the grid
     * @param[in] __height The height of the grid
     * @param[in, out] __default The default value to use
     * @param[in, out] __a The allocator to use
     */

    overlap_grid(
      size_type __width,
      size_type __height,
      T&& __default = {}
    )
      : m_grid(
        __height + 1,
        line_type(__width, std::forward<T>(__default))
      )
    {}
  public:
    /**
     * Copy assignment operator
     *
     * @param[in] __rhs The overlap grid to copy from
     * @return A reference to the updated overlap grid
     */

    overlap_grid& operator=(const overlap_grid& __rhs) = default;

    /**
     * Move assignment operator
     *
     * @param[in, out] __rhs The overlap grid to move from
     * @return A reference to the updated overlap grid
     */

    overlap_grid& operator=(overlap_grid&& __rhs) noexcept = default;
  public:
    /**
     * Get an iterator to the beginning of the grid
     *
     * @return An iterator to the beginning of the grid
     */

    iterator begin()
      { return iterator(m_grid.begin(), 0, m_grid.front().size()); }

    /**
     * Get a constant iterator to the beginning of the grid
     *
     * @return A constant iterator to the beginning of the grid
     */

    const_iterator begin() const
      { return const_iterator(m_grid.begin(), 0, m_grid.front().size()); }

    /**
     * Get a constant iterator to the beginning of the grid
     *
     * @return A constant iterator to the beginning of the grid
     */

    const_iterator cbegin() const
      { return begin(); }

    /**
     * Get an iterator to the ending of the grid
     *
     * @return An iterator to the ending of the grid
     */

    iterator end()
    {
      return
        iterator(
          m_grid.begin() + row_number(),
          0,
          m_grid.front().size()
        );
    }

    /**
     * Get a constant iterator to the ending of the grid
     *
     * @return A constant iterator to the ending of the grid
     */

    const_iterator end() const
    {
      return
        const_iterator(
          m_grid.begin() + row_number(),
          0,
          m_grid.front().size()
        );
    }

    /**
      * Get a constant iterator to the ending of the grid
      *
      * @return A constant iterator to the ending of the grid
      */

    const_iterator cend() const
      { return end(); }
  public:
    /**
     * Get a reverse iterator to the ending of the grid
     *
     * @return A reverse iterator to the ending of the grid
     */

    reverse_iterator rbegin()
      { return reverse_iterator(end()); }

    /**
     * Get a constant reverse iterator to the ending of the grid
     *
     * @return A constant reverse iterator to the ending of the grid
     */

    const_reverse_iterator rbegin() const
      { return const_reverse_iterator(end()); }

    /**
     * Get a constant reverse iterator to the ending of the grid
     *
     * @return A constant reverse iterator to the ending of the grid
     */

    const_reverse_iterator crbegin() const
      { return rbegin(); }

    /**
     * Get a reverse iterator to the beginning of the grid
     *
     * @return A reverse iterator to the beginning of the grid
     */

    reverse_iterator rend()
      { return reverse_iterator(begin()); }

    /**
     * Get a constant reverse iterator to the beginning of the grid
     *
     * @return A constant reverse iterator to the beginning of the grid
     */

    const_reverse_iterator rend() const
      { return const_reverse_iterator(begin()); }

    /**
     * Get a constant reverse iterator to the beginning of the grid
     *
     * @return A constant reverse iterator to the beginning of the grid
     */

    const_reverse_iterator crend() const
      { return rend(); }
  public:
    /**
     * Get the number of column in the grid
     *
     * @return The number of column in the grid
     */

    size_type column_number() const
      { return m_grid.front().size(); }

    /**
     * Get the number of row in the grid
     *
     * @return The number of row in the grid
     */

    size_type row_number() const noexcept
      { return m_grid.size() - 1; }

    /**
     * Get the size of the container
     *
     * @return The size of the container
     */

    size_type size() const
      { return row_number() * column_number(); }

    /**
     * Get the maximum size of the container
     *
     * @return The maximum size of the container
     */

    size_type max_size() const
      { return m_grid.max_size() * m_grid.front().max_size(); }

    /**
     * Check if the container is empty
     *
     * @return `true` if the container is empty, `false` otherwise
     */

    bool empty() const noexcept
      { return m_grid.empty(); }
  public:
   /**
    * Swap the contents of this overlap grid with another
    *
    * This function exchanges the contents of the current overlap grid
    * with those of the provided overlap grid. The operation is performed
    * in constant time as it only swaps the internal data structures.
    *
    * @param[in, out] __rhs The overlap grid to swap with
    */

    void swap(overlap_grid& __rhs) noexcept
      { return swap(*this, __rhs); }
  private:
    grid_type m_grid;
  };

  /**
   * Equality operator for \ref overlap_grid
   *
   * Compares two overlap_grid objects for equality. Two \ref overlap_grid
   * objects are considered equal if their internal grid structures are
   * identical.
   *
   * @tparam U The type of the data stored in the grid
   * @tparam b The bin size of the grid
   * @tparam A The allocator type used for the grid
   *
   * @param[in] __lhs The left-hand side \ref overlap_grid to compare
   * @param[in] __rhs The right-hand side \ref overlap_grid to compare
   *
   * @return `true` if the two \ref overlap_grid objects are equal, `false`
   * otherwise
   */

  template <typename U, std::uint8_t b>
  bool operator==(
    const overlap_grid<U, b>& __lhs,
    const overlap_grid<U, b>& __rhs
  )
  {
    return
      (__lhs.size() == __rhs.size()) &&
      std::equal(__lhs.cbegin(), __lhs.cend(), __rhs.cbegin());
  }

  /**
   * Inequality operator for \ref overlap_grid
   *
   * Compares two overlap_grid objects for inequality. Two \ref overlap_grid
   * objects are considered unequal if their internal grid structures are
   * different.
   *
   * @tparam U The type of the data stored in the grid
   * @tparam b The bin size of the grid
   * @tparam A The allocator type used for the grid
   *
   * @param[in] __lhs The left-hand side \ref overlap_grid to compare
   * @param[in] __rhs The right-hand side \ref overlap_grid to compare
   *
   * @return `true` if the two \ref overlap_grid objects are not equal, `false`
   * otherwise
   */

  template <typename U, std::uint8_t b>
  bool operator!=(
    const overlap_grid<U, b>& __lhs,
    const overlap_grid<U, b>& __rhs
  )
    { return !(__lhs == __rhs); }

  /**
   * Swap two \ref overlap_grid objects
   *
   * Exchanges the contents of two \ref overlap_grid objects. This operation is
   * performed in constant time as it only swaps the internal data structures
   * of the two objects.
   *
   * @tparam U The type of the data stored in the grid
   * @tparam b The bin size of the grid
   * @tparam A The allocator type used for the grid
   *
   * @param[in, out] __lhs The first \ref overlap_grid object
   * @param[in, out] __rhs The second \ref overlap_grid object
   */

  template <typename U, std::uint8_t b>
  void swap(
    overlap_grid<U, b>& __lhs,
    overlap_grid<U, b>& __rhs
  ) noexcept;

  /**
   * The overlap penalty target
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  class overlap_penalty_target
  {
    RLST_ENFORCE_RULE_OF_FOUR(overlap_penalty_target);
  public:
    /**
     * Constructs an overlap penalty target with its parameters
     */

    constexpr overlap_penalty_target(
      real_t __alpha = 1.4_r,
      real_t __beta = 0.15_r,
      real_t __desired_row_length = 0._r,
      iteration_t __max_iterations = 120_it
    ) noexcept
      : m_alpha(__alpha)
      , m_beta(__beta)
      , m_desired_row_length(__desired_row_length)
      , m_max_iterations(__max_iterations)
    {}
  public:
    /**
     * Get the next overlap penalty target
     *
     * @param[in] __i The current iteration
     * @return The next overlap penalty target
     */

    constexpr real_t operator()(iteration_t __i) const
    {
      return
        (
          m_alpha -

          m_beta * (
            static_cast<real_t>(__i) /
            static_cast<real_t>(m_max_iterations)
          )
        ) * m_desired_row_length;
    }
  private:
    real_t m_alpha;
    real_t m_beta;
    real_t m_desired_row_length;
    iteration_t m_max_iterations;
  };

  /**
   * The overlap penalty weight
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  class overlap_penalty_weight
  {
    RLST_ENFORCE_RULE_OF_FOUR(overlap_penalty_weight);
  public:
    /**
     * Constructs an overlap penalty weight with its parameters
     *
     * @param[in] __overlap_penalty_target The overlap penalty target
     * @param[in] __desired_row_length The desired row length
     */

    constexpr overlap_penalty_weight(
      overlap_penalty_target __overlap_penalty_target = {},
      real_t __desired_row_length = 0._r
    ) noexcept
      : m_overlap_penalty_target(std::move(__overlap_penalty_target))
      , m_desired_row_length(__desired_row_length)
    {}
  public:
    /**
     * Gets the next overlap penalty weight
     *
     * @param[in] __overlap_penalty_weight The current overlap penalty weight
     * @param[in] __i The current iteration
     *
     * @return The next overlap penalty weight
     */

    constexpr real_t operator()(
      real_t __overlap_penalty_weight,
      real_t __overlap_penalty,
      iteration_t __i
    ) const
    {
      return
        std::max(
          0._r,

          __overlap_penalty_weight +

          (__overlap_penalty - m_overlap_penalty_target(__i)) /
          m_desired_row_length
        );
    }
  private:
    overlap_penalty_target m_overlap_penalty_target;
    real_t m_desired_row_length;
  };

  /**
   * The overlap penalty cost
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  class overlap_penalty_cost
  {
    RLST_ENFORCE_RULE_OF_FOUR(overlap_penalty_cost);
  public:
    /**
     * Constructs an overlap penalty cost with its parameters
     *
     * @param[in] __overlap_penalty_weight The overlap penalty weight
     */

    constexpr overlap_penalty_cost(
      overlap_penalty_weight __overlap_penalty_weight = {}
    ) noexcept
      : m_overlap_penalty_weight(std::move(__overlap_penalty_weight))
    {}
  public:
    constexpr real_t operator()(
      real_t __overlap_penalty_weight,
      real_t __overlap_penalty,
      iteration_t __i
    ) const
    {
      return
        m_overlap_penalty_weight(
          __overlap_penalty_weight,
          __overlap_penalty,
          __i
        ) * __overlap_penalty;
    }
  private:
    overlap_penalty_weight m_overlap_penalty_weight;
  };

  /* row length */

  /**
   * The row length control penalty target
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  class row_length_penalty_target
  {
    RLST_ENFORCE_RULE_OF_FOUR(row_length_penalty_target);
  public:
    /**
     * Constructs a row length penalty target with its parameters
     *
     * @param[in] __alpha The alpha parameter
     * @param[in] __beta The beta parameter
     * @param[in] __desired_row_length The desired row length
     * @param[in] __max_iterations The maximum number of iterations
     */

    constexpr row_length_penalty_target(
      real_t __alpha = 5._r,
      real_t __beta = 4._r,
      real_t __desired_row_length = 0._r,
      iteration_t __max_iterations = 120_it,
      real_t __xi = 0._r
    ) noexcept
      : m_alpha(__alpha)
      , m_beta(__beta)
      , m_desired_row_length(__desired_row_length)
      , m_max_iterations(__max_iterations)
      , m_xi(__xi)
    {}
  public:
    /**
     * Get the next row length penalty target
     *
     * @param[in] __i The current iteration
     * @return The next row length penalty target
     */

    constexpr real_t operator()(iteration_t __i) const
    {
      return
        m_alpha -

        m_beta * (
          static_cast<real_t>(__i) / static_cast<real_t>(m_max_iterations)
        ) * m_xi * m_desired_row_length;
    }
  private:
    real_t m_alpha;
    real_t m_beta;
    real_t m_desired_row_length;
    iteration_t m_max_iterations;
    real_t m_xi;
  };

  /**
   * The row length control weight
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  class row_length_weight
  {
    RLST_ENFORCE_RULE_OF_FOUR(row_length_weight);
  public:
    /**
     * Constructs a row length weight with its parameters
     *
     * @param[in] __row_length_penalty_target The row length penalty target
     */

    constexpr row_length_weight(
      row_length_penalty_target __row_length_penalty_target = {}
    ) noexcept
      : m_row_length_penalty_target(std::move(__row_length_penalty_target))
    {}
  public:
    /**
     * Get the next row length weight
     *
     * @param[in] __i The current iteration
     * @param[in] __row_length_weight The current row length weight
     * @param[in] __row_length_penalty The current row length penalty
     *
     * @return The next row length weight
     */

    constexpr real_t operator()(
      iteration_t __i,
      real_t __row_length_weight,
      real_t __row_length_penalty
    ) const
    {
      real_t rlp_target = m_row_length_penalty_target(__i);

      return
        std::max(
          0._r,
          __row_length_weight + (__row_length_penalty - rlp_target) / rlp_target
        );
    }
  private:
    row_length_penalty_target m_row_length_penalty_target;
  };

  /**
   * The row length cost
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  class row_length_cost
  {
    RLST_ENFORCE_RULE_OF_FOUR(row_length_cost);
  public:
    /**
     * Constructs a row length cost with its parameters
     *
     * @param[in] __row_length_weight The row length weight
     */

    constexpr row_length_cost(row_length_weight __row_length_weight = {})
      : m_row_length_weight(std::move(__row_length_weight))
    {}
  public:
    /**
     * Get the next row length cost
     *
     * @param[in] __i The current iteration
     * @param[in] __row_length_weight The current row length weight
     * @param[in] __row_length_penalty The current row length penalty
     *
     * @return The next row length cost
     */

    constexpr real_t operator()(
      iteration_t __i,
      real_t __row_length_weight,
      real_t __row_length_penalty
    ) const
    {
      return
        m_row_length_weight(__i, __row_length_weight, __row_length_penalty) *
        __row_length_penalty;
    }
  private:
    row_length_weight m_row_length_weight;
  };

  /**
   * Compute the wire length cost of nets
   *
   * @param[in] __begin The begin iterator of the nets
   * @param[in] __end The end iterator of the nets
   *
   * @return The wire length cost of the nets
   */

  template <class InputIt>
  constexpr real_t wire_length_cost(InputIt __begin, InputIt __end)
  {
    return
      std::reduce(
        __begin,
        __end,
        0._r,

        [] (
          real_t __acc,
          const std::pair<cell::PlacedPort, cell::PlacedPort>& __net
        )
        {
          return
            __acc +
            __net.first.port.position.manhattan(__net.second.port.position);
        }
      );
  }
}

#include "placement.ipp"

#endif // RLST_RLST_PAR_PLACEMENT_HPP
