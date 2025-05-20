#include <algorithm>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "par/cell/Cell.hpp"
#include "par/routing.hpp"

/*
 * Cost is length + redstone length that could be traversed in the number of
 * ticks it takes to go up to then down from the level of the route
 */

/* The number of blocks a signal can go without a repeater.
 * If block 0 is a restone block or a repeater, then block MAX_SIGNAL_LENGTH
 * will be powered but won't be able to power anything (i.e. if block
 * MAX_SIGNAL_LENGTH is a repeater or a piston it will be on, but if it's
 * redstone it will be off).
 */
constexpr uint8_t MAX_SIGNAL_LENGTH = 16;
/* The cost of going 1 level higher */
constexpr uint8_t LEVEL_COST = MAX_SIGNAL_LENGTH * 2; // TODO adjust
/* The maximum number of levels */
constexpr uint8_t MAX_LEVEL = 70; // TODO adjust

namespace rlst::par
{
  /**
   * 3d vector that represents occupied space
   */
  class Levels {
  public:
    Levels(Size __size) :
      x_length(__size.width),
      y_length(__size.height),
      z_length(MAX_LEVEL)
    {
      vec = std::vector<uint8_t>(x_length * y_length * z_length);
    }

    /**
     * Returns whether there are no reasons for a space to be unavailable
     */
    bool available(uliteral_t __x, uliteral_t __y, uliteral_t __z) const {
      if (__x >= x_length || __y >= y_length || __z >= z_length) return false;
      return vec.at(__x + __y * x_length + __z * x_length * y_length) == 0;
    }

    /**
     * Remove a reason to make a space unavailable
     */
    void remove_reason(uliteral_t __x, uliteral_t __y, uliteral_t __z)
    {
      assert(__x < x_length);
      assert(__y < y_length);
      assert(__z < z_length);
      --vec.at(__x + __y * x_length + __z * x_length * y_length);
      assert(vec.at(__x + __y * x_length + __z * x_length * y_length) >= 0);
    }

    /**
     * Add a reason to make a space unavailable
     */
    void add_reason(uliteral_t __x, uliteral_t __y, uliteral_t __z)
    {
      assert(__x < x_length);
      assert(__y < y_length);
      assert(__z < z_length);
      ++vec.at(__x + __y * x_length + __z * x_length * y_length);
    }

    /**
     * Add a reason to make a space and the spaces north, east, west, and south
     * of it unavailable
     */
    void add_reason_cross(uliteral_t __x, uliteral_t __y, uliteral_t __z)
    {
      assert(__x < x_length);
      assert(__y < y_length);
      assert(__z < z_length);
      add_reason(__x, __y, __z);
      uliteral_t x = __x + 1;
      if (x < x_length) add_reason(x, __y, __z);
      uliteral_t y = __y + 1;
      if (y < y_length) add_reason(__x, y, __z);
      if (__x > 0) {
        x = __x - 1;
        add_reason(x, __y, __z);
      }
      if (__y > 0) {
        y = __y - 1;
        add_reason(__x, y, __z);
      }
    }

    /**
     * Add a reason to make a space and the spaces north, east, west, and south
     * of it unavailable
     */
    void remove_reason_cross(uliteral_t __x, uliteral_t __y, uliteral_t __z)
    {
      assert(__x < x_length);
      assert(__y < y_length);
      assert(__z < z_length);
      remove_reason(__x, __y, __z);
      uliteral_t x = __x + 1;
      if (x < x_length) remove_reason(x, __y, __z);
      uliteral_t y = __y + 1;
      if (y < y_length) remove_reason(__x, y, __z);
      if (__x > 0) {
        x = __x - 1;
        remove_reason(x, __y, __z);
      }
      if (__y > 0) {
        y = __y - 1;
        remove_reason(__x, y, __z);
      }
    }
  private:
    /**
     * 0 is available, +1 for each reason for a space to be unavailable
     *
     * This allows temporarily removing a reason when the spaces around a
     * PlacedPort need to be made available to route to/from it
     *
     * This way, if there is a nearby input/output also making one/some of these
     * spaces unavailable, they will remain unavailable
     */
    std::vector<uint8_t> vec;

