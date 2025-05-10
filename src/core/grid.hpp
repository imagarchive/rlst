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


#ifndef RLST_RLST_CORE_GRID_HPP
#  define RLST_RLST_CORE_GRID_HPP

#include "core.hpp"
#include "par/cell/Cell.hpp"

#include <boost/contract.hpp>
#include <cstdint>
#include <vector>

namespace rlst::core
{

#ifndef RLST_DOXYGEN_SHOULD_SKIP_THIS

  namespace details
  {
    // TODO: add `noexcept` qualifiers

    template <class RowIterator>
    class grid_iterator
    {
      RLST_ENFORCE_RULE_OF_FOUR(grid_iterator);
    public:
      using row_iterator_type = RowIterator;

      /*
       * This type is equal to
       * - `typename std::iterator_traits<RowIterator>::value_type::iterator`, or
       * - `typename std::iterator_traits<RowIterator>::value_type::const_iterator`.
       *
       * According to the constness of the `value_type` behind `RowIterator`,
       * one is choosen over the other.
       */

      using column_iterator_type =
        decltype(std::begin(*std::declval<RowIterator>()));

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
    private:
      template <class R>
      friend bool operator==(const grid_iterator<R>&, const grid_iterator<R>&);

      template <class R>
      friend bool operator!=(const grid_iterator<R>&, const grid_iterator<R>&);

      template <class R>
      friend bool operator<(const grid_iterator<R>&, const grid_iterator<R>&);

      template <class R>
      friend bool operator>(const grid_iterator<R>&, const grid_iterator<R>&);

      template <class R>
      friend bool operator<=(const grid_iterator<R>&, const grid_iterator<R>&);

      template <class R>
      friend bool operator>=(const grid_iterator<R>&, const grid_iterator<R>&);

      template <class R>
      friend typename grid_iterator<R>::difference_type
      operator-(const grid_iterator<R>&, const grid_iterator<R>&);

      template <class R>
      friend void swap(grid_iterator<R>&, grid_iterator<R>&);

#ifdef RLST_WITH_TESTS
      template <class R>
      friend void PrintTo(
        const grid_iterator<R>& __it,
        std::ostream* __os
      );
#endif
    public:
      constexpr grid_iterator()
        : m_column_iterator()
        , m_row_iterator()
        , m_offset(0)
        , m_row_size(0)
      {}

      template <class U>
      explicit constexpr grid_iterator(
        U&& __row_iterator,
        difference_type __column_index,
        difference_type __row_size,
        difference_type __offset = 0
      )
        : m_column_iterator(__row_iterator->begin() + __offset + __column_index)
        , m_row_iterator(std::forward<U>(__row_iterator))
        , m_offset(__offset)
        , m_row_size(__row_size)
      {}
    public:
      constexpr reference operator*() const
        { return *m_column_iterator; }

      constexpr pointer operator->() const
        { return m_column_iterator.operator->(); }

      constexpr grid_iterator& operator++();
      constexpr grid_iterator operator++(int);

      grid_iterator operator--();
      grid_iterator operator--(int);

      constexpr grid_iterator& operator+=(difference_type __n);

      constexpr grid_iterator& operator-=(difference_type __n)
        { return *this += -__n; }

      constexpr grid_iterator operator-(difference_type __n) const;

      constexpr reference operator[](difference_type __n) const
        { return *(*this + __n); }
    public:
      constexpr row_iterator_type row_iterator() const
        { return m_row_iterator; }
    public:
      constexpr difference_type column_index() const
        { return m_column_iterator - column_begin(); }

      constexpr column_iterator_type column_begin() const
        { return m_row_iterator->begin() + m_offset; }

      constexpr column_iterator_type column_end() const
        { return m_row_iterator->begin() + m_row_size + m_offset; }
    public:
      constexpr difference_type x() const
        { return m_offset + column_index(); }
    private:
      column_iterator_type m_column_iterator;
      row_iterator_type m_row_iterator;

      difference_type m_offset;
      difference_type m_row_size;
    };

    template <class U>
    grid_iterator(
      U&&,
      typename grid_iterator<U>::difference_type,
      typename grid_iterator<U>::difference_type,
      typename grid_iterator<U>::difference_type
    ) -> grid_iterator<U>;

