#include <memory>
#include <string>

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

void pCell::computeConnections() {
  // output -> input/inoutput
  for (pPort outputPort: outputPorts) {
    // Find connections with input ports
    for (pPort inputPort: inputPorts) {
      if (outputPort.bitVector == inputPort.bitVector) {
        outputPort.connections.push_back(inputPort);
        inputPort.connections.push_back(outputPort);
      }
    }

    // Find connections with inoutput ports
    for (pPort inoutPort: inoutPorts) {
      if (outputPort.bitVector == inoutPort.bitVector) {
        outputPort.connections.push_back(inoutPort);
        inoutPort.connections.push_back(outputPort);
      }
    }
  }

  // input -> inoutput
  for (pPort inputPort: inputPorts) {
    for (pPort inoutPort: inoutPorts) {
      if (inputPort.bitVector == inoutPort.bitVector) {
        inputPort.connections.push_back(inoutPort);
        inoutPort.connections.push_back(inputPort);
      }
    }
  }
}



