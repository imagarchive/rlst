#include "pTypes.hpp"

std::list<par::cell::CellType> initCellTypes() {
  std::list<par::cell::CellType> CellTypes = {};

  // NOT CellType
  par::cell::Port NotInput1 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port NotOutput1 = {par::Point(), par::cell::PortType(output)};

  std::list<par::cell::Port> NotPorts = {NotInput1, NotOutput1};
  std::string NotName = "NOT";

  par::cell::CellType NotCellType = {NotPorts, NotName, par::Size()};
  CellTypes.push_front(NotCellType);

  // AND CellType
  par::cell::Port AndInput1 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port AndInput2 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port AndOutput1 = {par::Point(), par::cell::PortType(output)};

  std::list<par::cell::Port> AndPorts = {AndInput1, AndInput2, AndOutput1};
  std::string AndName = "AND";

  par::cell::CellType AndCellType = {AndPorts, AndName, par::Size()};
  CellTypes.push_front(AndCellType);

  // OR CellType
  par::cell::Port OrInput1 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port OrInput2 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port OrOutput1 = {par::Point(), par::cell::PortType(output)};

  std::list<par::cell::Port> OrPorts = {OrInput1, OrInput2, OrOutput1};
  std::string OrName = "OR";

  par::cell::CellType OrCellType = {OrPorts, OrName, par::Size()};
  CellTypes.push_front(OrCellType);

  // XOR CellType
  par::cell::Port XorInput1 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port XorInput2 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port XorOutput1 = {par::Point(), par::cell::PortType(output)};

  std::list<par::cell::Port> XorPorts = {XorInput1, XorInput2, XorOutput1};
  std::string XorName = "XOR";

  par::cell::CellType XorCellType = {XorPorts, XorName, par::Size()};
  CellTypes.push_front(XorCellType);

  // XNOR CellType
  par::cell::Port XnorInput1 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port XnorInput2 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port XnorOutput1 = {par::Point(), par::cell::PortType(output)};

  std::list<par::cell::Port> XnorPorts = {XnorInput1, XnorInput2, XnorOutput1};
  std::string XnorName = "XNOR";

  par::cell::CellType XnorCellType = {XnorPorts, XnorName, par::Size()};
  CellTypes.push_front(XnorCellType);

  // NAND CellType
  par::cell::Port NandInput1 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port NandInput2 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port NandOutput1 = {par::Point(), par::cell::PortType(output)};

  std::list<par::cell::Port> NandPorts = {NandInput1, NandInput2, NandOutput1};
  std::string NandName = "NAND";

  par::cell::CellType NandCellType = {NandPorts, NandName, par::Size()};
  CellTypes.push_front(NandCellType);

  // NOR CellType
  par::cell::Port NorInput1 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port NorInput2 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port NorOutput1 = {par::Point(), par::cell::PortType(output)};

  std::list<par::cell::Port> NorPorts = {NorInput1, NorInput2, NorOutput1};
  std::string NorName = "NOR";

  par::cell::CellType NorCellType = {NorPorts, NorName, par::Size()};
  CellTypes.push_front(NorCellType);

  // MUX CellType
  par::cell::Port MuxInput1 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port MuxInput2 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port MuxOutput1 = {par::Point(), par::cell::PortType(output)};

  std::list<par::cell::Port> MuxPorts = {MuxInput1, MuxInput2, MuxOutput1};
  std::string MuxName = "MUX";

  par::cell::CellType MuxCellType = {MuxPorts, MuxName, par::Size()};
  CellTypes.push_front(MuxCellType);

  // OAI3 CellType
  par::cell::Port Oai3Input1 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port Oai3Input2 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port Oai3Input3 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port Oai3Output1 = {par::Point(), par::cell::PortType(output)};

  std::list<par::cell::Port> Oai3Ports = {Oai3Input1, Oai3Input2, Oai3Input3,
                                          Oai3Output1};
  std::string Oai3Name = "OAI3";

  par::cell::CellType Oai3CellType = {Oai3Ports, Oai3Name, par::Size()};
  CellTypes.push_front(Oai3CellType);

  // AOI3 CellType
  par::cell::Port Aoi3Input1 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port Aoi3Input2 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port Aoi3Input3 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port Aoi3Output1 = {par::Point(), par::cell::PortType(output)};

  std::list<par::cell::Port> Aoi3Ports = {Aoi3Input1, Aoi3Input2, Aoi3Input3,
                                          Aoi3Output1};
  std::string Aoi3Name = "AOI3";

  par::cell::CellType Aoi3CellType = {Aoi3Ports, Aoi3Name, par::Size()};
  CellTypes.push_front(Aoi3CellType);

  // OAI4 CellType
  par::cell::Port Oai4Input1 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port Oai4Input2 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port Oai4Input3 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port Oai4Input4 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port Oai4Output1 = {par::Point(), par::cell::PortType(output)};

  std::list<par::cell::Port> Oai4Ports = {Oai4Input1, Oai4Input2, Oai4Input3,
                                          Oai4Input4, Oai4Output1};
  std::string Oai4Name = "OAI4";

  par::cell::CellType Oai4CellType = {Oai4Ports, Oai4Name, par::Size()};
  CellTypes.push_front(Oai4CellType);

  // AOI4 CellType
  par::cell::Port Aoi4Input1 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port Aoi4Input2 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port Aoi4Input3 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port Aoi4Input4 = {par::Point(), par::cell::PortType(input)};
  par::cell::Port Aoi4Output1 = {par::Point(), par::cell::PortType(output)};

  std::list<par::cell::Port> Aoi4Ports = {Aoi4Input1, Aoi4Input2, Aoi4Input3,
                                          Aoi4Input4, Aoi4Output1};
  std::string Aoi4Name = "AOI4";

  par::cell::CellType Aoi4CellType = {Aoi4Ports, Aoi4Name, par::Size()};
  CellTypes.push_front(Aoi4CellType);

  // DFFE_PP CellType

  return CellTypes;
}

