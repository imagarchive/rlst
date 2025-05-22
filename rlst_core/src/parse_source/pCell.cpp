#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "parse_source/pTypes.hpp"
#include "par/cell/ports.hpp"
#include "par/cell/types.hpp"

// List of all CellTypes
std::list<par::cell::CellType> CellTypes;

void initCellTypes(std::list<par::cell::CellType> __cell_types)
  { CellTypes = std::move(__cell_types); }

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

  std::cout << "rawTypeStringUpper is : " << rawTypeStringUpper << std::endl;

  for (std::list<par::cell::CellType>::iterator it = CellTypes.begin();
       it != CellTypes.end(); it++) {
    std::string typeName = it->name;

    if (rawTypeStringUpper == typeName) {
      return std::make_shared<par::cell::CellType>(*it);
    }
  }

  // Fail when no corresponding CellType is found
  perror("Missing cell type\n");
  exit(-1);
}

pCell::pCell(std::string cell_name, pt::ptree cell_tree) {
  std::cout << "\n----------------------------------------" << std::endl;
  std::cout << "CREATING CELL : " << cell_name << std::endl;
  this->name = cell_name;

  // initialize cell type
  std::string parsed_type = cell_tree.get<std::string>("type");
  std::cout << "Cell is of type : " << parsed_type << std::endl;

  this->type = findCellType(parsed_type);
  std::cout << "Computed type is : " << this->type->name << std::endl;

  // list of all ports, which will then be assigned to the current module
  // instance depending on whether they're inputs, outputs, or inoutputs
  std::vector<std::shared_ptr<pPort>> ports_list;

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
    std::shared_ptr<pPort> newPort =
        std::make_shared<pPort>(direction.first, portDirection);
    ports_list.push_back(newPort);

    if (directionName == "input") {
      this->inputPorts.push_back(newPort);
      this->inputNb++;
    } else if (directionName == "output") {
      this->outputPorts.push_back(newPort);
      this->outputNb++;
    } else {
      this->inoutPorts.push_back(newPort);
      this->inoutNb++;
    }
  }

  // initialize each port's bitVector
  for (const auto &port : connections_subtree) {
    std::string port_name = port.first;
    const pt::ptree &connections_array = port.second;

    for (const auto &connection : connections_array) {
      int bitVect = connection.second.get_value<int>();

      // Put val as the bitVector of the correct port
      for (const auto &port : ports_list) {
        if (port->name == port_name) {
          port->setBitVector(bitVect);
        }
      }
    }
  }

  std::cout << "Its ports are: " << std::endl;
  for (const auto &port : ports_list) {
    std::cout << "    " << port->name << " | bitVector: " << port->bitVector
              << " | direction: " << port->direction << std::endl;
  }

  std::cout << "----------------------------------------\n" << std::endl;
}

/**
 * Create a correspondance between every pPort to a unique PlacedPort
 */
void pCell::placePorts() {
  // Get input, output and inoutput par::Port list from cellType
  std::list<par::cell::PlacedPort> inputParPorts;
  std::list<par::cell::PlacedPort> outputParPorts;
  std::list<par::cell::PlacedPort> inoutParPorts;

  for (par::cell::Port &parPort : type->ports) {
    // create the PlacedPort
    par::cell::PlacedPort placed { parCell, parPort };
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

  for (const auto &inputPort : inputPorts) {
    parInputPorts.insert(std::make_pair(inputPort->bitVector, *inputParPortsIt));
  }
  for (const auto &outputPort : outputPorts) {
    parOutputPorts.insert(std::make_pair(outputPort->bitVector, *outputParPortsIt));
  }
  for (const auto &inoutPort : outputPorts) {
    parInoutPorts.insert(std::make_pair(inoutPort->bitVector, *inoutParPortsIt));
  }
}

std::list<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>>
pCell::computeConnections(pCell& otherCell) {
  std::list<std::pair<par::cell::PlacedPort, par::cell::PlacedPort>> cellNet_t;

  // std::cout << "COMPUTING CELL CONNECTIONS" << std::endl;
  // output -> input/inoutput
  for (const auto &outputPort : outputPorts) {
    // Find connections with input ports
    for (const auto &inputPort : otherCell.inputPorts) {
      if (outputPort->bitVector == inputPort->bitVector) {
        outputPort->connections.push_back(inputPort);
        inputPort->connections.push_back(outputPort);

        par::cell::PlacedPort parOutputPort =
            parOutputPorts.at(outputPort->bitVector);
        par::cell::PlacedPort parInputPort =
            otherCell.parInputPorts.at(inputPort->bitVector);

        cellNet_t.push_back(std::make_pair(parOutputPort, parInputPort));
        std::cout << outputPort->name << ", " << outputPort->bitVector
                  << " --> " << inputPort->name << ", " << inputPort->bitVector
                  << std::endl;
      }
    }

    // Find connections with inoutput ports
    for (const auto &inoutPort : otherCell.inoutPorts) {
      if (outputPort->bitVector == inoutPort->bitVector) {
        outputPort->connections.push_back(inoutPort);
        inoutPort->connections.push_back(outputPort);

        par::cell::PlacedPort parOutputPort =
            parOutputPorts.at(outputPort->bitVector);
        par::cell::PlacedPort parInoutPort =
            otherCell.parInputPorts.at(inoutPort->bitVector);

        cellNet_t.push_back(std::make_pair(parOutputPort, parInoutPort));
        std::cout << outputPort->name << ", " << outputPort->bitVector
                  << " --> " << inoutPort->name << ", " << inoutPort->bitVector
                  << std::endl;
      }
    }
  }

  // input -> inoutput
  for (const auto &inputPort : inputPorts) {
    for (const auto &inoutPort : otherCell.inoutPorts) {
      if (inputPort->bitVector == inoutPort->bitVector) {
        inputPort->connections.push_back(inoutPort);
        inoutPort->connections.push_back(inputPort);

        par::cell::PlacedPort parInputPort =
            parOutputPorts.at(inputPort->bitVector);
        par::cell::PlacedPort parInoutPort =
            otherCell.parInputPorts.at(inoutPort->bitVector);

        cellNet_t.push_back(std::make_pair(parInputPort, parInoutPort));
        std::cout << inputPort->name << ", " << inputPort->bitVector << " --> "
                  << inoutPort->name << ", " << inoutPort->bitVector
                  << std::endl;
      }
    }
  }

  return cellNet_t;
}
