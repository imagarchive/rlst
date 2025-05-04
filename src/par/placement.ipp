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
    template <class C, class R>
    constexpr overlap_grid_iterator<C, R>&
    overlap_grid_iterator<C, R>::operator++()
    {
      if (m_column_index == m_row_size) {
        ++m_row_iterator;
        m_column_iterator = m_row_iterator->begin();
        m_column_index = 0;
      } else {
        ++m_column_iterator;
      }

      return *this;
    }

    template <class C, class R>
    constexpr overlap_grid_iterator<C, R>
    overlap_grid_iterator<C, R>::operator++(int)
    {
      overlap_grid_iterator old = *this;
      ++(*this);
      return old;
    }

    template <class C, class R>
    overlap_grid_iterator<C, R> overlap_grid_iterator<C, R>::operator--()
    {
      boost::contract::check c =
        boost::contract::function()
          .postcondition([&] { BOOST_CONTRACT_ASSERT(--(++*this) == *this); });

      if (m_column_index == 0) {
        --m_row_iterator;
        m_column_iterator = m_row_iterator->end() - 1;
        m_column_index = m_row_size - 1;
      } else {
        --m_column_iterator;
      }

      return *this;
    }

    template <class C, class R>
    overlap_grid_iterator<C, R> overlap_grid_iterator<C, R>::operator--(int)
    {
      overlap_grid_iterator old = *this;
      --(*this);
      return old;
    }

    template <class C, class R>
    constexpr overlap_grid_iterator<C, R>&
    overlap_grid_iterator<C, R>::operator+=(difference_type __n)
    {
      difference_type rem = __n % static_cast<difference_type>(m_row_size);

      m_row_iterator += __n / static_cast<difference_type>(m_row_size);
      m_column_iterator += rem;
      m_column_index = rem;

      return *this;
    }

    template <class C, class R>
    constexpr overlap_grid_iterator<C, R>
    overlap_grid_iterator<C, R>::operator-(difference_type __n) const
    {
      overlap_grid_iterator copy = *this;
      copy -= __n;
      return copy;
    }

    template <class C, class R>
    bool operator==(
      const overlap_grid_iterator<C, R>& __lhs,
      const overlap_grid_iterator<C, R>& __rhs
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

    template <class C, class R>
    bool operator!=(
      const overlap_grid_iterator<C, R>& __lhs,
      const overlap_grid_iterator<C, R>& __rhs
    )
    {
      boost::contract::check c =
        boost::contract::function()
          .precondition(
            [&] { BOOST_CONTRACT_ASSERT(__lhs.m_row_size == __rhs.m_row_size); }
          );

      return !(__lhs == __rhs);
    }


    template <class C, class R>
    bool operator<(
      const overlap_grid_iterator<C, R>& __lhs,
      const overlap_grid_iterator<C, R>& __rhs
    )
    {
      boost::contract::check c =
        boost::contract::function()

          .precondition(
            [&] { BOOST_CONTRACT_ASSERT(__lhs.m_row_size == __rhs.m_row_size); }
          );

      return (__lhs - __rhs) < 0;
    }

    template <class C, class R>
    bool operator>(
      const overlap_grid_iterator<C, R>& __lhs,
      const overlap_grid_iterator<C, R>& __rhs
    )
    {
      boost::contract::check c =
        boost::contract::function()

          .precondition(
            [&] { BOOST_CONTRACT_ASSERT(__lhs.m_row_size == __rhs.m_row_size); }
          );

      return __rhs < __lhs;
    }

    template <class C, class R>
    bool operator<=(
      const overlap_grid_iterator<C, R>& __lhs,
      const overlap_grid_iterator<C, R>& __rhs
    )
    {
      boost::contract::check c =
        boost::contract::function()

          .precondition(
            [&] { BOOST_CONTRACT_ASSERT(__lhs.m_row_size == __rhs.m_row_size); }
          );

      return !(__lhs > __rhs);
    }

    template <class C, class R>
    bool operator>=(
      const overlap_grid_iterator<C, R>& __lhs,
      const overlap_grid_iterator<C, R>& __rhs
    )
    {
      boost::contract::check c =
        boost::contract::function()

          .precondition(
            [&] { BOOST_CONTRACT_ASSERT(__lhs.m_row_size == __rhs.m_row_size); }
          );

      return !(__lhs < __rhs);
    }

    template <class C, class R>
    typename overlap_grid_iterator<C, R>::difference_type operator-(
      const overlap_grid_iterator<C, R>& __lhs,
      const overlap_grid_iterator<C, R>& __rhs
    )
    {
      typename overlap_grid_iterator<C, R>::difference_type ret;

      boost::contract::check c =
        boost::contract::function()

          .precondition(
            [&] { BOOST_CONTRACT_ASSERT(__lhs.m_row_size == __rhs.m_row_size); }
          )

          .postcondition([&] { BOOST_CONTRACT_ASSERT(__lhs == __rhs + ret); });

      ret =
        (__lhs.m_row_iterator - __rhs.m_row_iterator) * __lhs.m_row_size +
        (__lhs.m_column_index - __rhs.m_column_index);

      return ret;
    }

    template <class C, class R>
    void swap(
      overlap_grid_iterator<C, R>& __lhs,
      overlap_grid_iterator<C, R>& __rhs
    )
    {
      using std::swap;

      swap(__lhs.m_column_iterator, __rhs.m_column_iterator);
      swap(__lhs.m_row_iterator, __rhs.m_row_iterator);
      swap(__lhs.m_column_index, __rhs.m_column_index);
      swap(__lhs.m_row_size, __rhs.m_row_size);
    }
  }
}
