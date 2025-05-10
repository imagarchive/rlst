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
    template <bool is_accepted>
    real_t update_rho(real_t __rho, iteration_t __n)
    {
      __rho *= __n;

      if constexpr (is_accepted) {
        ++__rho;
      }

      __rho /= __n + 1;

      return __rho;
    }
  }

  template <class InputIt, class OutputIt>
  void place(
    OutputIt __begin_cell,
    OutputIt __end_cell,
    InputIt __begin_net,
    InputIt __end_net
  )
  {
    iteration_t i = 1_it;
    real_t t = default_first_temperature;

    evolutor config;

    for (auto i = __begin_cell; i != __end_cell; ++i) {
      config.insert(*i);
    }

    temperature_schedule reduce;

    real_t c_1 = std::numeric_limits<real_t>::max();

    real_t w_2 = 0._r, c_2 = std::numeric_limits<real_t>::max();
    overlap_penalty_weight compute_w_2(.1_r);
    overlap_penalty_cost compute_c_2;

    real_t w_3 = 0._r, c_3 = std::numeric_limits<real_t>::max();
    row_length_weight compute_w_3;
    row_length_cost compute_c_3;

    real_t rho = .5_r, s = 1._r, c_prime = std::numeric_limits<real_t>::max();
    acceptance_scale compute_s;
    scaled_cost compute_c_prime;

    iteration_t n = 0_it;

    while ((i != 120_it) || (c_2 != 0._r)) {
      for (auto j = 0_it; j != 100_it; ++j) {
        evolutor new_config = config;
        new_config.evolute();

        real_t new_w_2 = compute_w_2(w_2, config.overlap_penalty(), i);
        real_t new_c_2 = compute_c_2(new_w_2, config.overlap_penalty());

        real_t p_r = row_length_penalty(__begin_cell, __end_cell);
        real_t new_w_3 = compute_w_3(i, w_3, p_r);
        real_t new_c_3 = compute_c_3(new_w_3, p_r);

        real_t new_s = compute_s(i, rho, s);
        real_t new_c_prime = compute_c_prime(new_s, new_c_2 + new_c_3);

        real_t delta_c = new_c_prime - c_prime;

        if (delta_c < 0._r) {
          real_t new_c_1 =
            compute_c_prime(
              new_s,
              wire_length_cost(__begin_net, __end_net)
            );

          delta_c += new_c_1 - c_1;

          if ((delta_c < 0._r) || (accept(delta_c, t) > uniform())) {
            c_1 = new_c_1;

            w_2 = new_w_2;
            c_2 = new_c_2;

            w_3 = new_w_3;
            c_3 = new_c_3;

            s = new_s;
            c_prime = new_c_prime;

            rho = details::update_rho<true>(rho, n);
          }
        } else {
          rho = details::update_rho<false>(rho, n);
        }

        ++n;
      }

      t = reduce(t);
      ++i;
    }
  }

  template <bool is_inserting>
  std::pair<overlap_grid::iterator, overlap_grid::iterator>
  overlap_grid::insert(const cell::Cell& __cell)
  {
    auto ret = rect(__cell);

    for (auto i = ret.first; i != ret.second; ++i) {
      value_type v =
        static_cast<value_type>(
          std::min(
            __cell.position.x() +
              static_cast<literal_t>(__cell.type->size.width),

            (i.x() + 1) * bin_size
          ) -

          std::max(__cell.position.x(), i.x() * bin_size)
        );

      if constexpr (is_inserting) {
        *i += v;
      } else {
        *i -= v;
      }
    }

    return ret;
  }

  namespace details
  {
    struct row_length_penalty_binop
    {
      constexpr real_t operator()(real_t __lhs, real_t __rhs) const noexcept
        { return __lhs + __rhs; }

      constexpr real_t operator()(
        const std::pair<literal_t, std::pair<literal_t, literal_t>>& __lhs,
        real_t __rhs
      ) const noexcept
      {
        return
          operator()(
            static_cast<real_t>(__lhs.second.second - __lhs.second.first),
            __rhs
          );
      }

      constexpr real_t operator()(
        real_t __lhs,
        const std::pair<literal_t, std::pair<literal_t, literal_t>>& __rhs
      ) const noexcept
        { return operator()(__rhs, __lhs); }

      constexpr real_t operator()(
        const std::pair<literal_t, std::pair<literal_t, literal_t>>& __lhs,
        const std::pair<literal_t, std::pair<literal_t, literal_t>>& __rhs
      ) const noexcept
      {
        return
          operator()(
            static_cast<real_t>(__lhs.second.second - __lhs.second.first),
            __rhs
          );
      }
    };
  }

  template <class InputIt>
  real_t row_length_penalty(InputIt __begin, InputIt __end)
  {
    std::unordered_map<
      literal_t,
      std::pair<literal_t, literal_t>
    > row_to_min_max;

    if constexpr (core::is_random_access_iterator_v<InputIt>) {
      row_to_min_max.reserve(__end - __begin);
    }

    for (; __begin != __end; ++__begin) {
      auto i =
        row_to_min_max.insert(
          std::make_pair(
            __begin->position.y(),

            std::make_pair(
              std::numeric_limits<literal_t>::max(),
              std::numeric_limits<literal_t>::min()
            )
          )
        ).first;

      i->second =
        std::make_pair(
          std::min(i->second.first, __begin->position.x()),

          std::max(
            i->second.second,

            __begin->position.x() +
              static_cast<literal_t>(__begin->type->size.width)
          )
        );
    }

  return
    std::reduce(
      row_to_min_max.cbegin(),
      row_to_min_max.cend(),
      0._r,
      details::row_length_penalty_binop {}
    ) / static_cast<real_t>(row_to_min_max.size());
  }

  /* evolutor */

}
