#include "pTypes.hpp"

pPort::pPort(std::string name, pt::ptree port_tree) {
  this->name = name;

  // create ports
  std::string direction = port_tree.get<std::string>("direction");
  int bitVector =
      port_tree.get_child("bits").begin()->second.get<int>("<0>", 0);

  if (direction == "input") {
    this->direction = input;
  } else if (direction == "output") {
    this->direction = output;
  } else if (direction == "inout") {
    this->direction = inout;
  }

  this->bitVector = bitVector;
}

pPort::pPort(std::string name, pPortDirection direction) {
  this->name = name;
  this->direction = direction;
}

void pPort::setBitVector(int bitVector) { this->bitVector = bitVector; };
