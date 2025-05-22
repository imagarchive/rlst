#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <string>
#include <map>

class Block
{
    private:
        // x -> east
        // z -> south
        // y -> up
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

        void setPosX(int x);

        int getPosy() const;

        void setPosY(int y);

        int getPosz() const;

        void setPosZ(int z);

        std::string getName() const;

        std::map<std::string, std::string> getProperties() const;
};

#endif