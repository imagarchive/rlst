#include <iostream>

#include "parse_source/pTypes.hpp"
#include "par/cell/ports.hpp"

pModule::pModule(std::string module_name, pt::ptree module_tree) {
  std::cout << "CREATING MODULE : " << module_name << std::endl;
  this->name = module_name;

  // access ports attribute
  pt::ptree ports_subtree = module_tree.get_child("ports");

  // list of all ports, which will then be assigned to the current module
  // instance depending on whether they're inputs, outputs, or inoutputs
  std::vector<pPort> ports_list;

  // create ports_list
  for (const auto &port : ports_subtree) {
    ports_list.push_back(pPort(port.first, port.second));
  }

  // initialize input, output and inoutput ports
  for (pPort &port : ports_list) {
    if (port.direction == input) {
      this->inputPorts.push_back(port);
      this->inputNb++;
    } else if (port.direction == output) {
      this->outputPorts.push_back(port);
      this->outputNb++;
    } else {
      this->inoutPorts.push_back(port);
      this->inoutNb++;
    }
  }

  // access cells attribute
  pt::ptree cells_subtree = module_tree.get_child("cells");

  // initialize cells
  for (const auto &cell : cells_subtree) {
    this->cells.push_back(pCell(cell.first, cell.second));
  }
}

/**
 * Compute a module's connections using simplest algorithm possible:
 *
 * for each cell -> for each port:
 *
 *  - if output, check for corresponding bitVecor in each cell -> each
 * input/inoutput port
 *
 *  - if input, check for corresponding bitVector in each cell -> each
 * inoutput port
 *
 */
std::list<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>>
pModule::computeConnections() {
  std::list<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>> modNet_t;

  for (pCell &cell : cells) {
    for (pCell &otherCell : cells) {
      std::list<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>>
          cellNet_t = cell.computeConnections(otherCell);
      modNet_t.splice(modNet_t.end(), cellNet_t);
    }
  }

  return modNet_t;
}

std::list<std::pair<pPort, pPort>> pModule::computePConnections() {
  std::list<std::pair<pPort, pPort>> pConnections;
  for (auto &cell : cells) {
    for (auto &otherCell : cells) {
      pConnections.splice(pConnections.end(), cell.computePConnections(otherCell));
    }
  }
  return pConnections;
}

void pModule::placePorts() {
  for (pCell &cell : cells) {
    // Create corresponding PlacedPort with par::cell::Cell parent and
    // par::cell::port port (a @ref to the cell type) for each pCell
    cell.placePorts();
  }
}
