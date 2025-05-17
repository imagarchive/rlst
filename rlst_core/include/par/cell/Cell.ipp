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


namespace rlst::par::cell
{
  constexpr bool Cell::can_be_moved_to(
    Point __to,
    const Size& __frame
  ) const noexcept
  {
    if (__to.is_outside(__frame)) {
      return false;
    } else {
      __to.x() += __frame.width;
      __to.y() += __frame.height;

      return __to.is_outside(__frame);
    }
  }
}
