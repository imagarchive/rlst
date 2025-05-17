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


#include "core/grid.hpp"
#include <gtest/gtest.h>

using namespace rlst::core;

using rlst::par::literal_t;
using rlst::par::uliteral_t;

namespace rlst::core::details
{
  template <class R>
  void PrintTo(
    const grid_iterator<R>& __it,
    std::ostream* __os
  )
  {
    auto x = &*__it.m_column_iterator;

    *__os
      << "grid_iterator { "
        << "column_iterator = " << x << " (" << *x << "), "
        << "row_iterator = " << &*__it.m_row_iterator << ", "
        << "column_index = " << __it.column_index() << ", "
        << "offset = " << __it.m_offset << ", "
        << "row_size = " << __it.m_row_size
      << " }";
  }
}

TEST(grid_iterator__basics__test, is_constructible)
{
  std::vector<std::vector<int>> grid(1);
  details::grid_iterator(grid.begin(), 0, 0);
}

TEST(grid_iterator__basics__test, is_default_constructible)
{
  using vector = std::vector<std::vector<int>>;
  details::grid_iterator<decltype(std::declval<vector>().begin())>();
}

TEST(grid_iterator__basics__test, incompatible_ne)
{
  EXPECT_DEATH(
    {
      std::vector<std::vector<int>> grid(1);

      details::grid_iterator lhs(grid.begin(), 0, 0);
      details::grid_iterator rhs(grid.begin(), 0, 1);

      static_cast<void>(lhs != rhs);
    },

    "precondition assertion.*failed"
  );
}

TEST(grid_iterator__basics__test, incompatible_sub)
{
  EXPECT_DEATH(
    {
      std::vector<std::vector<int>> grid(1);

      details::grid_iterator lhs(grid.begin(), 0, 0);
      details::grid_iterator rhs(grid.begin(), 0, 1);

      static_cast<void>(lhs - rhs);
    },

    "precondition assertion.*failed"
  );
}

class grid_iterator__multi_pass_guarantee__test
  : public testing::TestWithParam<int>
{};

TEST_P(grid_iterator__multi_pass_guarantee__test, increment)
{
  int row_size = GetParam();

  std::vector<std::vector<int>> grid(4, std::vector<int>(4));

  details::grid_iterator lhs(grid.begin(), 0, row_size);
  details::grid_iterator rhs(grid.begin(), 0, row_size);

  EXPECT_EQ(lhs, rhs);
  EXPECT_EQ(++lhs, ++rhs);
}

TEST_P(grid_iterator__multi_pass_guarantee__test, invariant)
{
  int row_size = GetParam();

  std::vector<std::vector<int>> grid(4, std::vector<int>(4));

  details::grid_iterator a(grid.begin(), 0, row_size);
  details::grid_iterator c(a);
  details::grid_iterator b(grid.begin(), 0, row_size);

  ++c;

  EXPECT_EQ(*a, *b);
}

INSTANTIATE_TEST_SUITE_P(
  same_row,
  grid_iterator__multi_pass_guarantee__test,
  testing::Range(2, 5)
);

INSTANTIATE_TEST_SUITE_P(
  jump_row,
  grid_iterator__multi_pass_guarantee__test,
  testing::Values(1)
);

class grid_iterator__output__test
  : public testing::TestWithParam<int>
{
protected:
  grid_iterator__output__test()
    : grid(4, std::vector<int>(4, 0))
  {}
protected:
  std::vector<std::vector<int>> grid;
};

TEST_P(grid_iterator__output__test, forward)
{
  int index = GetParam();
  details::grid_iterator a(grid.begin(), 0, 3);

  a[index] = 1;

  ASSERT_EQ(a[index], 1);
}

TEST_P(grid_iterator__output__test, backward)
{
  int index = -GetParam();
  details::grid_iterator a(grid.begin() + 2, 2, 3);

  a[index] = 1;

  ASSERT_EQ(a[index], 1);
}

INSTANTIATE_TEST_SUITE_P(
  same_row,
  grid_iterator__output__test,
  testing::Range(0, 3)
);

INSTANTIATE_TEST_SUITE_P(
  jump_row,
  grid_iterator__output__test,
  testing::Range(3, 9)
);

TEST(grid_iterator__basics__test, bidirectional)
{
  std::vector<std::vector<int>> grid(2, std::vector<int>(2, 0));

  details::grid_iterator a(grid.begin(), 1, 2);
  details::grid_iterator b(grid.begin(), 1, 2);

  ASSERT_EQ(a, b);
  ASSERT_EQ(--a, --b);
}