    template <class U>
    grid_iterator(
      U&&,
      typename grid_iterator<U>::difference_type,
      typename grid_iterator<U>::difference_type
    ) -> grid_iterator<U>;

    template <class R>
    bool operator==(
      const grid_iterator<R>& __lhs,
      const grid_iterator<R>& __rhs
    );

    template <class R>
    bool operator!=(
      const grid_iterator<R>& __lhs,
      const grid_iterator<R>& __rhs
    );

    template <class R>
    bool operator<(
      const grid_iterator<R>& __lhs,
      const grid_iterator<R>& __rhs
    );

    template <class R>
    bool operator>(
      const grid_iterator<R>& __lhs,
      const grid_iterator<R>& __rhs
    );

    template <class R>
    bool operator<=(
      const grid_iterator<R>& __lhs,
      const grid_iterator<R>& __rhs
    );

    template <class R>
    bool operator>=(
      const grid_iterator<R>& __lhs,
      const grid_iterator<R>& __rhs
    );

    template <class R>
    constexpr grid_iterator<R> operator+(
      grid_iterator<R> __lhs,
      typename grid_iterator<R>::difference_type __n
    )
      { return __lhs += __n; }

    template <class R>
    typename grid_iterator<R>::difference_type
    operator-(const grid_iterator<R>& __lhs, const grid_iterator<R>& __rhs);

    template <class R>
    void swap(grid_iterator<R>& __lhs, grid_iterator<R>& __rhs);
  }

#endif // RLST_DOXYGEN_SHOULD_SKIP_THIS

  /**
   * A grid
   */

  template <typename T, std::uint8_t bs>
  class grid
  {
    template <typename U, std::uint8_t b>
    friend bool operator==(
      const grid<U, b>&,
      const grid<U, b>&
    );

    template <typename U, std::uint8_t b>
    friend void swap(grid<U, b>&, grid<U, b>&) noexcept;
  public:
    /// The type of the data stored in the grid
    using value_type = T;

    /// The size of the bins
    static constexpr std::uint8_t bin_size = bs;
  private:
    using line_type = std::vector<value_type>;
    using grid_type = std::vector<line_type>;
  public:
    /// A reference to an element of the container
    using reference = std::add_lvalue_reference_t<T>;

    /// A constant reference to an element of the container
    using const_reference = std::add_const_t<std::add_lvalue_reference_t<T>>;

    /// The type of the iterator
    using iterator =
      details::grid_iterator<typename grid_type::iterator>;

    /// The type of the const iterator
    using const_iterator =
      details::grid_iterator<typename grid_type::const_iterator>;

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
    grid() = default;

    /**
     * Copy constructor
     *
     * @param[in] __other The overlap grid to copy from
     */

    grid(const grid& __other) = default;

    /**
     * Move constructor
     *
     * @param[in, out] __other The overlap grid to move from
     */

    grid(grid&& __other) = default;

    /// The destructor
    ~grid() = default;
  public:
    /**
     * Constructs an overlap grid with its parameters
     *
     * @param[in] __width The width of the grid
     * @param[in] __height The height of the grid
     * @param[in, out] __default The default value to use
     */

    grid(
      size_type __width,
      size_type __height,
      T __default = {}
    )
      : m_grid(
        __height / bin_size + 2,
        line_type(__width / bin_size, std::move(__default))
      )
    {}
  public:
    /**
     * Copy assignment operator
     *
     * @param[in] __rhs The overlap grid to copy from
     * @return A reference to the updated overlap grid
     */

    grid& operator=(const grid& __rhs) = default;

    /**
     * Move assignment operator
     *
     * @param[in, out] __rhs The overlap grid to move from
     * @return A reference to the updated overlap grid
     */

    grid& operator=(grid&& __rhs) noexcept = default;
  public:
    /**
     * Get an iterator to the beginning of the grid
     *
     * @return An iterator to the beginning of the grid
     */

    iterator begin()
      { return iterator(m_grid.begin() + 1, 0, m_grid.front().size()); }

    /**
     * Get a constant iterator to the beginning of the grid
     *
     * @return A constant iterator to the beginning of the grid
     */

