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
        (__lhs.m_column_iterator - __rhs.m_column_iterator);

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
}
