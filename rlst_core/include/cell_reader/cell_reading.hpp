#ifndef RLST_RLST_CELL_READER_CELL_READING_HPP
#  define RLST_RLST_CELL_READER_CELL_READING_HPP

/**
 * This namespace describes all utilities related to generating a list of
 * @CellType from json files
 */

#include "mca_parser/Block.hpp"
#include "par/cell/types.hpp"

namespace rlst::gate_reader
{
  /**
   * Given a directory containing json files describing logic gates, create and
   * return matching @ref par::cell::CellType
   *
   * @param[in] __path The path of the the directory containing the files
   *
   * @return The created @ref par::cell::CellType
   */

  std::list<par::cell::CellType> generate_gates(const std::string& __path);
}

#endif // RLST_RLST_CELL_READER_CELL_READING_HPP
