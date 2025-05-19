#include <boost/format.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include <vector>

#include "pTypes.hpp"
#include "par/cell/Cell.hpp"
#include "par/cell/types.hpp"

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

std::list<par::cell::Cell> getCells(std::vector<pModule> moduleList) {
  std::list<par::cell::Cell> cellList = {};
  for (pModule mod : moduleList) {
    for (pCell cell : mod.cells) {
      par::cell::Cell newCell = {
          par::Point(), std::make_shared<par::cell::CellType>(cell.type)};
      cellList.push_back(newCell);

      // attribute this par::cell::Cell to the pCell
      cell.parCell = newCell;
    }

    // create placedPorts
    mod.placePorts();
  }

  return cellList;
}

std::set<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>>
computeConnections(std::vector<pModule> moduleList) {
  // compute internal connections for each module
  std::set<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>> net_t;
  for (pModule module : moduleList) {
    std::set<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>> modNet_t =
        module.computeConnections();
    net_t.insert(modNet_t.begin(), modNet_t.end());
  }

  // TODO : compute connections between modules (necessary?)
  return net_t;
}

int main(int argc, char *argv[]) {
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
  std::set<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>> net_t =
      computeConnections(moduleList);

  return 0;
}