    const_iterator begin() const
      { return const_iterator(m_grid.begin() + 1, 0, m_grid.front().size()); }

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
          m_grid.begin() + 1 + row_number(),
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
          m_grid.begin() + 1 + row_number(),
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
      { return m_grid.size() - 2; }

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

    void swap(grid& __rhs) noexcept
      { return swap(*this, __rhs); }
  public:
    /**
     * Get the top left and bottom right iterator
     *
     * @param[in] __cell The given @ref par::cell::Cell
     * @return The top left and bottom right iterator
     */

    std::pair<iterator, iterator> rect(const par::cell::Cell& __cell)
      { return std::make_pair(top_left(__cell), bottom_right(__cell)); }

    /**
     * Get the top left and bottom right iterator
     *
     * @param[in] __cell The given @ref par::cell::Cell
     * @return The top left and bottom right iterator
     */

    std::pair<const_iterator, const_iterator>
    rect(const par::cell::Cell& __cell) const
      { return std::make_pair(top_left(__cell), bottom_right(__cell)); }

    /**
     * Get the number of bins that intersect with the given @ref par::cell::Cell
     * on the x axis
     *
     * @param[in] __cell The given @ref par::cell::Cell
     *
     * @return The number of bins that intersect with the given
     * @ref par::cell::Cell on the x axis
     */

    std::size_t width_of(const par::cell::Cell& __cell) const
    {
      return
        ceil_divide(__cell.position.x() + __cell.type->size.width, bin_size) -
        __cell.position.x() / bin_size;
    }

    /**
     * Get the number of bins that intersect with the given
     * @ref par::cell::Cell on the y axis
     *
     * @param[in] __cell The given @ref par::cell::Cell
     *
     * @return The number of bins that intersect with the given
     * @ref par::cell::Cell on the y axis
     */

    std::size_t height_of(const par::cell::Cell& __cell) const
    {
      return
        ceil_divide(__cell.position.y() + __cell.type->size.height, bin_size) -
        __cell.position.y() / bin_size;
    }

    /**
     * Get the top left iterator
     *
     * The top left iterator is the one pointing to the first element of the
     * submatrix corresponding to the bins intersecting with the given
     * @ref par::cell::Cell.
     *
     * @param[in]  __cell The given @ref par::cell::Cell
     * @return The top left iterator
     */

    iterator top_left(const par::cell::Cell& __cell)
    {
      return
        iterator(
          m_grid.begin() + 1 + __cell.position.y() / bin_size,
          0,
          static_cast<difference_type>(width_of(__cell)),
          __cell.position.x() / bin_size
        );
    }

    /**
     * Get the top left iterator
     *
     * The top left iterator is the one pointing to the first element of the
     * submatrix corresponding to the bins intersecting with the given
     * @ref par::cell::Cell.
     *
     * @param[in]  __cell The given @ref par::cell::Cell
     * @return The top left iterator
     */

    const_iterator top_left(const par::cell::Cell& __cell) const
      { return top_left(__cell); }

    /**
     * Get the bottom right iterator
     *
     * The bottom right iterator is the one pointing to the past-the-last
     * element of the submatrix corresponding to the bins intersecting with the
     * given @ref par::cell::Cell.
     *
     * Thus, the returned iterator is, in fact, not the bottom right iterator
     * but the one just after it.
     *
     * @param[in] __cell The bottom right iterator
     * @return The bottom right iterator
     */

    iterator bottom_right(const par::cell::Cell& __cell)
    {
      return
        iterator(
          m_grid.begin() + 1 + height_of(__cell),
          0,
          static_cast<difference_type>(width_of(__cell)),
          __cell.position.x() / bin_size
        );
    }

    /**
     * Get the bottom right iterator
     *
     * The bottom right iterator is the one pointing to the past-the-last
     * element of the submatrix corresponding to the bins intersecting with the
     * given @ref par::cell::Cell.
     *
     * Thus, the returned iterator is, in fact, not the bottom right iterator
     * but the one just after it.
     *
     * @param[in] __cell The bottom right iterator
     * @return The bottom right iterator
     */