    uliteral_t x_length, y_length, z_length;
  };

  /**
   * Returns the "Manhattan" route cost
   */
  uliteral_t manhattan_cost(const net_t& __net)
  {
    return
      static_cast<uliteral_t>(
        std::abs(
          cell::absolute_x(__net.first)
          - cell::absolute_x(__net.second)
        )
      + std::abs(
          cell::absolute_y(__net.first)
          - cell::absolute_y(__net.second)
        )
      );
  }

  /**
   * Returns the "Manhattan" route cost
   */
  uliteral_t manhattan_cost(const Point __src, const cell::PlacedPort& __dest)
  {
    return
      static_cast<uliteral_t>(
        std::abs(__src.x() - cell::absolute_x(__dest))
        + std::abs(__src.y() - cell::absolute_y(__dest))
      );
  }

  /**
   * Reconstructs the path found by A* (backwards)
   * RouteElement is left unset
   */
  std::vector<std::pair<Point, RouteElement>> reconstruct_path(
    const std::unordered_map<Point, Point, boost::hash<Point>>& __from,
    const Point& __start,
    const Point& __destination
  )
  {
    std::vector<std::pair<Point, RouteElement>> path;
    std::pair<Point, RouteElement> current;
    current.first = __from.at(__destination);
    while (current.first != __start) {
      path.push_back(current);
      current.first = __from.at(current.first);
    }
    return path;
  }

  /**
   * Returns the "A*" route (backwards)
   * Studies a single level
   * RouteElement is left unset
   */
  std::vector<std::pair<Point, RouteElement>> a_star(
    const net_t& __net,
    const uint8_t& level,
    const Levels& levels,
    const Point& __bottom_left
  )
  {
    Point start = Point(
      cell::absolute_x(__net.first),
      cell::absolute_y(__net.first),
      level
      );
    Point destination = Point(
      cell::absolute_x(__net.second),
      cell::absolute_y(__net.second),
      level
      );
    // estimate of the cost of best the path that goes through a point
    std::unordered_map<Point, uliteral_t, boost::hash<Point>> estimate;
    estimate[start] = manhattan_cost(start, __net.second);
    // discovered points
    auto compare = [&estimate](Point __lhs, Point __rhs) {
      // `<=` and not `<` because two points with the same estimate but not the
      // same location need to be inserted into the `discovered` set, which they
      // can only do if at least one of them returns `true` when compared to the
      // other
      return estimate[__lhs] <= estimate[__rhs];
    };
    std::set<Point, decltype(compare)> discovered(compare);
    discovered.insert(start);
    // preceding point on the shortest path
    std::unordered_map<Point, Point, boost::hash<Point>> from;
    // cost of the shortest path to each point
    std::unordered_map<Point, uliteral_t, boost::hash<Point>> cost;
    cost[start] = 0;
    // neighbors of the current point
    std::vector<Point> neighbors;
    neighbors.reserve(3);
    // find path
    while (!discovered.empty()) {
      Point current = *discovered.begin();
      if (current == destination) {
        return reconstruct_path(from, start, destination);
      }
      discovered.erase(discovered.begin());
      // POSSIBLE IMPROVEMENT: Remove repeater incompatible directions
      // from neighbors if a repeater is required
      // Currently, a repeater-incompatible route is unlikely but not impossible
      uliteral_t relative_x =
        static_cast<uliteral_t>(current.x() - __bottom_left.x());
      uliteral_t relative_y =
        static_cast<uliteral_t>(current.y() - __bottom_left.y());
      if (levels.available(relative_x + 1, relative_y, level)) {
        neighbors.push_back(Point(current.x() + 1, current.y(), level));
      }
      if (levels.available(relative_x - 1, relative_y, level)) {
        neighbors.push_back(Point(current.x() - 1, current.y(), level));
      }
      if (levels.available(relative_x, relative_y + 1, level)) {
        neighbors.push_back(Point(current.x(), current.y() + 1, level));
      }
      if (levels.available(relative_x, relative_y - 1, level)) {
        neighbors.push_back(Point(current.x(), current.y() - 1, level));
      }
      for (Point neighbor : neighbors) {
        uliteral_t new_cost = cost[current] + 1;
        // heuristic is admissible and consistent, so just check whether
        // a path already exists
        if (cost.find(neighbor) == cost.end()) {
          from[neighbor] = current;
          cost[neighbor] = new_cost;
          estimate[neighbor] =
            new_cost + manhattan_cost(neighbor, __net.second);
          // discovered is a set so it will prevent duplicates
          discovered.insert(neighbor);
        }
      }
      neighbors.clear();
    }
    // no path was found
    return std::vector<std::pair<Point, RouteElement>>();
  }

