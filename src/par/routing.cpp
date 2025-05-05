#include <algorithm>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "par/cell/Cell.hpp"
#include "par/routing.hpp"

/* Cost is number of ticks */

/* The number of blocks a signal can go without a repeater */
constexpr uint8_t MAX_SIGNAL_LENGTH = 15;
/* The cost of going 1 level higher */
constexpr uint8_t LEVEL_COST = 16 * 2; // TODO adjust
/* The height of each level in blocks (for the z coordinate) */
constexpr uint8_t LEVEL_HEIGHT = 4; // TODO adjust
/* The z coordinate of the first (0th) level */
constexpr uint8_t FIRST_LEVEL_Z = 10; // TODO adjust
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
      if (
        __x < 0 || __x >= x_length
        || __y < 0 || __y >= y_length
        || __z < 0 || __z >= y_length
        ) return false;
      return vec.at(__x + __y * x_length + __z * x_length * y_length) == 0;
    }

    /**
     * Remove a reason to make a space unavailable
     */
    void remove_reason(uliteral_t __x, uliteral_t __y, uliteral_t __z)
    {
      assert(__x >= 0);
      assert(__y >= 0);
      assert(__z >= 0);
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
      assert(__x >= 0);
      assert(__y >= 0);
      assert(__z >= 0);
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
      assert(__x >= 0);
      assert(__y >= 0);
      assert(__z >= 0);
      assert(__x < x_length);
      assert(__y < y_length);
      assert(__z < z_length);
      add_reason(__x, __y, __z);
      ++__x;
      if (__x < x_length) add_reason(__x, __y, __z);
      --__x;
      ++__y;
      if (__y < y_length) add_reason(__x, __y, __z);
      --__y;
      --__x;
      if (__x >= 0) add_reason(__x, __y, __z);
      ++__x;
      --__y;
      if (__y >= 0) add_reason(__x, __y, __z);
    }

    /**
     * Add a reason to make a space and the spaces north, east, west, and south
     * of it unavailable
     */
    void remove_reason_cross(uliteral_t __x, uliteral_t __y, uliteral_t __z)
    {
      assert(__x >= 0);
      assert(__y >= 0);
      assert(__z >= 0);
      assert(__x < x_length);
      assert(__y < y_length);
      assert(__z < z_length);
      remove_reason(__x, __y, __z);
      ++__x;
      if (__x < x_length) remove_reason(__x, __y, __z);
      --__x;
      ++__y;
      if (__y < y_length) remove_reason(__x, __y, __z);
      --__y;
      --__x;
      if (__x >= 0) remove_reason(__x, __y, __z);
      ++__x;
      --__y;
      if (__y >= 0) remove_reason(__x, __y, __z);
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

  std::vector<Point> reconstruct_path(
    const std::unordered_map<Point, Point, boost::hash<Point>>& __from,
    const Point& __destination
  )
  {
    std::vector<Point> path;
    path.push_back(__destination);
    Point current = __destination;
    while (__from.find(current) != __from.end()) {
      current = __from.at(current);
      path.push_back(current);
    }
    std::reverse(path.begin(), path.end());
    return path;
  }

  /**
   * Returns the "A*" route
   * Studies a single level
   */
  std::vector<Point> a_star(
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
      return estimate[__lhs] < estimate[__rhs];
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
      if (current == destination) return reconstruct_path(from, current);
      discovered.erase(discovered.begin());
      // TODO remove repeater incompatible directions from neighbors if a
      // repeater is required
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
    return std::vector<Point>();
  }

  enum Orientation { vertical, horizontal };

  /**
   * Converts a route from Point to Segments
   */
  std::vector<Segment> segmentify(const std::vector<Point>& __route)
  {
    std::vector<Segment> segments;
    Segment current_segment;
    current_segment.start() = __route[0];
    // set initial direction
    Orientation direction =
      (__route[0].x() == __route[1].x()) ? vertical : horizontal;
    // create a new segment when the direction changes
    for (
      size_t step_index = 1;
      step_index < __route.size() - 1;
      ++step_index)
    {
      Point step = __route[step_index];
      if (direction == vertical && current_segment.start().x() != step.x()){
        direction = horizontal;
        segments.push_back(current_segment);
        current_segment.start() = current_segment.end();
      } else if (direction == horizontal
          && current_segment.start().y() != step.y()) {
        direction = vertical;
        segments.push_back(current_segment);
        current_segment.start() = current_segment.end();
      }
      current_segment.end() = step;
    }
    segments.push_back(current_segment);
    return segments;
  }

  std::vector<Segment> route(
    std::vector<net_t>& __nets,
    const Point& __bottom_left,
    const Size& __size
  )
  {
    // store the selected routes as segments
    std::vector<Segment> segment_routes;
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
        // make the corners definitely unavailable (for better results)
        ++x;
        if (x < __size.width) {
          ++y;
          if (y < __size.height) levels.add_reason(x, y, level);
          y -= 2;
          if (y >= 0) levels.add_reason(x, y, level);
          ++y;
        }
        x -= 2;
        if (x >= 0) {
          ++y;
          if (y < __size.height) levels.add_reason(x, y, level);
          y -= 2;
          if (y >= 0) levels.add_reason(x, y, level);
          ++y;
        }
        ++x;
      }
    }
    // sort nets by increasing size (Manhattan)
    std::sort(__nets.begin(), __nets.end(), [](net_t __lhs, net_t __rhs) {
      return manhattan_cost(__lhs) < manhattan_cost(__rhs);
      });
    // route each net (nets are not moved once routed)
    for (const net_t& net : __nets) {
      uliteral_t best_cost = std::numeric_limits<uliteral_t>::max();
      std::vector<Point> best_route;
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
        std::vector<Point> route = a_star(net, level, levels, __bottom_left);
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
        throw std::runtime_error("Could not route");
      }
      // occupy the route
      for (
        size_t step_index = 1;
        step_index < best_route.size() - 1;
        ++step_index)
      {
        Point step = best_route[step_index];
        levels.add_reason_cross(
          static_cast<uliteral_t>(step.x() - __bottom_left.x()),
          static_cast<uliteral_t>(step.y() - __bottom_left.y()),
          static_cast<uliteral_t>(step.z())
          );
      }
      // make the start and end available for other routes
      levels.remove_reason(
        static_cast<uliteral_t>(best_route[0].x() - __bottom_left.x()),
        static_cast<uliteral_t>(best_route[0].y() - __bottom_left.y()),
        static_cast<uliteral_t>(best_route[0].z())
        );
      levels.remove_reason(
        static_cast<uliteral_t>(
          best_route[best_route.size() - 1].x() - __bottom_left.x()),
        static_cast<uliteral_t>(
          best_route[best_route.size() - 1].y() - __bottom_left.y()),
        static_cast<uliteral_t>(best_route[best_route.size() - 1].z())
        );
      std::vector<Segment> best_route_segments = segmentify(best_route);
      segment_routes.insert(
        segment_routes.end(),
        best_route_segments.begin(),
        best_route_segments.end()
        );
    }
    return segment_routes;
  }
}
