#ifndef RLST_RLST_SVG_SVG
#  define RLST_RLST_SVG_SVG

#include "par/cell/Cell.hpp"
#include "par/geometry.hpp"

#include <fstream>

/**
 * @file
 *
 * The SVG generation class.
 *
 */

namespace rlst::svg::svg
{
  /**
   * An SVG generator
   */

  class SVG
  {
  public:
    SVG(const char* __file_name);

    ~SVG();

    /**
     * Draw a @ref rlst::par::cell::Cell object
     *
     * @param[in] __cell The cell to draw
     */
    void draw_cell(const rlst::par::cell::Cell& __cell);

    /**
     * Draw a @ref rlst::par::Segment object
     *
     * @param[in] __segment The segment to draw
     * @param[in] __color The color to use
     */
    void draw_segment(
      const rlst::par::Segment& __segment,
      const std::string_view& __color
    );

  private:
    std::ofstream m_svg_file; ///< The SVG file that is being written to
  };
}

#endif // RLST_RLST_SVG_SVG
