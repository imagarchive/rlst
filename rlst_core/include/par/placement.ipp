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
  template <class InputIt, class OutputIt>
  void place(
    OutputIt __begin_cell,
    OutputIt __end_cell,
    InputIt __begin_net,
    InputIt __end_net,
    uliteral_t __desired_row_length
  )
  {
    Point tl(0, 0, 0);
    Point tr(0, 0, 0);
    uliteral_t max_height = 0_ul;

    for (auto i = __begin_cell; i != __end_cell; ++i) {
      tr.x() += i->type()->size.width;

      if (tr.x() >= static_cast<literal_t>(__desired_row_length)) {
        tl.x() = 0;
        tl.y() += max_height;

        tr.x() = i->type()->size.width;
        tr.y() += max_height;
      }

      i->position() = tl;
      tl.x() += i->type()->size.width;
      max_height = std::max(max_height, i->type()->size.height);
    }
  }

  template <class InputIt>
  real_t row_length_penalty(InputIt __begin, InputIt __end)
  {
    using row_to_min_max_type =
      std::unordered_map<
        literal_t,
        std::pair<literal_t, literal_t>
      >;

    row_to_min_max_type row_to_min_max;

    if constexpr (core::is_random_access_iterator_v<InputIt>) {
      row_to_min_max.reserve(__end - __begin);
    }

    for (; __begin != __end; ++__begin) {
      auto i =
        row_to_min_max.insert(
          std::make_pair(
            __begin->position().y(),

            std::make_pair(
              std::numeric_limits<literal_t>::max(),
              std::numeric_limits<literal_t>::min()
            )
          )
        ).first;

      i->second =
        std::make_pair(
          std::min(i->second.first, __begin->position().x()),

          std::max(
            i->second.second,

            __begin->position().x() +
              static_cast<literal_t>(__begin->type()->size.width)
          )
        );
    }

  return
    std::reduce(
      row_to_min_max.cbegin(),
      row_to_min_max.cend(),
      0._r,

      core::transformed_binop(
        [] (const row_to_min_max_type::value_type& __p) {
          return static_cast<real_t>(__p.second.second - __p.second.first);
        }
      )
    ) / static_cast<real_t>(row_to_min_max.size());
  }
}
