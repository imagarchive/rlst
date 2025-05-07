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


namespace rlst::par
{
  namespace details
  {
    template <class R>
    constexpr overlap_grid_iterator<R>&
    overlap_grid_iterator<R>::operator++()
    {
      if (m_column_iterator == (column_end() - 1)) {
        ++m_row_iterator;
        m_column_iterator = column_begin();
      } else {
        ++m_column_iterator;
      }

      return *this;
    }

    template <class R>
    constexpr overlap_grid_iterator<R>
    overlap_grid_iterator<R>::operator++(int)
    {
      overlap_grid_iterator old = *this;
      ++(*this);
      return old;
    }

    template <class R>
    overlap_grid_iterator<R> overlap_grid_iterator<R>::operator--()
    {
      boost::contract::old_ptr old = BOOST_CONTRACT_OLDOF(*this);

      boost::contract::check c =
        boost::contract::public_function(this)

          .postcondition(
            [&] {
              overlap_grid_iterator copy = *this;
              BOOST_CONTRACT_ASSERT(++copy == *old);
            }
          );

      if (m_column_iterator == column_begin()) {
        --m_row_iterator;
        m_column_iterator = column_end() - 1;
      } else {
        --m_column_iterator;
      }

      return *this;
    }

    template <class R>
    overlap_grid_iterator<R> overlap_grid_iterator<R>::operator--(int)
    {
      overlap_grid_iterator old = *this;
      --(*this);
      return old;
    }

    template <class R>
    constexpr overlap_grid_iterator<R>&
    overlap_grid_iterator<R>::operator+=(difference_type __n)
    {
      __n += column_index();

      if (__n < 0) {
        ++__n;
        m_row_iterator += __n / m_row_size - 1;
        __n %= m_row_size;
        __n += m_row_size - 1;
      } else {
        m_row_iterator += __n / m_row_size;
        __n %= m_row_size;
      }

      m_column_iterator = column_begin() + __n;

      return *this;
    }

    template <class R>
    constexpr overlap_grid_iterator<R>
    overlap_grid_iterator<R>::operator-(difference_type __n) const
    {
      overlap_grid_iterator copy = *this;
      copy -= __n;
      return copy;
    }

    template <class R>
    bool operator==(
      const overlap_grid_iterator<R>& __lhs,
      const overlap_grid_iterator<R>& __rhs
    )
    {
      boost::contract::check c =
        boost::contract::function()
          .precondition(
            [&] { BOOST_CONTRACT_ASSERT(__lhs.m_row_size == __rhs.m_row_size); }
          );

      return
        (__lhs.m_column_iterator == __rhs.m_column_iterator) &&
        (__lhs.m_row_iterator == __rhs.m_row_iterator);
    }

    template <class R>
    bool operator!=(
      const overlap_grid_iterator<R>& __lhs,
      const overlap_grid_iterator<R>& __rhs
    )
    {
      boost::contract::check c =
        boost::contract::function()
          .precondition(
            [&] { BOOST_CONTRACT_ASSERT(__lhs.m_row_size == __rhs.m_row_size); }
          );

      return !(__lhs == __rhs);
    }


    template <class R>
    bool operator<(
      const overlap_grid_iterator<R>& __lhs,
      const overlap_grid_iterator<R>& __rhs
    )
    {
      boost::contract::check c =
        boost::contract::function()

          .precondition(
            [&] { BOOST_CONTRACT_ASSERT(__lhs.m_row_size == __rhs.m_row_size); }
          );

      return (__lhs - __rhs) < 0;
    }

    template <class R>
    bool operator>(
      const overlap_grid_iterator<R>& __lhs,
      const overlap_grid_iterator<R>& __rhs
    )
    {
      boost::contract::check c =
        boost::contract::function()

          .precondition(
            [&] { BOOST_CONTRACT_ASSERT(__lhs.m_row_size == __rhs.m_row_size); }
          );

      return __rhs < __lhs;
    }

    template <class R>
    bool operator<=(
      const overlap_grid_iterator<R>& __lhs,
      const overlap_grid_iterator<R>& __rhs
    )
    {
      boost::contract::check c =
        boost::contract::function()

          .precondition(
            [&] { BOOST_CONTRACT_ASSERT(__lhs.m_row_size == __rhs.m_row_size); }
          );

      return !(__lhs > __rhs);
    }

    template <class R>
    bool operator>=(
      const overlap_grid_iterator<R>& __lhs,
      const overlap_grid_iterator<R>& __rhs
    )
    {
      boost::contract::check c =
        boost::contract::function()

          .precondition(
            [&] { BOOST_CONTRACT_ASSERT(__lhs.m_row_size == __rhs.m_row_size); }
          );

      return !(__lhs < __rhs);
    }

