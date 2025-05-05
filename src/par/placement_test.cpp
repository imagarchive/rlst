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
#include <gtest/gtest.h>

using namespace rlst::par;

namespace rlst::par::details
{
  template <class R>
  void PrintTo(
    const overlap_grid_iterator<R>& __it,
    std::ostream* __os
  )
  {
    auto x = &*__it.m_column_iterator;

    *__os
      << "overlap_grid_iterator { "
        << "column_iterator = " << x << " (" << *x << "), "
        << "row_iterator = " << &*__it.m_row_iterator << ", "
        << "column_index = " << __it.column_index() << ", "
        << "row_size = " << __it.m_row_size
      << " }";
  }
}

TEST(overlap_grid_iterator__basics__test, is_constructible)
{
  std::vector<std::vector<int>> grid(1);
  details::overlap_grid_iterator(grid.begin(), 0, 0);
}

TEST(overlap_grid_iterator__basics__test, is_default_constructible)
{
  using vector = std::vector<std::vector<int>>;
  details::overlap_grid_iterator<decltype(std::declval<vector>().begin())>();
}

TEST(overlap_grid_iterator__basics__test, incompatible_ne)
{
  EXPECT_DEATH(
    {
      std::vector<std::vector<int>> grid(1);

      details::overlap_grid_iterator lhs(grid.begin(), 0, 0);
      details::overlap_grid_iterator rhs(grid.begin(), 0, 1);

      static_cast<void>(lhs != rhs);
    },

    "precondition assertion.*failed"
  );
}

TEST(overlap_grid_iterator__basics__test, incompatible_sub)
{
  EXPECT_DEATH(
    {
      std::vector<std::vector<int>> grid(1);

      details::overlap_grid_iterator lhs(grid.begin(), 0, 0);
      details::overlap_grid_iterator rhs(grid.begin(), 0, 1);

      static_cast<void>(lhs - rhs);
    },

    "precondition assertion.*failed"
  );
}

class overlap_grid_iterator__multi_pass_guarantee__test
  : public testing::TestWithParam<int>
{};

TEST_P(overlap_grid_iterator__multi_pass_guarantee__test, increment)
{
  int row_size = GetParam();

  std::vector<std::vector<int>> grid(4, std::vector<int>(4));

  details::overlap_grid_iterator lhs(grid.begin(), 0, row_size);
  details::overlap_grid_iterator rhs(grid.begin(), 0, row_size);

  EXPECT_EQ(lhs, rhs);
  EXPECT_EQ(++lhs, ++rhs);
}

TEST_P(overlap_grid_iterator__multi_pass_guarantee__test, invariant)
{
  int row_size = GetParam();

  std::vector<std::vector<int>> grid(4, std::vector<int>(4));

  details::overlap_grid_iterator a(grid.begin(), 0, row_size);
  details::overlap_grid_iterator c(a);
  details::overlap_grid_iterator b(grid.begin(), 0, row_size);

  ++c;

  EXPECT_EQ(*a, *b);
}

INSTANTIATE_TEST_SUITE_P(
  same_row,
  overlap_grid_iterator__multi_pass_guarantee__test,
  testing::Range(2, 5)
);

INSTANTIATE_TEST_SUITE_P(
  jump_row,
  overlap_grid_iterator__multi_pass_guarantee__test,
  testing::Values(1)
);

class overlap_grid_iterator__output__test
  : public testing::TestWithParam<int>
{
protected:
  overlap_grid_iterator__output__test()
    : grid(4, std::vector<int>(4, 0))
  {}
protected:
  std::vector<std::vector<int>> grid;
};

TEST_P(overlap_grid_iterator__output__test, forward)
{
  int index = GetParam();
  details::overlap_grid_iterator a(grid.begin(), 0, 3);

  a[index] = 1;

  ASSERT_EQ(a[index], 1);
}

TEST_P(overlap_grid_iterator__output__test, backward)
{
  int index = -GetParam();
  details::overlap_grid_iterator a(grid.begin() + 2, 2, 3);

  a[index] = 1;

  ASSERT_EQ(a[index], 1);
}

INSTANTIATE_TEST_SUITE_P(
  same_row,
  overlap_grid_iterator__output__test,
  testing::Range(0, 3)
);

INSTANTIATE_TEST_SUITE_P(
  jump_row,
  overlap_grid_iterator__output__test,
  testing::Range(3, 9)
);

