#pragma once

#include "par/cell/Cell.hpp"
#include "par/cell/ports.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <utility>

namespace par = rlst::par;

// int parse_v(int argc, char *argv[]);
std::tuple<std::list<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>>,
          std::list<par::cell::Cell>, std::unordered_map<std::string, std::shared_ptr<par::cell::Port>>>
parse_v(int argc, char *argv[]);
