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


namespace rlst
{
  inline namespace core
  {
    template <class Iterator>
    Iterator choice(Iterator __begin, Iterator __end)
    {
      using difference_type =
        typename std::iterator_traits<Iterator>::difference_type;

      difference_type n = std::distance(__begin, __end);

      std::random_device device;
      std::mt19937 generator(device());
      std::uniform_int_distribution<difference_type> distribution(0, n - 1);

      return std::advance(__begin, distribution());
    }
  }
}
