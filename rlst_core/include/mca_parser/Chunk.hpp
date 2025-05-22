/*
 * Copyright (C) 2025 Mattéo Rossillol‑‑Laruelle <beatussum@protonmail.com>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef RLST_CORE_RLST_MCA_PARSER_CHUNK_HPP
#  define RLST_CORE_RLST_MCA_PARSER_CHUNK_HPP

#include <vector>

extern "C" {
  #include <zlib.h>
}

namespace rlst::mca_parser
{
  class Chunk
  {
  public:
    using data_type = std::vector<Bytef>;
    using size_type = data_type::size_type;
  public:
    // compression type is encoded in 1 byte
    static constexpr size_type compression_type_length = 1;

    /*
     * The length of the compressed data + the size of the compression
     * type is encoded in 4 bytes
     */

    static constexpr size_type size_length = 4;
  public:
    Chunk() noexcept
      : m_x(0)
      , m_y(0)
      , m_compressed_data()
      , m_uncompressed_data()
      , m_is_not_generated(false)
    {}

    Chunk(const Chunk& __other) = default;
    Chunk(Chunk&& __other) noexcept = default;
    ~Chunk() = default;
  public:
    Chunk(size_type __x, size_type __y) noexcept
      : m_x(__x)
      , m_y(__y)
      , m_compressed_data()
      , m_uncompressed_data()
      , m_is_not_generated(false)
    {}

    Chunk(
      size_type __x,
      size_type __y,
      data_type __compressed_data,
      data_type __uncompressed_data
    ) noexcept
      : m_x(__x)
      , m_y(__y)
      , m_compressed_data(std::move(__compressed_data))
      , m_uncompressed_data(std::move(__uncompressed_data))
      , m_is_not_generated(false)
    {}

    Chunk(
      size_type __x,
      size_type __y,
      data_type __compressed_data,
      data_type __uncompressed_data,
      bool is_not_generated

    ) noexcept
      : m_x(__x)
      , m_y(__y)
      , m_compressed_data(std::move(__compressed_data))
      , m_uncompressed_data(std::move(__uncompressed_data))
      , m_is_not_generated(is_not_generated)
    {}

  public:
    Chunk& operator=(const Chunk& __rhs) = default;
    Chunk& operator=(Chunk&& __rhs) noexcept = default;
  public:
    size_type x() const noexcept { return m_x; }
    size_type y() const noexcept { return m_y; }

    const data_type& compressed_data() const noexcept
      { return m_compressed_data; }

    data_type& compressed_data() noexcept { return m_compressed_data; }

    const data_type& uncompressed_data() const noexcept
      { return m_uncompressed_data; }

    data_type& uncompressed_data() noexcept { return m_uncompressed_data; }
    bool is_not_generated() const noexcept { return m_is_not_generated; }
  private:
    size_type m_x;
    size_type m_y;
    data_type m_compressed_data;
    data_type m_uncompressed_data;

    bool m_is_not_generated;
  };
}

#endif // RLST_CORE_RLST_MCA_PARSER_CHUNK_HPP
