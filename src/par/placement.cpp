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


#include "par/placement.hpp"
#include <random>

namespace rlst::par
{
  namespace
  {
    bool is_outside(
      const Point& __top_left,
      const cell::Cell& __cell,
      uliteral_t __width,
      uliteral_t __height
    ) noexcept
    {
      Point bottom_right(
        __top_left.x() + static_cast<literal_t>(__cell.type->size.width),
        __top_left.y() + static_cast<literal_t>(__cell.type->size.height),
        0_l
      );

      return
        (__top_left.x() < 0_l) ||
        (__top_left.y() < 0_l) ||
        (bottom_right.x() >= static_cast<literal_t>(__width)) ||
        (bottom_right.y() >= static_cast<literal_t>(__height));
    }
  }

  real_t uniform()
  {
    std::random_device device;
    std::mt19937 generator(device());
    std::uniform_real_distribution distribution(0._r, 1._r);

    return distribution(generator);
  }

  /* placement_grid */

  std::pair<placement_grid::iterator, placement_grid::iterator>
  placement_grid::insert(cell::Cell& __cell)
  {
    auto ret = rect(__cell);

    for (auto i = ret.first; i != ret.second; ++i) {
      i->push_front(__cell);
    }

    return ret;
  }

  std::pair<placement_grid::iterator, placement_grid::iterator>
  placement_grid::erase(cell::Cell& __cell)
  {
    auto ret = rect(__cell);

    for (auto i = ret.first; i != ret.second; ++i) {
      i->remove(__cell);
    }

    return ret;
  }

  /* evolutor */

  void evolutor::insert(cell::Cell& __cell)
  {
    m_cells.push_back(__cell);

    m_overlap_grid.insert(__cell);
    m_placement_grid.insert(__cell);
  }

  void evolutor::replace(cell::Cell& __lhs, cell::Cell& __rhs)
  {
    using std::swap;

    m_overlap_grid.erase(__lhs);
    m_placement_grid.erase(__lhs);

    m_overlap_grid.erase(__rhs);
    m_placement_grid.erase(__rhs);

    swap(__lhs.position, __rhs.position);

    m_overlap_grid.insert(__lhs);
    m_placement_grid.insert(__lhs);

    m_overlap_grid.insert(__rhs);
    m_placement_grid.insert(__rhs);
  }

  void evolutor::replace(cell::Cell& __lhs, const Point& __rhs)
  {
    m_overlap_grid.erase(__lhs);
    m_placement_grid.erase(__lhs);

    __lhs.position = __rhs;

    m_overlap_grid.insert(__lhs);
    m_placement_grid.insert(__lhs);
  }

  std::variant<
    std::pair<std::reference_wrapper<cell::Cell>, Point>,

    std::pair<
      std::reference_wrapper<cell::Cell>,
      std::reference_wrapper<cell::Cell>
    >
  >
  evolutor::evolute()
  {
    using std::swap;

    auto from = core::choice(m_cells.begin(), m_cells.end());

    Point to =
      random_neighbor(
        *from,
        m_placement_grid.column_number(),
        m_placement_grid.row_number()
      );

    m_overlap_grid.erase(*from);
    m_placement_grid.erase(*from);

    swap(from->get().position, to);

    m_overlap_grid.insert(*from);
    m_placement_grid.insert(*from);

    auto [begin, end] = m_placement_grid.rect(*from);

    auto to_swap =
      std::find_if(
        begin,
        end,

        [&] (const auto& __list) {
          if (__list.empty()) {
            return false;
          } else {
            const cell::Cell& cell = __list.front();

            return
              (&cell != &from->get()) &&

              !is_outside(
                to,
                cell,
                m_placement_grid.column_number(),
                m_placement_grid.row_number()
              );
          }
        }
      );

    if (to_swap == end) {
      return std::make_pair(*from, to);
    } else {
      cell::Cell& cell = to_swap->front();

      m_overlap_grid.erase(cell);
      m_placement_grid.erase(cell);

      cell.position = to;

      m_overlap_grid.insert(cell);
      m_placement_grid.insert(cell);

      return std::make_pair(*from, std::ref(cell));
    }
  }

  void swap(evolutor& __lhs, evolutor& __rhs)
  {
    using std::swap;

    swap(__lhs.m_cells, __rhs.m_cells);
    swap(__lhs.m_overlap_grid, __rhs.m_overlap_grid);
    swap(__lhs.m_placement_grid, __rhs.m_placement_grid);
  }

  Point random_neighbor(
    const cell::Cell& __cell,
    uliteral_t __width,
    uliteral_t __height
  )
  {
    Point ret(
      static_cast<literal_t>(__width),
      static_cast<literal_t>(__height),
      0
    );

    while (is_outside(ret, __cell, __width, __height)) {
      literal_t delta_x =
        static_cast<literal_t>(
          core::randint(
            -__cell.type->size.width,
            __cell.type->size.width
          )
        );

      literal_t delta_y =
        static_cast<literal_t>(
          core::randint(
            -__cell.type->size.height,
            __cell.type->size.height
          )
        );

      ret =
        Point(
          __cell.position.x() + delta_x,
          __cell.position.y() + delta_y,
          0
        );
    }

    return ret;
  }
}
