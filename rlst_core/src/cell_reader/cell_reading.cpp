#include "par/cell/types.hpp"

#include <boost/json/src.hpp>
#include <fstream>

namespace rlst::gate_reader
{
  par::cell::CellType generate_gate(const std::string __file_name)
  {
    // Read the file
    std::ifstream gate_file(__file_name);
    std::stringstream file_contents;
    file_contents << gate_file.rdbuf();
    gate_file.close();
    boost::json::object gate_json =
      boost::json::parse(file_contents.str()).as_object();

    // Generate the CellType
    par::cell::CellType cell_type;
    cell_type.name = std::string(gate_json["type"].as_string().c_str());
    boost::json::object size = gate_json["size"].as_object();
    cell_type.size.height =
      static_cast<uliteral_t>(size["height"].as_int64());
    cell_type.size.width =
      static_cast<uliteral_t>(size["width"].as_int64());

    // Generate and add the ports
    for (const boost::json::value& terminal :
      gate_json["terminals"].as_array())
    {
      par::cell::Port port;
      if (terminal.as_object().at("type") == "input") {
        port.type = par::cell::PortType::input;
      } else if (terminal.as_object().at("type") == "output") {
        port.type = par::cell::PortType::output;
      }
      boost::json::object position =
        terminal.as_object().at("position").as_object();
      port.position.x() = position["x"].as_int64();
      port.position.y() = position["y"].as_int64();
      port.position.z() = position["z"].as_int64();
      cell_type.ports.push_back(port);
    }

    // Generate and add the blocks
    for (const boost::json::value& block : gate_json["blocks"].as_array()) {
      mca_parser::Block::properties_type properties;
      for (const boost::json::value& property :
        block.as_object().at("properties").as_array())
      {
        std::string prop_name(
          property.as_object().at("name").as_string().c_str()
        );
        std::string prop_val(
          property.as_object().at("value").as_string().c_str()
        );
        properties[prop_name] = prop_val;
      }
      boost::json::object position =
        block.as_object().at("position").as_object();
      cell_type.blocks.push_back(
        mca_parser::Block(
          Point(
            static_cast<literal_t>(position["x"].as_int64()),
            static_cast<literal_t>(position["y"].as_int64()),
            static_cast<literal_t>(position["z"].as_int64())
          ),
          std::string(block.as_object().at("name").as_string().c_str()),
          properties
        )
      );
    }

    return cell_type;
  }
}
