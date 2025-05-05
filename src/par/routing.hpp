#ifndef RLST_RLST_PAR_ROUTING_HPP
#  define RLST_RLST_PAR_ROUTING_HPP

/**
 * This namespace describes all utilities related to Place And Route
 */

#include "par/geometry.hpp"
#include "par.hpp"

#include <vector>

namespace rlst::par
{
  /**
   * Given a placement, returns routes
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
   * @return The segments that represent the routes
   */

  std::vector<Point> route(
    std::vector<net_t>& __nets,
    const Point& __bottom_left,
    const Size& __size
  );
}

#endif // RLST_RLST_PAR_ROUTING_HPP
