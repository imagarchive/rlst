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
#include "par/geometry.hpp"

#include <numeric>
#include <unordered_set>
#include <utility>

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
   * Normalize a value to the range @f$ (0; 100) @f$
   *
   * This function takes a real number as input and maps it to a value within
   * the range @f$ (0; 100) @f$ using a specific normalization formula. The
   * normalization ensures that the output is bounded and scaled
   * appropriately.
   *
   * @param[in] __x The input value to normalize
   * @return The normalized value in the range @f$ (0; 100) @f$
   */

  inline real_t normalize(real_t __x)
  {
    return
      (100._r / M_PI) *
      std::copysign(1._r, __x) *
      std::atan(std::log(std::abs(__x) + 1._r)) + 50._r;
  }

  /**
   * Generate a random number in the range [0, 1]
   *
   * @return A random number in the range [0, 1]
   */

  real_t uniform();

  /**
   * Place a @ref cell::Cell "cells"
   *
   * @tparam InputIt The iterator type used for nets
   * @tparam OutputIt The iterator type used for @ref cell::Cell "cells"
   *
   * @param[in, out] __begin_cell The begin iterator of the
   * @ref cell::Cell "cells"
   *
   * @param[in, out] __end_cell The past-the-last iterator of the
   * @ref cell::Cell "cells"
   *
   * @param[in] __begin_net The begin iterator of the nets
   * @param[in] __end_net The past-the-last iterator of the nets
   * @param[in] __desired_row_length The desired row length
   */

  template <class InputIt, class OutputIt>
  void place(
    OutputIt __begin_cell,
    OutputIt __end_cell,
    InputIt __begin_net,
    InputIt __end_net,
    uliteral_t __desired_row_length
  );

  /* placement */

  namespace details
  {
    struct placement_grid_hasher
    {
      std::size_t operator()(std::reference_wrapper<cell::Cell> __cell) const
        { return std::hash<cell::Cell*>()(&__cell.get()); }
    };
  }

  class placement_grid
    : public core::grid<
      std::unordered_set<
        std::reference_wrapper<cell::Cell>,
        details::placement_grid_hasher
      >
    >
  {
  private:
    using base_type =
      core::grid<
        std::unordered_set<
          std::reference_wrapper<cell::Cell>,
          details::placement_grid_hasher
        >
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
     * Get the top left and bottom right iterator
     *
     * @param[in] __cell The given @ref par::cell::Cell
     * @return The top left and bottom right iterator
     */

    std::pair<iterator, iterator> rect(const par::cell::Cell& __cell);

    /**
     * Get the top left and bottom right iterator
     *
     * @param[in] __cell The given @ref par::cell::Cell
     * @return The top left and bottom right iterator
     */

    std::pair<const_iterator, const_iterator>
    rect(const par::cell::Cell& __cell) const;

    /**
     * Get the top left iterator
     *
     * The top left iterator is the one pointing to the first element of the
     * submatrix corresponding to the bins intersecting with the given
     * @ref par::cell::Cell.
     *
     * @param[in]  __cell The given @ref par::cell::Cell
     * @return The top left iterator
     */

    iterator top_left(const par::cell::Cell& __cell)
    {
      return
        iterator(
          m_grid.begin() + 1 + __cell.position.y(),
          0,
          static_cast<difference_type>(__cell.type->size.width),
          __cell.position.x()
        );
    }

    /**
     * Get the top left iterator
     *
     * The top left iterator is the one pointing to the first element of the
     * submatrix corresponding to the bins intersecting with the given
     * @ref par::cell::Cell.
     *
     * @param[in]  __cell The given @ref par::cell::Cell
     * @return The top left iterator
     */

    const_iterator top_left(const par::cell::Cell& __cell) const
    {
      return
        const_iterator(
          m_grid.cbegin() + 1 + __cell.position.y(),
          0,
          static_cast<difference_type>(__cell.type->size.width),
          __cell.position.x()
        );
    }

    /**
     * Get the bottom right iterator
     *
     * The bottom right iterator is the one pointing to the past-the-last
     * element of the submatrix corresponding to the bins intersecting with the
     * given @ref par::cell::Cell.
     *
     * Thus, the returned iterator is, in fact, not the bottom right iterator
     * but the one just after it.
     *
     * @param[in] __cell The bottom right iterator
     * @return The bottom right iterator
     */

    iterator bottom_right(const par::cell::Cell& __cell)
    {
      return
        iterator(
          m_grid.begin() +
            __cell.position.y() +
            static_cast<difference_type>(__cell.type->size.height)
            + 1,

          0,
          static_cast<difference_type>(__cell.type->size.width),
          __cell.position.x()
        );
    }

    /**
     * Get the bottom right iterator
     *
     * The bottom right iterator is the one pointing to the past-the-last
     * element of the submatrix corresponding to the bins intersecting with the
     * given @ref par::cell::Cell.
     *
     * Thus, the returned iterator is, in fact, not the bottom right iterator
     * but the one just after it.
     *
     * @param[in] __cell The bottom right iterator
     * @return The bottom right iterator
     */

    const_iterator bottom_right(const par::cell::Cell& __cell) const
    {
      return
        const_iterator(
          m_grid.cbegin() +
            __cell.position.y() +
            static_cast<difference_type>(__cell.type->size.height)
            + 1,

          0,
          static_cast<difference_type>(__cell.type->size.width),
          __cell.position.x()
        );
    }
  public:
    /**
     * Insert a new @ref cell::Cell in the grid
     *
     * @param[in] __cell The @ref cell::Cell to insert
     * @return The _rectangle_ of this @ref cell::Cell
     */

    std::pair<iterator, iterator> insert(cell::Cell& __cell);

    /**
     * Erase a @ref cell::Cell from the grid
     *
     * @param[in] __cell The @ref cell::Cell to erase
     * @return The _rectangle_ of this @ref cell::Cell
     */

    std::pair<iterator, iterator> erase(cell::Cell& __cell);
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
   * @param[in] __t The current temperature
   * @return The next temperature value
   *
   * @see https://doi.org/10.1145/103724.103725
   */

  constexpr real_t reduce(real_t __t) noexcept
    { return .98_r * __t; }

  /* row length */

  /**
   * The row length cost
   */

  class row_length_cost
  {
    RLST_ENFORCE_RULE_OF_FOUR(row_length_cost);
  public:
    /**
     * Construct a row length cost object
     *
     * @param[in] __desired_row_length The desired row length to be used for
     * computing the cost
     */

    explicit constexpr row_length_cost(real_t __desired_row_length) noexcept
      : m_desired_row_length(__desired_row_length)
    {}
  public:
    /**
     * Get the row length cost
     *
     * @param[in] __penalty The current row length penalty
     * @return The row length cost
     */

    constexpr real_t operator()(real_t __penalty) const
    {
      return
        normalize(
          (__penalty - m_desired_row_length) /
          m_desired_row_length
        );
    }
  private:
    real_t m_desired_row_length;
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

  /**
   * Compute the wire length penalty of net
   *
   * @tparam InputIt0 The iterator type (enable perfect forwarding)
   * @tparam InputIt1 The iterator type (enable perfect forwarding)
   *
   * @param[in] __begin The begin iterator of the net
   * @param[in] __end The end iterator of the net
   *
   * @return The wire length penalty of the given net
   */

  template <
    class InputIt0,
    class InputIt1,

    std::enable_if_t<
      std::is_same_v<
        std::decay_t<InputIt0>,
        std::decay_t<InputIt1>>
    >* = nullptr
  >
  constexpr real_t wire_length_penalty(InputIt0&& __begin, InputIt1&& __end)
  {
    return
      std::reduce(
        std::forward<InputIt0>(__begin),
        std::forward<InputIt1>(__end),
        0._r,

        core::transformed_binop(
          [] (const std::pair<cell::PlacedPort, cell::PlacedPort>& __net) {
            Point first = __net.first.parent.get().position;

            first.x() += __net.first.port.position.x();
            first.y() += __net.first.port.position.y();
            first.z() += __net.first.port.position.z();

            Point second = __net.second.parent.get().position;

            second.x() += __net.second.port.position.x();
            second.y() += __net.second.port.position.y();
            second.z() += __net.second.port.position.z();

            return static_cast<real_t>(first.manhattan(second));
          }
        )
      );
  }

  /**
   * Compute the wire length cost from the given wire length penalty
   *
   * @param[in] __penalty The wire length penalty
   * @return The wire length cost
   *
   * @see wire_length_penalty()
   */

  inline real_t wire_length_cost(real_t __penalty)
    { return normalize(__penalty); }

  /**
   * Get a random neighbor of a @ref cell::Cell
   *
   * @param[in] __cell The @ref cell::Cell to get the neighbor from
   * @param[in] __frame The frame that should contain the @ref cell::Cell
   *
   * @return A random neighbor of the @ref cell::Cell
   */

  inline Point random_neighbor(const cell::Cell& __cell, const Size& __frame)
  {
    return
      Point(
        std::min(
          core::randint(
            0_l,
            static_cast<literal_t>(__frame.width - __cell.type->size.width)
          ),

          static_cast<literal_t>(__cell.type->size.width) / 10_l
        ),

        std::min(
          core::randint(
            0_l,
            static_cast<literal_t>(__frame.height - __cell.type->size.height)
          ),

          static_cast<literal_t>(__cell.type->size.height) / 10_l
        ),

        0_l
      );
  }
}

#include "placement.ipp"

#endif // RLST_RLST_PAR_PLACEMENT_HPP
