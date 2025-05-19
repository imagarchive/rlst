#pragma once

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include "par/cell/Cell.hpp"
#include "par/cell/ports.hpp"
#include "par/cell/types.hpp"
#include <unordered_map>
#include <vector>
#include <set>

namespace pt = boost::property_tree;
namespace par = rlst::par;

class pModule;
class pLogicGate;
class pPort;

enum pPortDirection { input, output, inout };

/**
 * A cell's representation. Abstract class from which derive classes
 * representing all cell types, for example Logic Gates and Flip Flops
 */
class pCell {
public:
  std::string name;
  par::cell::CellType type;

  int inputNb = 0;
  std::vector<pPort> inputPorts;
  std::unordered_map<int, par::cell::PlacedPort> parInputPorts;

  int outputNb = 0;
  std::vector<pPort> outputPorts;
  std::unordered_map<int, par::cell::PlacedPort> parOutputPorts;

  int inoutNb = 0;
  std::vector<pPort> inoutPorts;
  std::unordered_map<int, par::cell::PlacedPort> parInoutPorts;

  par::cell::Cell parCell;

  // extract the Cell from the gate_data.json
  pCell(std::string cell_name, pt::ptree cell_tree);

  // create PlacedPorts
  void placePorts();

  // compute this cell's connections
  std::set<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>> computeConnections(pCell otherCell);

  // compute this cell's netlist (compatible with the placing and routing steps)
  void computeNetList();
};

/**
 * A port's representation. All ports are single-bit, as Yosys is instructed to
 * transform multi-bit ports into single-bit ones during synthesis
 */
class pPort {
public:
  std::string name;
  pPortDirection direction;
  int bitVector;
  std::set<pPort> connections;
  int parent;

  // extract the Port from the gate_data.json
  pPort(std::string name, pt::ptree port_tree);
  pPort(std::string name, pPortDirection direction);

  void setBitVector(int bitVector);
};

/**
 * A module's representation
 */
class pModule {
public:
  std::string name;

  int inputNb = 0;
  std::vector<pPort> inputPorts;

  int outputNb = 0;
  std::vector<pPort> outputPorts;

  int inoutNb = 0;
  std::vector<pPort> inoutPorts;

  std::vector<pCell> cells;

  // extract the Module from the gate_data.json
  pModule(std::string module_name, pt::ptree module_tree);

  // create PlacedPorts
  // TODO : check if actual creation is needed for modules
  void placePorts();

  // compute this module's connections
  std::set<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>> computeConnections();

  // compute this module's netlist (compatible with the placing and routing steps)
  void computeNetList();
};


std::list<par::cell::CellType> initCellTypes();
