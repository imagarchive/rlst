#pragma once

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include "par/cell/Cell.hpp"
#include "par/cell/ports.hpp"
#include "par/cell/types.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

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
  std::vector<std::shared_ptr<pPort>> inputPorts;
  std::unordered_map<int, par::cell::PlacedPort> parInputPorts;

  int outputNb = 0;
  std::vector<std::shared_ptr<pPort>> outputPorts;
  std::unordered_map<int, par::cell::PlacedPort> parOutputPorts;

  int inoutNb = 0;
  std::vector<std::shared_ptr<pPort>> inoutPorts;
  std::unordered_map<int, par::cell::PlacedPort> parInoutPorts;

  par::cell::Cell parCell;

  // extract the Cell from the gate_data.json
  pCell(std::string cell_name, pt::ptree cell_tree);

  // create PlacedPorts
  void placePorts();

  // compute this cell's connections
  std::list<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>> computeConnections(pCell &otherCell);
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
  std::list<std::shared_ptr<pPort>> connections;

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
  std::list<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>> computeConnections();
};


void initCellTypes(std::list<par::cell::CellType> __cell_types);
