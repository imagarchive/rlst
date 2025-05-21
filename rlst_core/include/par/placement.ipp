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

    Size frame = frame_of(__begin_cell, __end_cell);
    frame.width = std::max(frame.width, __desired_row_length);
    frame *= 2_ul;

    BOOST_LOG_TRIVIAL(debug)
      << "`place()`: frame = { width = "
      << frame.width
      << ", height = "
      << frame.height
      << " }";

    real_t c_20;

    do {
      BOOST_LOG_TRIVIAL(trace) << "`place()`: setting initial configuration";

      for (auto i = __begin_cell; i != __end_cell; ++i) {
        i->position() = {};
      }

      placement_grid config(frame, __begin_cell, __end_cell);
      row_length_cost compute_c_3(static_cast<real_t>(__desired_row_length));

      real_t t = default_first_temperature;

      c_20 = config.overlap_cost();
      BOOST_LOG_TRIVIAL(debug) << "overlap cost = " << c_20;

      real_t c =
        height_cost(__begin_cell, __end_cell) +
        wire_length_cost(__begin_net, __end_net) +
        config.emptiness_cost() +
        compute_c_3(__begin_cell, __end_cell);

      BOOST_LOG_TRIVIAL(trace) << "`place()`: initial configuration set";

      for (iteration_t i = 0_it; i != 140_it; ++i) {
        for (iteration_t j = 0_it; j != 32_it; ++j) {
          BOOST_LOG_TRIVIAL(trace)
            << "`place()`: computing ("
            << static_cast<int>(i)
            << ", "
            << static_cast<int>(j)
            << ')';

          evolve_reverter reverter = config.evolve();

          real_t new_c_20 = config.overlap_cost();
          real_t delta_c = new_c_20 - c_20;

          if (delta_c <= 0._r) {
            real_t new_c =
              height_cost(__begin_cell, __end_cell) +
              wire_length_cost(__begin_net, __end_net) +
              config.emptiness_cost() +
              compute_c_3(__begin_cell, __end_cell);

            delta_c += new_c - c;

            if ((delta_c <= 0._r) || (accept(delta_c, t) >= uniform())) {
              BOOST_LOG_TRIVIAL(trace)
                << "`place()`: ("
                << static_cast<int>(i)
                << ", "
                << static_cast<int>(j)
                << ") is accepted ("
                << ((delta_c <= 0._r) ? "ok" : "random")
                << ')';

              c_20 = new_c_20;
              c = new_c;

              continue;
            }
          }

          BOOST_LOG_TRIVIAL(trace)
            << "`place()`: ("
            << static_cast<int>(i)
            << ", "
            << static_cast<int>(j)
            << ") is rejected";

          reverter.revert();
        }

        t = reduce(t);
      }

      BOOST_LOG_TRIVIAL(debug) << "overlap cost = " << c_20;
    } while (c_20 != 0);
  }

  template <class InputIt>
  real_t height_penalty(InputIt __begin, InputIt __end)
  {
    literal_t top = std::numeric_limits<literal_t>::min();
    literal_t bottom = std::numeric_limits<literal_t>::max();

    for (; __begin != __end; ++__begin) {
      top = std::min(top, __begin->position().y());

      bottom =
          std::max(
            bottom,

            __begin->position().y() +
              static_cast<literal_t>(__begin->type()->size.height)
          );
    }

    return static_cast<real_t>(top + bottom);
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

  template <class InputIt>
  Size frame_of(InputIt __begin, InputIt __end)
  {
    Size ret { 0_ul, 0_ul };

    for (; __begin != __end; ++__begin) {
      ret.width =
        std::max(
          ret.width,
          __begin->position().x() + __begin->type()->size.width
        );

      ret.height =
        std::max(
          ret.height,
          __begin->position().y() + __begin->type()->size.height
        );
    }

    return ret;
  }
}
