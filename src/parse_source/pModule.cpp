#include "pTypes.hpp"

pModule::pModule(std::string module_name, pt::ptree module_tree) {
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
void pModule::computeConnections() {
  for (pCell cell : cells) {
    cell.computeConnections();
  }
}
