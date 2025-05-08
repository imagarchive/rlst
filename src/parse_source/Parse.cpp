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
    }
  }

  return cellList;
}

void computeConnections(std::vector<pModule> moduleList) {
  // compute internal connections for each module
  for (pModule module : moduleList) {
    module.computeConnections();
  }

  // compute connections between modules
}

int main(int argc, char *argv[]) {
  // initialize yosys environment
  // system("source ../../oss-cad-suite/environment");

  /* execute synthesis script, generating gate_data.json and
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

  return 0;
}
