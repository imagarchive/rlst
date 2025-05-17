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
using rlst::par::cell::PlacedPort;
using rlst::par::cell::PortType;

class placement_grid__modifiers__test
  : public testing::TestWithParam<std::tuple<uliteral_t, uliteral_t>>
{
protected:
  placement_grid__modifiers__test()
    : g(5, 5)
  {}
protected:
  placement_grid g;
};

TEST_P(placement_grid__modifiers__test, identity)
{
  auto [width, height] = GetParam();
  Cell cell = cell_from_geometry_params(1_l, 1_l, width, height);

  g.insert(cell);
  g.erase(cell);

  ASSERT_TRUE(
    std::all_of(
      g.cbegin(),
      g.cend(),
      [] (const auto& __list) { return __list.empty(); }
    )
  );
}

TEST_P(placement_grid__modifiers__test, identity_width_two)
{
  auto [width, height] = GetParam();

  Cell first = cell_from_geometry_params(1_l, 1_l, width, height);
  Cell second = cell_from_geometry_params(1_l, 1_l, width, height);

  g.insert(first);
  g.insert(second);

  g.erase(second);
  g.erase(first);

  ASSERT_TRUE(
    std::all_of(
      g.cbegin(),
      g.cend(),
      [] (const auto& __list) { return __list.empty(); }
    )
  );
}

TEST_P(placement_grid__modifiers__test, insert)
{
  auto [width, height] = GetParam();
  Cell cell = cell_from_geometry_params(1_l, 1_l, width, height);

  g.insert(cell);

  ASSERT_EQ(
    std::count_if(
      g.cbegin(),
      g.cend(),

      [&] (const auto& __list) {
        return !__list.empty() && (&__list.front().get() == &cell);
      }
    ),

    width * height
  );
}

TEST_P(placement_grid__modifiers__test, one_above_another)
{
  auto [width, height] = GetParam();

  Cell first = cell_from_geometry_params(1_l, 1_l, width, height);
  Cell second = cell_from_geometry_params(1_l, 1_l, width, height);

  g.insert(first);
  g.insert(second);

  ASSERT_EQ(
    std::count_if(
      g.cbegin(),
      g.cend(),

      [&] (const auto& __list) {
        return !__list.empty() && (&__list.front().get() == &second);
      }
    ),

    width * height
  );
}

INSTANTIATE_TEST_SUITE_P(
  column,
  placement_grid__modifiers__test,

  testing::Combine(
    testing::Range(1_ul, 2_ul),
    testing::Range(1_ul, 5_ul)
  )
);

INSTANTIATE_TEST_SUITE_P(
  row,
  placement_grid__modifiers__test,

  testing::Combine(
    testing::Range(1_ul, 5_ul),
    testing::Range(1_ul, 2_ul)
  )
);

INSTANTIATE_TEST_SUITE_P(
  any,
  placement_grid__modifiers__test,

  testing::Combine(
    testing::Range(2_ul, 5_ul),
    testing::Range(2_ul, 5_ul)
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
  Cell cell = cell_from_geometry_params(0, 0, 10, 10);

  std::vector<std::pair<PlacedPort, PlacedPort>> nets = {
    std::make_pair(
      PlacedPort { cell, Port { Point(0_l, 0_l, 0_l), PortType::output } },
      PlacedPort { cell, Port { Point(10_l, 0_l, 0_l), PortType::input } }
    )
  };

  ASSERT_EQ(
    wire_length_cost(nets.cbegin(), nets.cend()),
    10._r
  );
}

TEST(wire_length_cost__basics__test, same_column)
{
  Cell cell = cell_from_geometry_params(0, 0, 10, 10);

  std::vector<std::pair<PlacedPort, PlacedPort>> nets = {
    std::make_pair(
      PlacedPort { cell, Port { Point(0_l, 0_l, 0_l), PortType::output } },
      PlacedPort { cell, Port { Point(0_l, 10_l, 0_l), PortType::input } }
    )
  };

  ASSERT_EQ(
    wire_length_cost(nets.cbegin(), nets.cend()),
    10._r
  );
}

TEST(wire_length_cost__basics__test, random)
{
  Cell cell = cell_from_geometry_params(0, 0, 10, 10);

  std::vector<std::pair<PlacedPort, PlacedPort>> nets = {
    std::make_pair(
      PlacedPort { cell, Port { Point(10_l, 0_l, 0_l), PortType::output } },
      PlacedPort { cell, Port { Point(0_l, 10_l, 0_l), PortType::input } }
    )
  };

  ASSERT_EQ(
    wire_length_cost(nets.cbegin(), nets.cend()),
    20._r
  );
}