TEST(overlap_grid_iterator__basics__test, bidirectional)
{
  std::vector<std::vector<int>> grid(2, std::vector<int>(2, 0));

  details::overlap_grid_iterator a(grid.begin(), 1, 2);
  details::overlap_grid_iterator b(grid.begin(), 1, 2);

  ASSERT_EQ(a, b);
  ASSERT_EQ(--a, --b);
}

TEST(overlap_grid_iterator__cmp__test, lt)
{
  std::vector<std::vector<int>> grid(2, std::vector<int>(2, 0));

  details::overlap_grid_iterator a(grid.begin(), 0, 2);
  details::overlap_grid_iterator b(grid.begin(), 1, 2);

  ASSERT_LT(a, b);
}

TEST(overlap_grid_iterator__cmp__test, gt)
{
  std::vector<std::vector<int>> grid(2, std::vector<int>(2, 0));

  details::overlap_grid_iterator a(grid.begin(), 1, 2);
  details::overlap_grid_iterator b(grid.begin(), 0, 2);

  ASSERT_GT(a, b);
}

TEST(overlap_grid_iterator__cmp__test, le)
{
  std::vector<std::vector<int>> grid(2, std::vector<int>(2, 0));

  details::overlap_grid_iterator a(grid.begin(), 0, 2);
  details::overlap_grid_iterator b(grid.begin(), 0, 2);

  ASSERT_GE(a, b);
}

TEST(overlap_grid_iterator__cmp__test, ge)
{
  std::vector<std::vector<int>> grid(2, std::vector<int>(2, 0));

  details::overlap_grid_iterator a(grid.begin(), 0, 2);
  details::overlap_grid_iterator b(grid.begin(), 0, 2);

  ASSERT_GE(a, b);
}

class overlap_grid_iterator__select__test : public testing::Test
{
protected:
  overlap_grid_iterator__select__test()
    : grid()
  {
    grid.reserve(4);

      for (int i = 0; i != 4; ++i) {
        std::vector<int> line;
        line.reserve(4);

        for (int j = 0; j != 4; ++j) {
          line.push_back(i * 4 + j);
        }

        grid.push_back(std::move(line));
      }
  }
protected:
  std::vector<std::vector<int>> grid;
};

TEST_F(overlap_grid_iterator__select__test, forward)
{
  details::overlap_grid_iterator it(grid.begin(), 0, 4);

  for (int i = 0; i != 16; ++i) {
    ASSERT_EQ(*it, i);
    ++it;
  }
}

TEST_F(overlap_grid_iterator__select__test, backward)
{
  details::overlap_grid_iterator it(grid.begin() + 3, 3, 4);

  for (int i = 15; i >= 0; --i) {
    ASSERT_EQ(*it, i);
    --it;
  }
}

class overlap_grid_iterator__increment__test
  : public testing::TestWithParam<int>
{};

TEST_P(overlap_grid_iterator__increment__test, forward)
{
  int n = GetParam();

  std::vector<std::vector<int>> grid(4, std::vector<int>(4, 0));

  details::overlap_grid_iterator a(grid.begin(), 0, 4);
  details::overlap_grid_iterator b(grid.begin(), 0, 4);

  a += n;

  for (int i = n; i != 0; --i) {
    ++b;
  }

  ASSERT_EQ(a, b);
}

TEST_P(overlap_grid_iterator__increment__test, backward)
{
  int n = GetParam();

  std::vector<std::vector<int>> grid(4, std::vector<int>(4, 0));

  details::overlap_grid_iterator a(grid.begin() + 3, 3, 4);
  details::overlap_grid_iterator b(grid.begin() + 3, 3, 4);

  a -= n;

  for (int i = n; i != 0; --i) {
    --b;
  }

  ASSERT_EQ(a, b);
}

INSTANTIATE_TEST_SUITE_P(
  same_row,
  overlap_grid_iterator__increment__test,
  testing::Range(0, 4)
);

INSTANTIATE_TEST_SUITE_P(
  jump_one,
  overlap_grid_iterator__increment__test,
  testing::Range(4, 8)
);

INSTANTIATE_TEST_SUITE_P(
  jump_two,
  overlap_grid_iterator__increment__test,
  testing::Range(8, 12)
);

INSTANTIATE_TEST_SUITE_P(
  jump_three,
  overlap_grid_iterator__increment__test,
  testing::Range(12, 16)
);
