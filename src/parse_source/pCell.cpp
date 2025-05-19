#include <memory>
#include <string>
#include <utility>

#include "pTypes.hpp"
#include "par/cell/ports.hpp"
#include "par/cell/types.hpp"

// List of all CellTypes
std::list<par::cell::CellType> CellTypes = initCellTypes();

std::string to_uppercase(const std::string &str) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::toupper(c); });
  return result;
}

par::cell::CellType findCellType(std::string rawTypeString) {
  // Get exact name from rawTypeString
  std::string rawTypeStringUpper =
      to_uppercase(rawTypeString.substr(2, rawTypeString.size() - 3));

  for (std::list<par::cell::CellType>::iterator it = CellTypes.begin();
       it != CellTypes.end(); it++) {
    if (rawTypeString.find(it->name)) {
      return *it;
    }
  }

  // Fail when no corresponding CellType is found
  perror("Missing cell type\n");
  exit(-1);
}

pCell::pCell(std::string cell_name, pt::ptree cell_tree) {
  this->name = cell_name;

  // initialize cell type
  std::string type = cell_tree.get<std::string>("type");
  this->type = findCellType(this->name);

  // list of all ports, which will then be assigned to the current module
  // instance depending on whether they're inputs, outputs, or inoutputs
  std::vector<pPort> ports_list;

  // access directions attribute
  pt::ptree directions_subtree = cell_tree.get_child("port_directions");

  // access connections attribute
  pt::ptree connections_subtree = cell_tree.get_child("connections");

  // initialie ports (only name and direction)
  for (const auto &direction : directions_subtree) {
    std::string directionName = direction.second.data();
    pPortDirection portDirection;
    if (directionName == "input") {
      portDirection = input;
    } else if (directionName == "output") {
      portDirection = output;
    } else {
      portDirection = inout;
    }
    ports_list.push_back(pPort(direction.first, portDirection));
  }

  // initialize each port's bitVector
  size_t count = 0;
  for (const auto &connection : connections_subtree) {
    ports_list.at(count).setBitVector(connection.second.get<int>("<0>", 0));
    count++;
  }
}

/**
 * Create a correspondance between every pPort to a unique PlacedPort
 */
void pCell::placePorts() {
  // Get input, output and inoutput par::Port list from cellType
  std::list<par::cell::PlacedPort> inputParPorts;
  std::list<par::cell::PlacedPort> outputParPorts;
  std::list<par::cell::PlacedPort> inoutParPorts;

  for (par::cell::Port parPort : type.ports) {
    // create the PlacedPort
    par::cell::PlacedPort placed = {
        std::make_shared<const par::cell::Cell>(parCell), parPort};
    if (parPort.type == par::cell::PortType(input)) {
      inputParPorts.push_back(placed);
    } else if (parPort.type == par::cell::PortType(output)) {
      outputParPorts.push_back(placed);
    } else if (parPort.type == par::cell::PortType(inout)) {
      inoutParPorts.push_back(placed);
    }
  }

  // parPort lists iterators
  std::list<par::cell::PlacedPort>::iterator inputParPortsIt =
      inputParPorts.begin();
  std::list<par::cell::PlacedPort>::iterator outputParPortsIt =
      outputParPorts.begin();
  std::list<par::cell::PlacedPort>::iterator inoutParPortsIt =
      inoutParPorts.begin();

  for (pPort inputPort : inputPorts) {
    parInputPorts[inputPort.bitVector] = *inputParPortsIt;
  }
  for (pPort outputPort : outputPorts) {
    parOutputPorts[outputPort.bitVector] = *outputParPortsIt;
  }
  for (pPort inoutPort : outputPorts) {
    parInoutPorts[inoutPort.bitVector] = *inoutParPortsIt;
  }
}

std::set<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>> pCell::computeConnections(pCell otherCell) {
  std::set<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>> cellNet_t;

  // output -> input/inoutput
  for (pPort outputPort : outputPorts) {
    // Find connections with input ports
    for (pPort inputPort : otherCell.inputPorts) {
      if (outputPort.bitVector == inputPort.bitVector) {
        outputPort.connections.insert(inputPort);
        inputPort.connections.insert(outputPort);

        par::cell::PlacedPort parOutputPort = parOutputPorts[outputPort.bitVector];
        par::cell::PlacedPort parInputPort = otherCell.parInputPorts[inputPort.bitVector];

        cellNet_t.insert(std::make_pair(parOutputPort, parInputPort));
      }
    }

    // Find connections with inoutput ports
    for (pPort inoutPort : otherCell.inoutPorts) {
      if (outputPort.bitVector == inoutPort.bitVector) {
        outputPort.connections.insert(inoutPort);
        inoutPort.connections.insert(outputPort);

        par::cell::PlacedPort parOutputPort = parOutputPorts[outputPort.bitVector];
        par::cell::PlacedPort parInoutPort = otherCell.parInputPorts[inoutPort.bitVector];

        cellNet_t.insert(std::make_pair(parOutputPort, parInoutPort));
      }
    }
  }

  // input -> inoutput
  for (pPort inputPort : inputPorts) {
    for (pPort inoutPort : otherCell.inoutPorts) {
      if (inputPort.bitVector == inoutPort.bitVector) {
        inputPort.connections.insert(inoutPort);
        inoutPort.connections.insert(inputPort);

        par::cell::PlacedPort parInputPort = parOutputPorts[inputPort.bitVector];
        par::cell::PlacedPort parInoutPort = otherCell.parInputPorts[inoutPort.bitVector];

        cellNet_t.insert(std::make_pair(parInputPort, parInoutPort));
      }
    }
  }

  return cellNet_t;
}

void pCell::computeNetList() {
  for (pPort outputPort : outputPorts) {
    for (pPort connection : outputPort.connections) {
      par::cell::PlacedPort parOutputPort = parOutputPorts[outputPort.bitVector];

    }
  }
}
