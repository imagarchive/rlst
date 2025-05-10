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

#include "core/grid.hpp"
#include "core.hpp"

#include "par/cell/ports.hpp"

#include <forward_list>
#include <numeric>

/**
 * This namespace describes all utilities related to Place And Route
 */

namespace rlst::par
{
  namespace details {}

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

  using real_t = double; ///< A real number

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
   * Generate a random number in the range [0, 1]
   *
   * @return A random number in the range [0, 1]
   */

  real_t uniform();

  /* placement */

  class placement_grid
    : public core::grid<
      std::forward_list<std::reference_wrapper<cell::Cell>>,
      1
    >
  {
  private:
    using base_type =
      core::grid<
        std::forward_list<std::reference_wrapper<cell::Cell>>,
        1
      >;
  public:
    /// The default constructor
    placement_grid() = default;

    /**
     * Copy constructor
     *
     * @param[in] __other The placement grid to copy from
     */

    placement_grid(const placement_grid& __other) = default;

    /**
     * Move constructor
     *
     * @param[in, out] __other The placement grid to move from
     */

    placement_grid(placement_grid&& __other) = default;

    /// The destructor
    ~placement_grid() = default;
  public:
    /**
     * Constructs a placement grid with its parameters
     *
     * @param[in] __width The width of the grid
     * @param[in] __height The height of the grid
     */

    placement_grid(size_type __width, size_type __height)
      : base_type(__height, __width,  {})
    {}
  public:
    /**
     * Copy assignment operator
     *
     * @param[in] __rhs The placement grid to copy from
     * @return A reference to the updated placement grid
     */

    placement_grid& operator=(const placement_grid& __rhs) = default;

    /**
     * Move assignment operator
     *
     * @param[in, out] __rhs The placement grid to move from
     * @return A reference to the updated placement grid
     */

    placement_grid& operator=(placement_grid&& __rhs) = default;
  public:
    /**
     * Insert a new @ref cell::Cell in the grid
     *
     * @param[in] __cell The @ref cell::Cell to insert
     * @return The _rectangle_ of this @ref cell::Cell
     */

    std::pair<iterator, iterator> insert(cell::Cell& __cell) override;

    /**
     * Erase a @ref cell::Cell from the grid
     *
     * @param[in] __cell The @ref cell::Cell to erase
     * @return The _rectangle_ of this @ref cell::Cell
     */

    std::pair<iterator, iterator> erase(cell::Cell& __cell) override;
  };

  /* acceptance */

  /**
   * The acceptance rate target
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  class acceptance_rate_target
  {
    RLST_ENFORCE_RULE_OF_FOUR(acceptance_rate_target);
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
    RLST_ENFORCE_RULE_OF_FOUR(acceptance_scale);
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

  class scaled_cost
  {
    RLST_ENFORCE_RULE_OF_FOUR(scaled_cost);
  public:
    /**
     * Constructs a scaled cost with its parameters
     *
     * @param[in] __acceptance_scale The acceptance scale
     */

