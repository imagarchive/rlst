#include <boost/format.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cstdlib>
#include <exception>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>

#include "include/geometry.hpp"
#include "par/cell/Cell.hpp"
#include "parse_source/Parse.hpp"
#include "parse_source/pTypes.hpp"

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

// List of all CellTypes
std::list<par::cell::CellType> CellTypes;
std::unordered_map<std::string, std::shared_ptr<par::cell::CellType>>
    mapCellTypes;

void initCellTypes(std::list<par::cell::CellType> __cell_types) {
  CellTypes = std::move(__cell_types);

  for (auto &type : CellTypes) {
    mapCellTypes[type.name] = std::make_shared<par::cell::CellType>(type);
  }
}

std::string to_uppercase(const std::string &str) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::toupper(c); });
  return result;
}

std::shared_ptr<par::cell::CellType> findCellType(std::string rawTypeString) {
  // Get exact name from rawTypeString
  std::string rawTypeStringUpper =
      to_uppercase(rawTypeString.substr(2, rawTypeString.size() - 3));

  // std::cout << "rawTypeStringUpper is : " << rawTypeStringUpper << std::endl;

  return mapCellTypes[rawTypeStringUpper];
}

// Map each pCell to a Cell
std::unordered_map<std::string, std::shared_ptr<par::cell::Cell>> cellMap;

// Map each pPort to a Port
std::unordered_map<std::string, std::shared_ptr<par::cell::Port>> portMap;

std::list<par::cell::Cell> getCells(std::vector<pModule> &moduleList) {
  using namespace rlst;

  // Get all pCells
  std::list<pCell> pCellList = {};
  for (pModule &mod : moduleList) {
    pCellList.splice(pCellList.end(), mod.cells);
  }

  std::list<par::cell::Cell> cellList = {};
  for (pCell &cell : pCellList) {
    par::cell::Cell newCell = par::cell::Cell(Point(), findCellType(cell.pType));
    cellList.push_back(newCell);

    // Correspondance between pPorts and Ports (key will be name of the cell + name of the parent)
    // get an iterator for each of the pCell's three pPort lists
    std::vector<std::shared_ptr<pPort>>::iterator inputIt = cell.inputPorts.begin();
    std::vector<std::shared_ptr<pPort>>::iterator outputIt = cell.outputPorts.begin();
    // std::vector<std::shared_ptr<pPort>>::iterator inoutIt = cell.inoutPorts.begin();

    for (auto &port : newCell.type()->ports) {
      if (port.type == par::cell::PortType(input)) {
        portMap[(*inputIt)->parent->name + (*inputIt)->name] = std::make_shared<par::cell::Port>(port);
        inputIt++;
      } else if (port.type == par::cell::PortType(output)) {
        portMap[(*outputIt)->parent->name + (*inputIt)->name] = std::make_shared<par::cell::Port>(port);
        outputIt++;
      }
    }

    // map each pCell to a Cell
    cellMap[cell.name] = std::make_shared<par::cell::Cell>(newCell);
  }

  return cellList;
}

std::list<std::pair<pPort, pPort>>
computePConnections(std::vector<pModule> &moduleList) {
  std::list<std::pair<pPort, pPort>> pConnections;
  for (auto &mod : moduleList) {
    pConnections.splice(pConnections.end(), mod.computePConnections());
  }

  return pConnections;
}

std::list<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>>
computeConnections(std::list<std::pair<pPort, pPort>> &pConnections,
                   std::list<par::cell::Cell> &cellList) {
  std::list<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>>
      net_t;
  
  // Reminder: placedPort contains:
  // - std::reference_wrapper<const Cell> parent;
  // - std::reference_wrapper<const Port> port;

  for (auto &pConnection : pConnections) {
    pPort lhsPort = pConnection.first;
    pPort rhsPort = pConnection.second;

    // Get lhs Cell, make PlacedPort
    par::cell::Cell lhsCell = *cellMap[lhsPort.parent->name];
    par::cell::Cell rhsCell = *cellMap[rhsPort.parent->name];

    par::cell::PlacedPort lhsPlacedPort = {std::ref(lhsCell), std::ref(*portMap[lhsPort.parent->name + lhsPort.name])};
    par::cell::PlacedPort rhsPlacedPort = {std::ref(rhsCell), std::ref(*portMap[rhsPort.parent->name + rhsPort.name])};

    net_t.push_back(std::make_pair(lhsPlacedPort, rhsPlacedPort));
  }

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

  // // Generate par::cell:Cell list
  // std::list<par::cell::Cell> cellList = getCells(moduleList);
  //
  // // Generate ret_t
  // std::list<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>> net_t =
  //     computeConnections(moduleList);

  // ---------------------------------------------------------------------
  // TODO : New workflow:
  // - compute pConnections for pModule, pCell and pPort classes
  // - create std::list<Cell>, and a correspondance between pCells and Cells (a
  //   Cell needs shared_ptr<CellType>, which NEEDS to be unique, we have a map
  //   for that) (Any reference_wrapper fuckery would doom us)
  // - create connections: for each pConnection, get corresponding Port and
  //   corresping Cell, that makes a PlacedPort, but would the Port survive?

  // Compute pConnections
  std::list<std::pair<pPort, pPort>> pConnections =
      computePConnections(moduleList);

  // Compute cellList and get correspondance between pCells and Cells
  std::list<par::cell::Cell> cellList = getCells(moduleList);

  // Compute final connectionx
  std::list<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>>
      net_t = computeConnections(pConnections, cellList);

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

  return std::make_pair(std::move(net_t), std::move(cellList));
  // ---------------------------------------------------------------------
}