  /**
   * Given a list of routes, sets the detailed composition of the route, i.e.
   * the @RouteElement of each block of the route
   *
   * @param[in] __routes The routes
   *
   * @return The routes with an @ref RouteElement for each point
   */

  void set_route_elements(
    std::vector<std::vector<std::pair<Point, RouteElement>>>& __routes
  )
  {
    for (std::vector<std::pair<Point, RouteElement>>& route : __routes) {
      // POSSIBLE IMPROVEMENT: Allow the first and last blocks to be repeaters
      // last powered block is the one after the last repeater / redstone block
      size_t last_powered_index = 0;
      size_t last_candidate_index = 0;
      RouteElement last_candidate_repeater;
      for (
        size_t block_index = 1;
        block_index < route.size() - 1;
        ++block_index)
      {
        // set default RouteElement
        route[block_index].second = RouteElement::redstone;
        // if possible repeater, find which one
        Point previous_block_pos = route[block_index - 1].first;
        Point next_block_pos = route[block_index + 1].first;
        if (previous_block_pos.x() == next_block_pos.x()) {
          last_candidate_index = block_index;
          if (previous_block_pos.y() < next_block_pos.y()) {
            last_candidate_repeater = RouteElement::northFacingRepeater;
          } else {
            last_candidate_repeater = RouteElement::southFacingRepeater;
          }
        } else if (previous_block_pos.y() == next_block_pos.y()) {
          last_candidate_index = block_index;
          if (previous_block_pos.x() < next_block_pos.x()) {
            last_candidate_repeater = RouteElement::eastFacingRepeater;
          } else {
            last_candidate_repeater = RouteElement::westFacingRepeater;
          }
        }
        // if this is the last powered block, place a repeater
        if (block_index == last_powered_index + MAX_SIGNAL_LENGTH - 1) {
          // if the closest candidate can't power the next block, throw an error
          if (block_index + 1 > last_candidate_index + MAX_SIGNAL_LENGTH) {
            throw std::runtime_error("Could not route (repeaters)");
          }
          route[last_candidate_index].second = last_candidate_repeater;
          last_powered_index = last_candidate_index + 1;
        }
      }
      // check if the last stretch is OK
      if (route.size() > last_powered_index + MAX_SIGNAL_LENGTH - 1) {
        if (route.size() > last_candidate_index + MAX_SIGNAL_LENGTH) {
          throw std::runtime_error("Could not route (repeaters)");
        }
        route[last_candidate_index].second = last_candidate_repeater;
      }
    }
  }