    constexpr scaled_cost(acceptance_scale __acceptance_scale = {}) noexcept
      : m_acceptance_scale(std::move(__acceptance_scale))
    {}
  public:
    constexpr real_t operator()(
      iteration_t __i,
      real_t __acceptance_rate,
      real_t __acceptance_scale,
      real_t __cost
    ) const
    {
      return
        m_acceptance_scale(__i, __acceptance_rate, __acceptance_scale) *
        __cost;
    }
  private:
    acceptance_scale m_acceptance_scale;
  };

  /* temperature */

  /**
   * The default first temperature
   *
   * This value is chosen accordingly the TimberWolf 4.2 algorithm.
   */

  constexpr real_t default_first_temperature = 500._r;

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
    RLST_ENFORCE_RULE_OF_FOUR(temperature_schedule);
  public:
    /**
     * Constructs a temperature schedule with its parameter
     *
     * @param[in] __alpha The alpha parameter
     */

    constexpr temperature_schedule(real_t __alpha = .98_r) noexcept
      : m_alpha(__alpha)
    {}
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

  /* overlap */

  /**
   * The overlap grid
   *
   * This data structure aims to provide an easy way to compute the overlap
   * score.
   */

  class overlap_grid : public core::grid<std::int16_t, 8>
  {
  private:
    using base_type = core::grid<std::int16_t, 8>;
  public:
    /// The default constructor
    overlap_grid() = default;

    /**
     * Copy constructor
     *
     * @param[in] __other The overlap grid to copy from
     */

    overlap_grid(const overlap_grid& __other) = default;

    /**
     * Move constructor
     *
     * @param[in, out] __other The overlap grid to move from
     */

    overlap_grid(overlap_grid&& __other) = default;

    /// The destructor
    ~overlap_grid() = default;
  public:
    /**
     * Constructs an overlap grid with its parameters
     *
     * @param[in] __width The width of the grid
     * @param[in] __height The height of the grid
     */

    overlap_grid(size_type __width, size_type __height)
      : base_type(__height, __width,  0)
    {}
  public:
    /**
     * Copy assignment operator
     *
     * @param[in] __rhs The overlap grid to copy from
     * @return A reference to the updated overlap grid
     */

    overlap_grid& operator=(const overlap_grid& __rhs) = default;

    /**
     * Move assignment operator
     *
     * @param[in, out] __rhs The overlap grid to move from
     * @return A reference to the updated overlap grid
     */

    overlap_grid& operator=(overlap_grid&& __rhs) noexcept = default;
  private:
    template <bool is_inserting>
    std::pair<iterator, iterator> insert(const cell::Cell& __cell);
  public:
    /**
     * Insert a new @ref cell::Cell in the grid
     *
     * @param[in] The @ref cell::Cell to insert
     * @return The _rectangle_ of this @ref cell::Cell
     *
     * @see rect()
     */

    std::pair<iterator, iterator> insert(cell::Cell& __cell) override
      { return insert<true>(__cell); }

    /**
     * Erase a @ref cell::Cell from the grid
     *
     * @param[in] The @ref cell::Cell to erase
     * @return The _rectangle_ of this @ref cell::Cell
     *
     * @see rect()
     */

    std::pair<iterator, iterator> erase(cell::Cell& __cell) override
      { return insert<false>(__cell); }
  public:
    /**
     * Get the overlap penalty
     *
     * @return The overlap penalty
     * @see https://doi.org/10.1145/103724.103725
     */

    value_type penalty() const noexcept
    {
      return
        static_cast<value_type>(
          std::reduce(
            cbegin(),
            cend(),
            0,

            [] (value_type __lhs, value_type __rhs)
            {
              return
                std::abs(__lhs - bin_size) +
                std::abs(__rhs - bin_size);
            }
          )
        );
    }
  };

  /**
   * The overlap penalty target
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  class overlap_penalty_target
  {
    RLST_ENFORCE_RULE_OF_FOUR(overlap_penalty_target);
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
    RLST_ENFORCE_RULE_OF_FOUR(overlap_penalty_weight);
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
      real_t __overlap_penalty,
      iteration_t __i
    ) const
    {
      return
        std::max(
          0._r,

          __overlap_penalty_weight +

          (__overlap_penalty - m_overlap_penalty_target(__i)) /
          m_desired_row_length
        );
    }
  private:
    overlap_penalty_target m_overlap_penalty_target;
    real_t m_desired_row_length;
  };

  /**
   * The overlap penalty cost
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  class overlap_penalty_cost
  {
    RLST_ENFORCE_RULE_OF_FOUR(overlap_penalty_cost);
  public:
    /**
     * Constructs an overlap penalty cost with its parameters
     *
     * @param[in] __overlap_penalty_weight The overlap penalty weight
     */

    constexpr overlap_penalty_cost(
      overlap_penalty_weight __overlap_penalty_weight = {}
    ) noexcept
      : m_overlap_penalty_weight(std::move(__overlap_penalty_weight))
    {}
  public:
    constexpr real_t operator()(
      real_t __overlap_penalty_weight,
      real_t __overlap_penalty,
      iteration_t __i
    ) const
    {
      return
        m_overlap_penalty_weight(
          __overlap_penalty_weight,
          __overlap_penalty,
          __i
        ) * __overlap_penalty;
    }
  private:
    overlap_penalty_weight m_overlap_penalty_weight;
  };

  /* row length */

  /**
   * The row length control penalty target
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  class row_length_penalty_target
  {
    RLST_ENFORCE_RULE_OF_FOUR(row_length_penalty_target);
  public:
    /**
     * Constructs a row length penalty target with its parameters
     *
     * @param[in] __alpha The alpha parameter
     * @param[in] __beta The beta parameter
     * @param[in] __desired_row_length The desired row length
     * @param[in] __max_iterations The maximum number of iterations
     */

    constexpr row_length_penalty_target(
      real_t __alpha = 5._r,
      real_t __beta = 4._r,
      real_t __desired_row_length = 0._r,
      iteration_t __max_iterations = 120_it,
      real_t __xi = 0._r
    ) noexcept
      : m_alpha(__alpha)
      , m_beta(__beta)
      , m_desired_row_length(__desired_row_length)
      , m_max_iterations(__max_iterations)
      , m_xi(__xi)
    {}
  public:
    /**
     * Get the next row length penalty target
     *
     * @param[in] __i The current iteration
     * @return The next row length penalty target
     */

    constexpr real_t operator()(iteration_t __i) const
    {
      return
        m_alpha -

        m_beta * (
          static_cast<real_t>(__i) / static_cast<real_t>(m_max_iterations)
        ) * m_xi * m_desired_row_length;
    }
  private:
    real_t m_alpha;
    real_t m_beta;
    real_t m_desired_row_length;
    iteration_t m_max_iterations;
    real_t m_xi;
  };

  /**
   * The row length control weight
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  class row_length_weight
  {
    RLST_ENFORCE_RULE_OF_FOUR(row_length_weight);
  public:
    /**
     * Constructs a row length weight with its parameters
     *
     * @param[in] __row_length_penalty_target The row length penalty target
     */

    constexpr row_length_weight(
      row_length_penalty_target __row_length_penalty_target = {}
    ) noexcept
      : m_row_length_penalty_target(std::move(__row_length_penalty_target))
    {}
  public:
    /**
     * Get the next row length weight
     *
     * @param[in] __i The current iteration
     * @param[in] __row_length_weight The current row length weight
     * @param[in] __row_length_penalty The current row length penalty
     *
     * @return The next row length weight
     */

    constexpr real_t operator()(
      iteration_t __i,
      real_t __row_length_weight,
      real_t __row_length_penalty
    ) const
    {
      real_t rlp_target = m_row_length_penalty_target(__i);

      return
        std::max(
          0._r,
          __row_length_weight + (__row_length_penalty - rlp_target) / rlp_target
        );
    }
  private:
    row_length_penalty_target m_row_length_penalty_target;
  };

  /**
   * The row length cost
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  class row_length_cost
  {
    RLST_ENFORCE_RULE_OF_FOUR(row_length_cost);
  public:
    /**
     * Constructs a row length cost with its parameters
     *
     * @param[in] __row_length_weight The row length weight
     */

    constexpr row_length_cost(row_length_weight __row_length_weight = {})
      : m_row_length_weight(std::move(__row_length_weight))
    {}
  public:
    /**
     * Get the next row length cost
     *
     * @param[in] __i The current iteration
     * @param[in] __row_length_weight The current row length weight
     * @param[in] __row_length_penalty The current row length penalty
     *
     * @return The next row length cost
     */

    constexpr real_t operator()(
      iteration_t __i,
      real_t __row_length_weight,
      real_t __row_length_penalty
    ) const
    {
      return
        m_row_length_weight(__i, __row_length_weight, __row_length_penalty) *
        __row_length_penalty;
    }
  private:
    row_length_weight m_row_length_weight;
  };

  /**
   * Get the row length penalty
   *
   * @tparam The iterator type
   *
   * @param[in] __begin The begin iterator of the @ref cell::Cell "cells"
   * @param[in] __end The past-the-last iterator of the @ref cell::Cell "cells"
   *
   * @return The row length penalty
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  template <class InputIt>
  real_t row_length_penalty(InputIt __begin, InputIt __end);

  /* wire length */

  namespace details
  {
    struct wire_length_cost_binop
    {
      constexpr real_t operator()(real_t __lhs, real_t __rhs) const noexcept
        { return __lhs + __rhs; }

      real_t operator()(
        const std::pair<cell::Port, cell::Port>& __lhs,
        real_t __rhs
      ) const noexcept
      {
        return
          operator()(
            static_cast<real_t>(
              __lhs.first.position.manhattan(__lhs.second.position)
            ),

            __rhs
          );
      }

      real_t operator()(
        real_t __lhs,
        const std::pair<cell::Port, cell::Port>& __rhs
      ) const noexcept
        { return operator()(__rhs, __lhs); }

      real_t operator()(
        const std::pair<cell::Port, cell::Port>& __lhs,
        const std::pair<cell::Port, cell::Port>& __rhs
      ) const noexcept
      {
        return
          operator()(
            static_cast<real_t>(
              __lhs.first.position.manhattan(__lhs.second.position)
            ),

            static_cast<real_t>(
              __rhs.first.position.manhattan(__rhs.second.position)
            )
          );
      }
    };
  }

  /**
   * Compute the wire length cost of nets
   *
   * @tparam InputIt The iterator type
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
        std::move(__begin),
        std::move(__end),
        0._r,
        details::wire_length_cost_binop {}
      );
  }

  /* evolutor */

  class evolutor
  {
  public:
    /// The size type
    using size_type =
      std::common_type_t<
        placement_grid::size_type,
        overlap_grid::size_type
      >;
  public:
    /// The default constructor
    evolutor() = default;

    /**
     * Copy constructor
     *
     * @param[in] __other The evolutor to copy from
     */

    evolutor(const evolutor& __other) = default;

    /**
     * Move constructor
     *
     * @param[in, out] __other The evolutor to move from
     */

    evolutor(evolutor&& __other) = default;

    /// The destructor
    ~evolutor() = default;
  public:
    /**
     * Copy assignment operator
     *
     * @param[in] __rhs The evolutor to copy from
     * @return A reference to the updated evolutor
     */

    evolutor& operator=(const evolutor& __rhs) = default;

    /**
     * Move assignment operator
     *
     * @param[in, out] __rhs The evolutor to move from
     * @return A reference to the updated evolutor
     */

    evolutor& operator=(evolutor&& __rhs) = default;
  public:
    /**
     * Constructs an evolutor with its parameters
     *
     * @param[in] __width The width of the grid
     * @param[in] __height The height of the grid
     */

    evolutor(size_type __width, size_type __height)
      : m_cells()
      , m_overlap_grid(__width, __height)
      , m_placement_grid(__width, __height)
    {}
  public:
    /**
     * Insert a new @ref cell::Cell in the grids
     *
     * @tparam U The type of the @ref cell::Cell (enable perfect forwarding)
     * @param[in] __cell The @ref cell::Cell to insert
     */

    template <class U>
    void insert(U&& __cell);
  private:
    std::vector<cell::Cell> m_cells;
    overlap_grid m_overlap_grid;
    placement_grid m_placement_grid;
  };

  /**
   * Get a random neighbor of a @ref cell::Cell
   *
   * @param[in] __cell The @ref cell::Cell to get the neighbor from
   * @return A random neighbor of the @ref cell::Cell
   */

  Point random_neighbor(const cell::Cell& __cell);
}

#include "placement.ipp"

#endif // RLST_RLST_PAR_PLACEMENT_HPP
