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


#ifndef RLST_RLST_PAR_PLACEMENT_HPP
#  define RLST_RLST_PAR_PLACEMENT_HPP

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
   * @see https://doi.org/10.1145/103724.103725
   */

  class acceptance_rate_target
  {
  public:
    /**
     * Constructs an acceptance rate target with its parameters
     */

    constexpr acceptance_rate_target(
      real_t __alpha = 50._r,
      iteration_t __max_iterations = 120_it
    ) noexcept
      : m_alpha(__alpha)
      , m_max_iterations(__max_iterations)
    {}

    /**
     * Copy constructor
     *
     * @param[in] __other The other acceptance rate target
     */

    constexpr acceptance_rate_target(
      const acceptance_rate_target& __other
    ) noexcept = default;

    /**
     * Move constructor
     *
     * @param[in, out] __other The other acceptance rate target
     */

    constexpr acceptance_rate_target(
      acceptance_rate_target&& __other
    ) noexcept = default;

    ~acceptance_rate_target() noexcept = default; ///< Destructor
  public:
    /**
     * Copy assignment operator
     *
     * @param[in] __other The other acceptance rate target
     * @return A reference to this acceptance rate target
     */

    constexpr acceptance_rate_target& operator=(
      const acceptance_rate_target& __other
    ) noexcept = default;

    /**
     * Move assignment operator
     *
     * @param[in, out] __other The other acceptance rate target
     * @return A reference to this acceptance rate target
     */

    constexpr acceptance_rate_target& operator=(
      acceptance_rate_target&& __other
    ) noexcept = default;
  public:
    /**
     * The acceptance rate target
     *
     * @param[in] __i The current iteration
     * @return The acceptance rate target
     */

    constexpr real_t operator()(iteration_t __i) const
    {
      return
        m_alpha *

        (
          1 -
          (static_cast<real_t>(__i) / static_cast<real_t>(m_max_iterations))
        );
    }
  private:
    real_t m_alpha;
    iteration_t m_max_iterations;
  };

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
   * @see https://doi.org/10.1145/103724.103725
   */

  class acceptance_scale
  {
  public:
    /**
     * Constructs an acceptance scale with its parameters
     *
     * @param[in] __acceptance_rate_target The acceptance rate target
     * @param[in] __beta The beta parameter
     */

    constexpr acceptance_scale(
      acceptance_rate_target __acceptance_rate_target = {},
      real_t __alpha = 50._r
    ) noexcept
      : m_acceptance_rate_target(std::move(__acceptance_rate_target))
      , m_alpha(__alpha)
    {}

    /**
     * Copy constructor
     *
     * @param[in] __other The other acceptance scale
     */

    constexpr acceptance_scale(
      const acceptance_scale& __other
    ) noexcept = default;

    /**
     * Move constructor
     *
     * @param[in, out] __other The other acceptance scale
     */

    constexpr acceptance_scale(
      acceptance_scale&& __other
    ) noexcept = default;

    ~acceptance_scale() noexcept = default; ///< Destructor
  public:
    /**
     * Copy assignment operator
     *
     * @param[in] __other The other acceptance scale
     * @return A reference to this acceptance scale
     */

    constexpr acceptance_scale& operator=(
      const acceptance_scale& __other
    ) noexcept = default;

    /**
     * Move assignment operator
     *
     * @param[in, out] __other The other acceptance scale
     * @return A reference to this acceptance scale
     */

    constexpr acceptance_scale& operator=(
      acceptance_scale&& __other
    ) noexcept = default;
  public:
    /**
     * Get the next acceptance scale
     *
     * @param[in] __i The current iteration
     * @param[in] __acceptance_rate The current acceptance rate
     * @param[in] __acceptance_scale The current acceptance scale
     *
     * @return The new acceptance scale
     */

    constexpr real_t operator()(
      iteration_t __i,
      real_t __acceptance_rate,
      real_t __acceptance_scale
    ) const
    {
      return
        __acceptance_scale *

        (
          1 +

          (__acceptance_rate - m_acceptance_rate_target(__i)) /
          m_alpha
        );
    }
  private:
    acceptance_rate_target m_acceptance_rate_target;
    real_t m_alpha;
  };

  /**
   * The temperature schedule
   *
   * It gives the next temperature as a function of the number of iterations or
   * the previous temperature.
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  class temperature_schedule
  {
  public:
    /**
     * Constructs a temperature schedule with its parameter
     *
     * @param[in] __alpha The alpha parameter
     */

    constexpr temperature_schedule(real_t __alpha = .98_r) noexcept
      : m_alpha(__alpha)
    {}

    /**
     * Copy constructor
     *
     * @param[in] __other The other temperature schedule
     */

    constexpr temperature_schedule(
      const temperature_schedule& __other
    ) noexcept = default;

    /**
     * Move constructor
     *
     * @param[in, out] __other The other temperature schedule
     */

    constexpr temperature_schedule(
      temperature_schedule&& __other
    ) noexcept = default;

    ~temperature_schedule() noexcept = default; ///< Destructor
  public:
    /**
     * Copy assignment operator
     *
     * @param[in] __other The other temperature schedule
     * @return A reference to this temperature schedule
     */

    constexpr temperature_schedule& operator=(
      const temperature_schedule& __other
    ) noexcept = default;

    /**
     * Move assignment operator
     *
     * @param[in, out] __other The other temperature schedule
     * @return A reference to this temperature schedule
     */

    constexpr temperature_schedule& operator=(
      temperature_schedule&& __other
    ) noexcept = default;
  public:
    /**
     * Get the next temperature
     *
     * @param[in] __t The current temperature
     * @return The next temperature
     */

    constexpr real_t operator()(real_t __t) const noexcept
      { return m_alpha * __t; }
  private:
    real_t m_alpha;
  };

  /**
   * The overlap penalty target
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  class overlap_penalty_target
  {
  public:
    /**
     * Constructs an overlap penalty target with its parameters
     */

    constexpr overlap_penalty_target(
      real_t __alpha = 1.4_r,
      real_t __beta = 0.15_r,
      real_t __desired_row_length = 0._r,
      iteration_t __max_iterations = 120_it
    ) noexcept
      : m_alpha(__alpha)
      , m_beta(__beta)
      , m_desired_row_length(__desired_row_length)
      , m_max_iterations(__max_iterations)
    {}

    /**
     * Copy constructor
     *
     * @param[in] __other The other overlap penalty target
     */

    constexpr overlap_penalty_target(
      const overlap_penalty_target& __other
    ) noexcept = default;

    /**
     * Move constructor
     *
     * @param[in, out] __other The other overlap penalty target
     */

    constexpr overlap_penalty_target(
      overlap_penalty_target&& __other
    ) noexcept = default;

    ~overlap_penalty_target() noexcept = default; ///< Destructor
  public:
    /**
     * Copy assignment operator
     *
     * @param[in] __other The other overlap penalty target
     * @return A reference to this overlap penalty target
     */

    constexpr overlap_penalty_target& operator=(
      const overlap_penalty_target& __other
    ) noexcept = default;

    /**
     * Move assignment operator
     *
     * @param[in, out] __other The other overlap penalty target
     * @return A reference to this overlap penalty target
     */

    constexpr overlap_penalty_target& operator=(
      overlap_penalty_target&& __other
    ) noexcept = default;
  public:
    /**
     * Get the next overlap penalty target
     *
     * @param[in] __i The current iteration
     * @return The next overlap penalty target
     */

    constexpr real_t operator()(iteration_t __i) const
    {
      return
        (
          m_alpha -

          m_beta * (
            static_cast<real_t>(__i) /
            static_cast<real_t>(m_max_iterations)
          )
        ) * m_desired_row_length;
    }
  private:
    real_t m_alpha;
    real_t m_beta;
    real_t m_desired_row_length;
    iteration_t m_max_iterations;
  };

  /**
   * The overlap penalty weight
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  class overlap_penalty_weight
  {
  public:
    /**
     * Constructs an overlap penalty weight with its parameters
     *
     * @param[in] __overlap_penalty_target The overlap penalty target
     * @param[in] __desired_row_length The desired row length
     */

    constexpr overlap_penalty_weight(
      overlap_penalty_target __overlap_penalty_target = {},
      real_t __desired_row_length = 0._r
    ) noexcept
      : m_overlap_penalty_target(std::move(__overlap_penalty_target))
      , m_desired_row_length(__desired_row_length)
    {}

    /**
     * Copy constructor
     *
     * @param[in] __other The other overlap penalty weight
     */

    constexpr overlap_penalty_weight(
      const overlap_penalty_weight& __other
    ) noexcept = default;

    /**
     * Move constructor
     *
     * @param[in, out] __other The other overlap penalty weight
     */

    constexpr overlap_penalty_weight(
      overlap_penalty_weight&& __other
    ) noexcept = default;

    ~overlap_penalty_weight() noexcept = default; ///< Destructor
  public:
    /**
     * Copy assignment operator
     *
     * @param[in] __other The other overlap penalty weight
     * @return A reference to this overlap penalty weight
     */

    constexpr overlap_penalty_weight& operator=(
      const overlap_penalty_weight& __other
    ) noexcept = default;

    /**
     * Move assignment operator
     *
     * @param[in, out] __other The other overlap penalty weight
     * @return A reference to this overlap penalty weight
     */

    constexpr overlap_penalty_weight& operator=(
      overlap_penalty_weight&& __other
    ) noexcept = default;
  public:
    /**
     * Gets the next overlap penalty weight
     *
     * @param[in] __overlap_penalty_weight The current overlap penalty weight
     * @param[in] __i The current iteration
     *
     * @return The next overlap penalty weight
     */

    constexpr real_t operator()(
      real_t __overlap_penalty_weight,
      iteration_t __i
    ) const
    {
      // TODO: implement the overlap penalty functor

      return
        std::max(
          0._r,

          __overlap_penalty_weight +
          (0._r - m_overlap_penalty_target(__i)) / m_desired_row_length
        );
    }
  private:
    overlap_penalty_target m_overlap_penalty_target;
    real_t m_desired_row_length;
  };

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

#endif // RLST_RLST_PAR_PLACEMENT_HPP