TEST(grid_iterator__cmp__test, lt)
{
  std::vector<std::vector<int>> grid(2, std::vector<int>(2, 0));

  details::grid_iterator a(grid.begin(), 0, 2);
  details::grid_iterator b(grid.begin(), 1, 2);

  ASSERT_LT(a, b);
}

TEST(grid_iterator__cmp__test, gt)
{
  std::vector<std::vector<int>> grid(2, std::vector<int>(2, 0));

  details::grid_iterator a(grid.begin(), 1, 2);
  details::grid_iterator b(grid.begin(), 0, 2);

  ASSERT_GT(a, b);
}

TEST(grid_iterator__cmp__test, le)
{
  std::vector<std::vector<int>> grid(2, std::vector<int>(2, 0));

  details::grid_iterator a(grid.begin(), 0, 2);
  details::grid_iterator b(grid.begin(), 0, 2);

  ASSERT_GE(a, b);
}

TEST(grid_iterator__cmp__test, ge)
{
  std::vector<std::vector<int>> grid(2, std::vector<int>(2, 0));

  details::grid_iterator a(grid.begin(), 0, 2);
  details::grid_iterator b(grid.begin(), 0, 2);

  ASSERT_GE(a, b);
}

class grid_iterator__select__test : public testing::TestWithParam<int>
{
protected:
  grid_iterator__select__test()
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

TEST_P(grid_iterator__select__test, forward)
{
  int offset = GetParam();

  details::grid_iterator it(grid.begin(), 0, 4 - offset, offset);

  for (int i = 0; i != 4; ++i) {
    for (int j = offset; j != 4; ++j) {
      ASSERT_EQ(*it, j + 4 * i);
      ++it;
    }
  }
}

TEST_P(grid_iterator__select__test, backward)
{
  int offset = GetParam();

  // Force starting at the bottom right corner

  details::grid_iterator it(
    grid.begin() + 3,
    3 - offset,
    4 - offset,
    offset
  );

  for (int i = 3; i >= 0; --i) {
    for (int j = 3; j >= offset; --j) {
      ASSERT_EQ(*it, j + 4 * i);
      --it;
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
  no_offset,
  grid_iterator__select__test,
  testing::Values(0)
);

INSTANTIATE_TEST_SUITE_P(
  with_offset,
  grid_iterator__select__test,
  testing::Range(1, 4)
);

class grid_iterator__increment__test
  : public testing::TestWithParam<int>
{};

TEST_P(grid_iterator__increment__test, forward)
{
  int n = GetParam();

  std::vector<std::vector<int>> grid(4, std::vector<int>(4, 0));

  details::grid_iterator a(grid.begin(), 0, 4);
  details::grid_iterator b(grid.begin(), 0, 4);

  a += n;

  for (int i = n; i != 0; --i) {
    ++b;
  }

  ASSERT_EQ(a, b);
}

TEST_P(grid_iterator__increment__test, backward)
{
  int n = GetParam();

  std::vector<std::vector<int>> grid(4, std::vector<int>(4, 0));

  details::grid_iterator a(grid.begin() + 3, 3, 4);
  details::grid_iterator b(grid.begin() + 3, 3, 4);

  a -= n;

  for (int i = n; i != 0; --i) {
    --b;
  }

  ASSERT_EQ(a, b);
}

INSTANTIATE_TEST_SUITE_P(
  same_row,
  grid_iterator__increment__test,
  testing::Range(0, 4)
);

INSTANTIATE_TEST_SUITE_P(
  jump_one,
  grid_iterator__increment__test,
  testing::Range(4, 8)
);

INSTANTIATE_TEST_SUITE_P(
  jump_two,
  grid_iterator__increment__test,
  testing::Range(8, 12)
);

INSTANTIATE_TEST_SUITE_P(
  jump_three,
  grid_iterator__increment__test,
  testing::Range(12, 16)
);

class grid__iterator__test : public testing::Test
{
protected:
  grid__iterator__test()
    : g(20, 20, 1)
  {}
protected:
  grid<int> g;
};

TEST_F(grid__iterator__test, select_forward)
{
  for (auto i = g.begin(); i != g.end(); ++i) {
    ASSERT_EQ(*i, 1);
  }
}

TEST_F(grid__iterator__test, select_backward)
{
  for (auto i = g.rbegin(); i != g.rend(); ++i) {
    ASSERT_EQ(*i, 1);
  }
}

TEST_F(grid__iterator__test, distance)
{
  ASSERT_EQ(g.end() - g.begin(), 400);
}

TEST(grid__size__test, basic)
{
  grid<int> grid(20, 20);
  ASSERT_EQ(grid.size(), 400);
}

TEST(grid__size__test, empty)
{
  grid<int> grid;
  EXPECT_DEATH({ grid.size(); }, "");
}
