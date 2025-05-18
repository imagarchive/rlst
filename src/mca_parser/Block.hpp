#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <string>
#include <map>

class Block
{
    private:
        int posx;
        int posy;
        int posz;
        std::string name;
        std::map<std::string, std::string> properties;
    public:
        Block(int posx, int posy, int posz, std::string name, std::map<std::string, std::string> properties);

        Block();

        void printBlock() const;

        int getPosx() const;

        int getPosy() const;

        int getPosz() const;

        std::string getName() const;

        std::map<std::string, std::string> getProperties() const;
};

#endif