    const_iterator bottom_right(const par::cell::Cell& __cell) const
      { return bottom_right(__cell); }
  public:
    /**
     * Insert a new @ref cell::Cell in the grid
     *
     * @param[in] The @ref cell::Cell to insert
     * @return The _rectangle_ of this @ref cell::Cell
     *
     * @see rect()
     */

    virtual std::pair<iterator, iterator> insert(par::cell::Cell& __cell)
      { return {}; }

    /**
     * Insert new @ref cell::Cell "cells" in the grid
     *
     * @tparam InputIt The type of the iterator
     *
     * @param[in] __begin The begin iterator
     * @param[in] __end The past-the-last iterator
     */

    template <class InputIt>
    void insert(InputIt __begin, InputIt __end)
    {
      std::for_each(
        std::move(__begin),
        std::move(__end),
        [&] (const par::cell::Cell& __cell) { insert(__cell); }
      );
    }

    /**
     * Erase a @ref cell::Cell from the grid
     *
     * @param[in] The @ref cell::Cell to erase
     * @return The _rectangle_ of this @ref cell::Cell
     *
     * @see rect()
     */

    virtual std::pair<iterator, iterator> erase(par::cell::Cell& __cell)
      { return {}; }

    /**
     * Erase @ref cell::Cell "cells" from the grid
     *
     * @tparam The type of the iterator
     *
     * @param[in] __begin The begin iterator
     * @param[in] __end The past-the-last iterator
     */

    template <class InputIt>
    void erase(InputIt __begin, InputIt __end)
    {
      std::for_each(
        std::move(__begin),
        std::move(__end),
        [&] (const par::cell::Cell& __cell) { erase(__cell); }
      );
    }

    /**
     * Replace a @ref cell::Cell by another in the grid
     *
     * @note Positions of these cells are swapped
     *
     * @param[in] __lhs The left-hand side @ref cell::Cell
     * @param[in] __rhs The right-hand side @ref cell::Cell
     */

    void replace(const par::cell::Cell& __lhs, const par::cell::Cell& __rhs);
  private:
    grid_type m_grid;
  };

  /**
   * Equality operator for @ref grid
   *
   * Compares two grid objects for equality. Two @ref grid objects are
   * considered equal if their internal grid structures are identical.
   *
   * @tparam U The type of the data stored in the grid
   * @tparam b The bin size of the grid
   *
   * @param[in] __lhs The left-hand side @ref grid to compare
   * @param[in] __rhs The right-hand side @ref grid to compare
   *
   * @return `true` if the two \ref grid objects are equal, `false`
   * otherwise
   */

  template <typename U, std::uint8_t b>
  bool operator==(const grid<U, b>& __lhs, const grid<U, b>& __rhs)
  {
    return
      (__lhs.size() == __rhs.size()) &&
      std::equal(__lhs.cbegin(), __lhs.cend(), __rhs.cbegin());
  }

  /**
   * Inequality operator for @ref grid
   *
   * Compares two grid objects for inequality. Two @ref grid objects are
   * considered unequal if their internal grid structures are different.
   *
   * @tparam U The type of the data stored in the grid
   * @tparam b The bin size of the grid
   *
   * @param[in] __lhs The left-hand side @ref grid to compare
   * @param[in] __rhs The right-hand side @ref grid to compare
   *
   * @return `true` if the two \ref grid objects are not equal, `false`
   * otherwise
   */

  template <typename U, std::uint8_t b>
  bool operator!=(const grid<U, b>& __lhs, const grid<U, b>& __rhs)
    { return !(__lhs == __rhs); }

  /**
   * Swap two @ref grid objects
   *
   * Exchanges the contents of two @ref grid objects. This operation is
   * performed in constant time as it only swaps the internal data structures
   * of the two objects.
   *
   * @tparam U The type of the data stored in the grid
   * @tparam b The bin size of the grid
   *
   * @param[in, out] __lhs The first @ref grid object
   * @param[in, out] __rhs The second @ref grid object
   */

  template <typename U, std::uint8_t b>
  void swap(grid<U, b>& __lhs, grid<U, b>& __rhs) noexcept;
}

#include "grid.ipp"

#endif // RLST_RLST_CORE_GRID_HPP
