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
  real_t uniform()
  {
    std::random_device device;
    std::mt19937 generator(device());
    std::uniform_real_distribution distribution(0._r, 1._r);

    return distribution(generator);
  }

  /* placement_grid */

  void evolve_reverter::revert()
  {
    std::visit(
      core::lambda_wrapper {
        [=] (first_diff_type& __diff) {
          m_placement_grid.get().erase(__diff.first);
          __diff.first.get().position() = std::move(__diff.second);
          m_placement_grid.get().insert(__diff.first);
        },

        [=] (second_diff_type& __diff) {
          using std::swap;

          m_placement_grid.get().erase(__diff.first);
          m_placement_grid.get().erase(__diff.second);

          swap(__diff.first.get().position(), __diff.second.get().position());

          m_placement_grid.get().insert(__diff.first);
          m_placement_grid.get().insert(__diff.second);
        }
      },

      m_diff
    );
  }

  std::pair<placement_grid::iterator, placement_grid::iterator>
  placement_grid::rect(const par::cell::Cell& __cell)
  {
    std::pair<iterator, iterator> ret;

    boost::contract::check c =
      boost::contract::public_function(this)

        .postcondition(
          [&] { BOOST_CONTRACT_ASSERT(ret.second >= ret.first); }
        );

    ret = std::make_pair(top_left(__cell), bottom_right(__cell));

    return ret;
  }

  std::pair<placement_grid::const_iterator, placement_grid::const_iterator>
  placement_grid::rect(const par::cell::Cell& __cell) const
  {
    std::pair<const_iterator, const_iterator> ret;

    boost::contract::check c =
      boost::contract::public_function(this)

        .postcondition(
          [&] { BOOST_CONTRACT_ASSERT(ret.second >= ret.first); }
        );

    ret = std::make_pair(top_left(__cell), bottom_right(__cell));

    return ret;
  }

  std::pair<placement_grid::iterator, placement_grid::iterator>
  placement_grid::insert(cell::Cell& __cell)
  {
    auto ret = rect(__cell);

    for (auto i = ret.first; i != ret.second; ++i) {
      i->insert(__cell);
    }

    return ret;
  }

  std::pair<placement_grid::iterator, placement_grid::iterator>
  placement_grid::erase(cell::Cell& __cell)
  {
    auto ret = rect(__cell);

    for (auto i = ret.first; i != ret.second; ++i) {
      i->erase(__cell);
    }

    return ret;
  }
}
