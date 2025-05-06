#include "svg/SVG.hpp"

static constexpr char CELL_TEXT_COLOR[] = "black";
static constexpr char CELL_BACKGROUND_COLOR[] = "red";
static constexpr char PORT_BACKGROUND_COLOR[] = "blue";
static constexpr int SIZE_FACTOR = 10;
static constexpr int CANVAS_WIDTH = 1024 * SIZE_FACTOR;
static constexpr int CANVAS_HEIGHT = 1024 * SIZE_FACTOR;

namespace rlst::svg::svg
{
  SVG::SVG(const char* __file_name)
    : m_svg_file(__file_name)
  {
    m_svg_file
      << "<svg width='" << CANVAS_WIDTH << "' height='" << CANVAS_HEIGHT
      << "' xmlns='http://www.w3.org/2000/svg'>\n";
  }

  SVG::~SVG()
  {
    m_svg_file << "</svg>\n";
    m_svg_file.close();
  }

  void SVG::draw_cell(const rlst::par::cell::Cell& __cell)
  {
    // assumes that the cell is a gate since modules
    // don't exist as of writing this code
    m_svg_file
      << "<rect"
      << " width='" << __cell.type->size.width * SIZE_FACTOR << "'"
      << " height='" << __cell.type->size.height * SIZE_FACTOR << "'"
      << " x='" << __cell.position.x() * SIZE_FACTOR << "'"
      << " y='" << __cell.position.y() * SIZE_FACTOR << "'"
      << " fill='" << CELL_BACKGROUND_COLOR << "'"
      << "/>\n";
    m_svg_file
      << "<text"
      << " x='" << (__cell.position.x() + 1) * SIZE_FACTOR << "'"
      << " y='"

      << (__cell.position.y() + __cell.type->size.height - 1) * SIZE_FACTOR
        << "'"

      << " fill='" << CELL_TEXT_COLOR << "'"
      << ">" << __cell.type->name << "</text>\n";
    for (par::cell::Port port : __cell.type->ports) {
      par::Point port_point;
      port_point.x() = __cell.position.x() + port.position.x();
      port_point.y() = __cell.position.y() + port.position.y();
      draw_point(port_point, PORT_BACKGROUND_COLOR);
    }
  }

  void SVG::draw_point(
    const rlst::par::Point& __point,
    const std::string_view& __color)
  {
    m_svg_file
      << "<rect"
      << " width='" << SIZE_FACTOR << "'"
      << " height='" << SIZE_FACTOR << "'"
      << " x='" << __point.x() * SIZE_FACTOR
        << "'"
      << " y='" << __point.y() * SIZE_FACTOR
        << "'"
      << " fill='" << __color << "'"
      << "/>\n";
  }
}
