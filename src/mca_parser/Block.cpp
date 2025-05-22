#include <iostream>
#include "Block.hpp"

Block::Block(int posx, int posy, int posz, std::string name, std::map<std::string, std::string> properties)
  : posx(posx)
  , posy(posy)
  , posz(posz)
  , name(name)
  , properties(properties)
{}

Block::Block()
: posx(0)
, posy(0)
, posz(0)
, name("")
, properties()
{}

void Block::printBlock() const 
{
    std::cout << "block : position : " << posx << "," << posy << "," << posz << " name : " << name << std::endl;
}

int Block::getPosx() const 
{
    return posx;
}

int Block::getPosy() const
{
    return posy;
}

int Block::getPosz() const
{
    return posz;
}

void Block::setPosX(int x) {
    posx = x;
}

void Block::setPosY(int y) {
    posy = y;
}

void Block::setPosZ(int z) {
    posz = z;
}

std::string Block::getName() const
{
    return name;
}

std::map<std::string, std::string> Block::getProperties() const
{
    return properties;
}
