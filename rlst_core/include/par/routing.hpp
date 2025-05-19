#ifndef RLST_CORE_RLST_PAR_ROUTING_HPP
#  define RLST_CORE_RLST_PAR_ROUTING_HPP

/**
 * This namespace describes all utilities related to Place And Route
 */

#include "geometry.hpp"
#include "par.hpp"

#include <vector>

namespace rlst::par
{

  /**
   * The types of blocks that can appear on a route
   *
   * Added to routes by @ref route
   */

  enum class RouteElement
  {
      redstone,
      northFacingRepeater,
      eastFacingRepeater,
      southFacingRepeater,
      westFacingRepeater
  };

  /**
   * Given a placement, returns routes and their composition
   *
   * The algorithm is far from optimal, but it is decent enough for small
   * circuits.
   * One major flaw is that it does not allow two routes to join anywhere else
   * than on a terminal.
   *
   * @param[in] __nets The nets, represented by a list of placed port
   * pairs
   * @param[in] __bottom_left The bottom left corner of the area available to
   * the routing algorithm
   * @param[in] __size The size of the area available to the routing algorithm
   *
   * @return The routes and the blocks they are made of (oriented repeater or
   * redstone)
   */

  std::vector<std::vector<std::pair<Point, RouteElement>>> route(
    std::vector<net_t>& __nets,
    const Point& __bottom_left,
    const Size& __size
  );
}

#endif // RLST_CORE_RLST_PAR_ROUTING_HPP
