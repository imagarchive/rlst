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

#include "par/cell/ports.hpp"
#include <numeric>

/**
 * This namespace describes all utilities related to Place And Route
 */

namespace rlst::par
{
  /* iteration_t */

  using iteration_t = std::uint8_t; ///< The type of the number of iterations

  /**
   * User-defined literal for @ref rlst::par::iteration_t
   *
   * @param[in] __input The input value
   * @return The input value as a @ref rlst::par::iteration_t
   */

  constexpr iteration_t operator ""_it(unsigned long long __input) noexcept
    { return static_cast<iteration_t>(__input); }

  /* real_t */

  using real_t = long double; ///< A real number

  /**
   * User-defined literal for @ref rlst::par::real_t
   *
   * @param[in] __input The input value
   * @return The input value as a @ref rlst::par::real_t
   */

  constexpr real_t operator ""_r(long double __input) noexcept
    { return static_cast<real_t>(__input); }

  /* acceptance_parameters */

  /**
   * The acceptance parameters
   *
   * It is used to define the acceptance rate and the acceptance scale
   */

  struct acceptance_parameters
  {
    real_t alpha; ///< The acceptance rate
    real_t beta; ///< The acceptance scale deviation
  };

  /**
   * The default acceptance parameters
   *
   * It is used to define the acceptance rate and the acceptance scale
   */

  constexpr acceptance_parameters default_acceptance_parameters =
    { 50._r, 40._r };

  /* overlap_parameters */

  /**
   * The overlap parameters
   */

  struct overlap_parameters
  {
    real_t alpha;
    real_t beta;
  };

  constexpr overlap_parameters default_overlap_parameters = { 1.4_r, 1.15_r };

  /* schedule_parameters */

  /**
   * The temperature schedule parameters
   *
   * It is used to define the initial temperature, the cooling rate and the
   * number of iterations.
   */

  struct schedule_parameters
  {
    real_t t_0; ///< The initial temperature
    real_t alpha; ///< The cooling rate
  };

  /**
   * The default temperature schedule parameters
   */

  constexpr schedule_parameters default_schedule_parameters = { 500._r, .98_r };

  /* parameters */

  /**
   * The parameters of the simulated annealing algorithm
   */

  struct parameters
  {
    schedule_parameters schedule; ///< The temperature schedule parameters
    acceptance_parameters acceptance; ///< The acceptance parameters
    overlap_parameters overlap; ///< The overlap parameters
    iteration_t max_iterations; ///< The maximum number of iterations
  };

  /**
   * The default parameters of the simulated annealing algorithm
   */

  constexpr parameters default_parameters =
    {
      default_schedule_parameters,
      default_acceptance_parameters,
      default_overlap_parameters,
      120_it
    };

  /* functions */

  /**
   * The probabilistic acceptance function
   *
   * It is called when the cost is increased by a perturbation. It determines
   * whether to accept a move or not, depending on the cost increase and the
   * temperature.
   *
   * Usually it is the exponential function, but it can be any other function.
   *
   * @param[in] __delta_c The cost increase
   * @param[in] __t The temperature
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  inline real_t accept(real_t __delta_c, real_t __t)
    { return std::exp(-__delta_c / __t); }

  /**
   * The acceptance rate target
   *
   * @param[in] __i The current iteration
   * @param[in] __params The parameters of the simulated annealing algorithm
   *
   * @return The acceptance rate target
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  constexpr real_t acceptance_rate_target(
    iteration_t __i,
    const parameters& __params = default_parameters
  )
  {
    return
      __params.acceptance.alpha *

      (
        1 -

        (
          static_cast<real_t>(__i) /
          static_cast<real_t>(__params.max_iterations)
        )
      );
  }

  /**
   * Get the next acceptance scale
   *
   * Due to the wide variety of the circuits to be placed, a fixed temperature
   * schedule does not always produce an appropriate value of the rate of
   * acceptance of new configurations. It was observed that the ideal acceptance
   * rate was 50% in the beginning (@f$ i = 0 @f$) and was reduced to zero at
   * low temperatures (@f$i = i_{max}@f$). To achieve this acceptance
   * rate profile, negative feedback control has been provided.
   *
   * @param[in] __i The current iteration
   * @param[in] __acceptance_rate The current acceptance rate
   * @param[in] __acceptance_scale The current acceptance scale
   * @param[in] __params The parameters of the simulated annealing algorithm
   *
   * @return The new acceptance scale
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  constexpr real_t acceptance_scale(
    iteration_t __i,
    real_t __acceptance_rate,
    real_t __acceptance_scale,
    const parameters& __params = default_parameters
  )
  {
    return
      __acceptance_scale *

      (
        1 +

        (__acceptance_rate - acceptance_rate_target(__i, __params)) /
        __params.acceptance.beta
      );
  }

  /**
   * The temperature schedule
   *
   * It gives the next temperature as a function of the number of iterations or
   * the previous temperature.
   *
   * @param[in] __t The current temperature
   * @param[in] __params The schedule parameters
   *
   * @return The next temperature
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  constexpr real_t schedule(
    real_t __t,
    const schedule_parameters& __params = default_schedule_parameters
  ) noexcept
    { return __params.alpha * __t; }

  // TODO: comment

  constexpr real_t overlap_penalty_target(
    iteration_t __i,
    literal_t __desired_wire_length,
    const parameters& __params = default_parameters
  )
  {
    return
      (
        __params.overlap.alpha -

        __params.overlap.beta * (

          static_cast<real_t>(__i) /
          static_cast<real_t>(__params.max_iterations)
        )
      ) * __desired_wire_length;
  }

  /**
   * Generate a random number in the range [0, 1]
   *
   * @return A random number in the range [0, 1]
   */

  real_t uniform();

  /**
   * Compute the wire length cost of nets
   *
   * @param[in] __begin The begin iterator of the nets
   * @param[in] __end The end iterator of the nets
   *
   * @return The wire length cost of the nets
   */

  template <class InputIt>
  constexpr real_t wire_length_cost(InputIt __begin, InputIt __end)
  {
    return
      std::reduce(
        __begin,
        __end,
        0._r,

        [] (
          real_t __acc,
          const std::pair<cell::PlacedPort, cell::PlacedPort>& __net
        )
        {
          return
            __acc +
            __net.first.port.position.manhattan(__net.second.port.position);
        }
      );
  }
}