    template <class R>
    typename overlap_grid_iterator<R>::difference_type operator-(
      const overlap_grid_iterator<R>& __lhs,
      const overlap_grid_iterator<R>& __rhs
    )
    {
      typename overlap_grid_iterator<R>::difference_type ret;

      boost::contract::check c =
        boost::contract::function()

          .precondition(
            [&] { BOOST_CONTRACT_ASSERT(__lhs.m_row_size == __rhs.m_row_size); }
          )

          .postcondition([&] { BOOST_CONTRACT_ASSERT(__lhs == __rhs + ret); });

      ret =
        (__lhs.m_row_iterator - __rhs.m_row_iterator) * __lhs.m_row_size +
        (__lhs.column_index() - __rhs.column_index());

      return ret;
    }

    template <class R>
    void swap(overlap_grid_iterator<R>& __lhs, overlap_grid_iterator<R>& __rhs)
    {
      using std::swap;

      swap(__lhs.m_column_iterator, __rhs.m_column_iterator);
      swap(__lhs.m_row_iterator, __rhs.m_row_iterator);
      swap(__lhs.m_row_size, __rhs.m_row_size);
    }
  }

  template <class U, std::uint8_t b>
  template <bool is_inserting>
  std::pair<
    typename overlap_grid<U, b>::iterator,
    typename overlap_grid<U, b>::iterator
  >
  overlap_grid<U, b>::insert(const cell::Cell& __cell)
  {
    auto ret = rect(__cell);

    for (auto i = ret.first; i != ret.second; ++i) {
      value_type v =
        static_cast<value_type>(
          std::min(
            __cell.position.x() +
              static_cast<literal_t>(__cell.type->size.width),

            (i.x() + 1) * b
          ) -

          std::max(__cell.position.x(), i.x() * b)
        );

      if constexpr (is_inserting) {
        *i += v;
      } else {
        *i -= v;
      }
    }

    return ret;
  }

  template<class U, std::uint8_t b>
  void overlap_grid<U, b>::replace(cell::Cell& __lhs, cell::Cell& __rhs)
  {
    using std::swap;

    erase(__lhs);
    erase(__rhs);

    swap(__lhs.position, __rhs.position);

    insert(__lhs);
    insert(__rhs);
  }

  template <typename U, std::uint8_t b>
  void swap(overlap_grid<U, b>& __lhs, overlap_grid<U, b>& __rhs) noexcept
  {
    using std::swap;
    swap(__lhs.m_grid, __rhs.m_grid);
  }

  namespace details
  {
    struct row_length_penalty_binop
    {
      constexpr real_t operator()(real_t __lhs, real_t __rhs) const noexcept
        { return __lhs + __rhs; }

      constexpr real_t operator()(
        const std::pair<literal_t, std::pair<literal_t, literal_t>>& __lhs,
        real_t __rhs
      ) const noexcept
      {
        return
          operator()(
            static_cast<real_t>(__lhs.second.second - __lhs.second.first),
            __rhs
          );
      }

      constexpr real_t operator()(
        real_t __lhs,
        const std::pair<literal_t, std::pair<literal_t, literal_t>>& __rhs
      ) const noexcept
        { return operator()(__rhs, __lhs); }

      constexpr real_t operator()(
        const std::pair<literal_t, std::pair<literal_t, literal_t>>& __lhs,
        const std::pair<literal_t, std::pair<literal_t, literal_t>>& __rhs
      ) const noexcept
      {
        return
          operator()(
            static_cast<real_t>(__lhs.second.second - __lhs.second.first),
            __rhs
          );
      }
    };
  }

  template <class InputIt>
  real_t row_length_penalty(InputIt __begin, InputIt __end)
  {
    std::unordered_map<
      literal_t,
      std::pair<literal_t, literal_t>
    > row_to_min_max;

    if constexpr (is_random_access_iterator_v<InputIt>) {
      row_to_min_max.reserve(__end - __begin);
    }

    for (; __begin != __end; ++__begin) {
      auto i =
        row_to_min_max.insert(
          std::make_pair(
            __begin->position.y(),

            std::make_pair(
              std::numeric_limits<literal_t>::max(),
              std::numeric_limits<literal_t>::min()
            )
          )
        ).first;

      i->second =
        std::make_pair(
          std::min(i->second.first, __begin->position.x()),

          std::max(
            i->second.second,

            __begin->position.x() +
              static_cast<literal_t>(__begin->type->size.width)
          )
        );
    }

  return
    std::reduce(
      row_to_min_max.cbegin(),
      row_to_min_max.cend(),
      0._r,
      details::row_length_penalty_binop {}
    ) / static_cast<real_t>(row_to_min_max.size());
  }
}