  std::vector<std::vector<std::pair<Point, RouteElement>>> route(
    std::vector<net_t>& __nets,
    const Point& __bottom_left,
    const Size& __size
  )
  {
    // store the selected routes
    std::vector<std::vector<std::pair<Point, RouteElement>>> routes;
    // first level with no routes on it
    uint8_t first_empty_level = 0;
    // store which spaces are available at each level after placing the routes
    Levels levels(__size);
    // make the spaces around the placed ports unavailable
    std::unordered_set<cell::PlacedPort, boost::hash<cell::PlacedPort>> ports;
    for (net_t net : __nets) {
      ports.insert(net.first);
      ports.insert(net.second);
    }
    for (cell::PlacedPort port : ports) {
      uliteral_t x =
        static_cast<uliteral_t>(cell::absolute_x(port) - __bottom_left.x());
      uliteral_t y =
        static_cast<uliteral_t>(cell::absolute_y(port) - __bottom_left.y());
      for (uint8_t level = 0; level < MAX_LEVEL; ++level) {
        // make the cross shape around ports unavailable, it will be made
        // available when the port is a source or destination
        levels.add_reason_cross(x, y, level);
      }
    }
    // sort nets by increasing size (Manhattan)
    std::sort(__nets.begin(), __nets.end(), [](net_t __lhs, net_t __rhs) {
      return manhattan_cost(__lhs) < manhattan_cost(__rhs);
      });
    // route each net (nets are not moved once routed)
    for (const net_t& net : __nets) {
      uliteral_t best_cost = std::numeric_limits<uliteral_t>::max();
      std::vector<std::pair<Point, RouteElement>> best_route;
      for (uint8_t level = 0; level <= first_empty_level; ++level) {
        // make the spaces around the source and destination available
        uliteral_t source_x = static_cast<uliteral_t>(
            cell::absolute_x(net.first) - __bottom_left.x());
        uliteral_t source_y = static_cast<uliteral_t>(
            cell::absolute_y(net.first) - __bottom_left.y());
        uliteral_t destination_x = static_cast<uliteral_t>(
            cell::absolute_x(net.second) - __bottom_left.x());
        uliteral_t destination_y = static_cast<uliteral_t>(
            cell::absolute_y(net.second) - __bottom_left.y());
        levels.remove_reason_cross(source_x, source_y, level);
        levels.remove_reason_cross(destination_x, destination_y, level);
        // find a route
        std::vector<std::pair<Point, RouteElement>> route =
          a_star(net, level, levels, __bottom_left);
        // make the spaces around the source and destination unavailable again
        levels.add_reason_cross(source_x, source_y, level);
        levels.add_reason_cross(destination_x, destination_y, level);
        // no route found -> continue
        if (route.size() == 0) continue;
        // route found -> compare it to previous best
        uliteral_t cost = level * LEVEL_COST + route.size();
        if (cost < best_cost) {
          best_route = std::move(route);
          best_cost = cost;
        }
        if (best_cost < manhattan_cost(net) + LEVEL_COST * (level + 1)) {
          if (level == first_empty_level) ++first_empty_level;
          break;
        }
      }
      if (best_route.size() == 0) {
        throw std::runtime_error("Could not route (space)");
      }
      // occupy the route
      for (
        size_t step_index = 1;
        step_index < best_route.size() - 1;
        ++step_index)
      {
        Point step = best_route[step_index].first;
        levels.add_reason_cross(
          static_cast<uliteral_t>(step.x() - __bottom_left.x()),
          static_cast<uliteral_t>(step.y() - __bottom_left.y()),
          static_cast<uliteral_t>(step.z())
          );
      }
      // make the start and end available for other routes
      levels.remove_reason(
        static_cast<uliteral_t>(best_route[0].first.x() - __bottom_left.x()),
        static_cast<uliteral_t>(best_route[0].first.y() - __bottom_left.y()),
        static_cast<uliteral_t>(best_route[0].first.z())
        );
      levels.remove_reason(
        static_cast<uliteral_t>(
          best_route[best_route.size() - 1].first.x() - __bottom_left.x()),
        static_cast<uliteral_t>(
          best_route[best_route.size() - 1].first.y() - __bottom_left.y()),
        static_cast<uliteral_t>(best_route[best_route.size() - 1].first.z())
        );
      // reverse the route (A* returns it backwards) and save it
      std::reverse(best_route.begin(), best_route.end());
      routes.push_back(best_route);
    }

    // set the RouteElements
    set_route_elements(routes);

    return routes;
  }
}
