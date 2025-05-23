#include <boost/format.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "par/cell/Cell.hpp"
#include "parse_source/Parse.hpp"
#include "parse_source/pTypes.hpp"
#include "include/geometry.hpp"

std::vector<pModule> parse_data_json() {
  std::vector<pModule> modules_list;

  std::string filename = "gate_data.json";
  pt::ptree tree;

  try {
    pt::read_json(filename, tree);

    // access modules attribute
    pt::ptree modules_subtree = tree.get_child("modules");

    // create modules_list
    for (const auto &module : modules_subtree) {
      modules_list.push_back(pModule(module.first, module.second));
    }
  } catch (const std::exception &e) {
    std::cerr << "Error parsing gate_data JSON: " << e.what() << std::endl;
  }
  return modules_list;
}

std::list<par::cell::Cell> getCells(std::vector<pModule> &moduleList) {
  using namespace rlst;

  std::list<par::cell::Cell> cellList = {};

  for (pModule &mod : moduleList) {
    std::cout << "THESE ARE THE " << mod.cells.size()
              << " PCELLS: " << std::endl;
    for (pCell &cell : mod.cells) {
      par::cell::Cell newCell(Point(), cell.type);
      cellList.push_back(newCell);

      std::cout << &cell << " | " << newCell.type() << " | "
                << newCell.type()->name << std::endl;

      // attribute this par::cell::Cell to the pCell
      cell.parCell = std::make_shared<par::cell::Cell>(newCell);
    }

    // create placedPorts
    mod.placePorts();
  }

  return cellList;
}

std::list<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>>
computeConnections(std::vector<pModule> &moduleList) {
  // compute internal connections for each module
  std::cout << "\n --- COMPUTING CONNECTIONS" << std::endl;
  std::list<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>> net_t;

  for (pModule &module : moduleList) {
    std::list<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>>
        modNet_t = module.computeConnections();
    net_t.splice(net_t.end(), modNet_t);
  }

  // TODO : compute connections between modules (necessary?):
  // add module ports the same way we do cell ports, and when computing
  // connections, also take into account module ports
  return net_t;
}

std::pair<std::list<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>>,
          std::list<par::cell::Cell>>
parse_v(int argc, char *argv[]) {
  // initialize yosys environment
  // system("source ../../oss-cad-suite/environment");

  /**
   * execute synthesis script, generating gate_data.json and
   * gate_metadata.json
   */

  // std::string synth_command =
  //     (boost::format("yosys -s ../synthesis_script.sh %s") % argv[0]).str();
  std::string synth_command = "yosys -s synthesis_script.sh";
  system(synth_command.c_str());

  // Parse gate_data.json, generation the list of modules
  std::vector<pModule> moduleList = parse_data_json();

  // Generate par::cell:Cell list
  std::list<par::cell::Cell> cellList = getCells(moduleList);

  // Generate ret_t
  /* NOTE : The Yosys synthesis script ideally only generates one module,
     therefore simplifying many things, among which is generation ret_t */
  std::list<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>> net_t =
      computeConnections(moduleList);

  // Print cellList
  std::cout << "\n --- CELL LIST" << std::endl;
  for (par::cell::Cell &cell : cellList) {
    std::cout << &(cell) << " : " << cell.type()->name << std::endl;
  }

  // Print ret_t
  std::cout << "\n --- NET_T" << std::endl;
  for (const auto &[p1, p2] : net_t) {
    std::cout << "(" << &(p1.port) << ", " << &(p2.port) << ")" << std::endl;
  }

  // Print all initial positions
  std::cout << "\n --- INITIAL POSITIONS" << std::endl;
  for (par::cell::Cell &cell : cellList) {
    rlst::Point bruh = (cell).position();
    std::cout << bruh.x() << " | " << bruh.y() << " | " << bruh.z() << std::endl;
  }

  for (const auto &[p1, p2] : net_t) {
    rlst::Point bruh = (&(p1.parent))->get()->position();
    rlst::Point bruh2 = (&(p2.parent))->get()->position();
    std::cout << bruh.x() << " | " << bruh.y() << " | " << bruh.z() << std::endl;
    std::cout << bruh2.x() << " | " << bruh2.y() << " | " << bruh2.z() << std::endl;
  }

  // Print all Cells' CellTypes addresses
  std::cout << "\n --- CELL CELLTYPES ADDRESSES" << std::endl;
  for (par::cell::Cell &cell : cellList) {
    std::cout << (cell.type()) << std::endl;
  }

  // Print initial addresses of CellTypes
  std::cout << "\n --- CELLTYPES ADDRESSES" << std::endl;
  for (const auto &pair : sharedCellTypes ) {
    std::cout << pair.second << std::endl;
  }



  return std::make_pair(std::move(net_t), std::move(cellList));
}
