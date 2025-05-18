#ifndef RLST_RLST_CELL_READER_CELL_READING_HPP
#  define RLST_RLST_CELL_READER_CELL_READING_HPP

/**
 * This namespace describes all utilities related to generating the blocks for
 * gates from their json files
 */

#include "mca_parser/Block.hpp"
#include "par/cell/types.hpp"

#include <vector>

namespace rlst::gate_reader
{
  /**
   * Given a json file describing a logic gate, create and return its
   * @ref par::cell::CellType
   *
   * @param[in] __nets The name of the json file
   * pairs
   *
   * @return The created @ref par::cell::CellType
   */

    par::cell::CellType generate_gate(const std::string __file_name);
}

#endif // RLST_RLST_CELL_READER_CELL_READING_HPP
