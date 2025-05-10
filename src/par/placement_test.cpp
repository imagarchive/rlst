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
#include <gmock/gmock.h>

#include "test.hpp"

using namespace rlst::par;
using namespace rlst::test;

using rlst::par::cell::Cell;
using rlst::par::cell::Port;
using rlst::par::cell::PortType;

class overlap_grid__modifiers__test
  : public testing::TestWithParam<std::tuple<int, int>>
{
protected:
  using iterator = overlap_grid::iterator;
  using size_type = overlap_grid::size_type;
  using value_type = overlap_grid::value_type;
protected:
  overlap_grid__modifiers__test()
    : g(overlap_grid::bin_size * 20, overlap_grid::bin_size * 20)
  {}
protected:
  template <
    std::pair<iterator, iterator> (overlap_grid::*modifier)(Cell&),
    value_type val
  >
  void check_modifier()
  {
    auto width = static_cast<uliteral_t>(std::get<0>(GetParam()));
    auto height = static_cast<uliteral_t>(std::get<1>(GetParam()));

    Cell c = cell_from_geometry_params(0, 0, 8 * width, 8 * height);
    auto [begin, end] = g.rect(c);

    (g.*modifier)(c);

    ASSERT_EQ(
      std::count(g.begin(), g.end(), 0),
      g.size() - static_cast<size_type>(end - begin)
    );

    ASSERT_EQ(std::count(begin, end, val), end - begin);
  }
protected:
  overlap_grid g;
};

TEST_P(overlap_grid__modifiers__test, insert)
  { check_modifier<&overlap_grid::insert, 8>(); }

TEST_P(overlap_grid__modifiers__test, erase)
  { check_modifier<&overlap_grid::erase, -8>(); }

INSTANTIATE_TEST_SUITE_P(
  same_row,
  overlap_grid__modifiers__test,

  testing::Combine(
    testing::Range(1, 8),
    testing::Values(1)
  )
);

INSTANTIATE_TEST_SUITE_P(
  same_column,
  overlap_grid__modifiers__test,

  testing::Combine(
    testing::Values(1),
    testing::Range(1, 8)
  )
);

INSTANTIATE_TEST_SUITE_P(
  many_rows_and_columns,
  overlap_grid__modifiers__test,

  testing::Combine(
    testing::Range(2, 8),
    testing::Range(2, 8)
  )
);

TEST(row_length_penalty__basics__test, empty)
{
  std::vector<Cell> cells;
  real_t penalty = row_length_penalty(cells.cbegin(), cells.cend());

  ASSERT_THAT(penalty, testing::IsNan());
}

TEST(row_length_penalty__basics__test, one)
{
  std::vector<Cell> cells = {
    cell_from_geometry_params(1_l, 1_l, 10_ul, 10_ul)
  };

  real_t penalty = row_length_penalty(cells.cbegin(), cells.cend());

  ASSERT_DOUBLE_EQ(penalty, 10._r);
}

TEST(row_length_penalty__basics__test, same_line)
{
  std::vector<Cell> cells = {
    cell_from_geometry_params(1_l, 1_l, 10_ul, 10_ul),
    cell_from_geometry_params(1_l, 1_l, 12_ul, 10_ul),
    cell_from_geometry_params(1_l, 1_l, 14_ul, 10_ul),
    cell_from_geometry_params(1_l, 1_l, 10_ul, 10_ul),
    cell_from_geometry_params(1_l, 1_l, 8_ul, 10_ul),
    cell_from_geometry_params(0_l, 1_l, 4_ul, 10_ul)
  };

  real_t penalty = row_length_penalty(cells.cbegin(), cells.cend());

  ASSERT_DOUBLE_EQ(penalty, 15._r);
}

TEST(row_length_penalty__basics__test, several_lines)
{
  std::vector<Cell> cells = {
    cell_from_geometry_params(1_l, 1_l, 10_ul, 10_ul),
    cell_from_geometry_params(1_l, 1_l, 12_ul, 10_ul),
    cell_from_geometry_params(1_l, 1_l, 14_ul, 10_ul),
    cell_from_geometry_params(1_l, 1_l, 10_ul, 10_ul),
    cell_from_geometry_params(1_l, 1_l, 8_ul, 10_ul),
    cell_from_geometry_params(0_l, 1_l, 4_ul, 10_ul),

    cell_from_geometry_params(3_l, 5_l, 2_ul, 10_ul),
    cell_from_geometry_params(0_l, 5_l, 1_ul, 10_ul),
    cell_from_geometry_params(1_l, 5_l, 2_ul, 10_ul),
    cell_from_geometry_params(1_l, 5_l, 3_ul, 10_ul),

    cell_from_geometry_params(0_l, 3_l, 5_ul, 10_ul),
    cell_from_geometry_params(2_l, 3_l, 8_ul, 10_ul),

    cell_from_geometry_params(2_l, 10_l, 12_ul, 10_ul),
    cell_from_geometry_params(0_l, 10_l, 10_ul, 10_ul),
  };

  real_t penalty = row_length_penalty(cells.cbegin(), cells.cend());

  ASSERT_DOUBLE_EQ(penalty, 11._r);
}

TEST(wire_length_cost__basics__test, same_row)
{
  std::vector<std::pair<Port, Port>> nets = {
    std::make_pair(
      Port { Point(0_l, 0_l, 0_l), PortType::output },
      Port { Point(10_l, 0_l, 0_l), PortType::input }
    )
  };

  ASSERT_EQ(
    wire_length_cost(nets.cbegin(), nets.cend()),
    10._r
  );
}

TEST(wire_length_cost__basics__test, same_column)
{
  std::vector<std::pair<Port, Port>> nets = {
    std::make_pair(
      Port { Point(0_l, 0_l, 0_l), PortType::output },
      Port { Point(0_l, 10_l, 0_l), PortType::input }
    )
  };

  ASSERT_EQ(
    wire_length_cost(nets.cbegin(), nets.cend()),
    10._r
  );
}

TEST(wire_length_cost__basics__test, random)
{
  std::vector<std::pair<Port, Port>> nets = {
    std::make_pair(
      Port { Point(10_l, 0_l, 0_l), PortType::output },
      Port { Point(0_l, 10_l, 0_l), PortType::input }
    )
  };

  ASSERT_EQ(
    wire_length_cost(nets.cbegin(), nets.cend()),
    20._r
  );
}
