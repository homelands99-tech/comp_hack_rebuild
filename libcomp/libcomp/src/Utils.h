/**
 * @file libcomp/src/Utils.h
 * @ingroup libcomp
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Miscellaneous utility functions.
 *
 * This file is part of the COMP_hack Library (libcomp).
 *
 * Copyright (C) 2012-2020 COMP_hack Team <compomega@tutanota.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBCOMP_SRC_UTILS_H
#define LIBCOMP_SRC_UTILS_H

// Standard C++11 Includes
#include <algorithm>
#include <iterator>
#include <set>

/**
 * Compare a new set of values against an old set of values.
 * @param newSet Current set of values.
 * @param oldSet Previous set of values.
 * @param additions Set to add values that are in the new set but not the
 *  old set to.
 * @param removals Set to add values that are in the old set but not the
 *  new set to.
 */
template <typename T>
void set_diff(const std::set<T>& newSet, const std::set<T>& oldSet,
              std::set<T>& additions, std::set<T>& removals) {
  std::set_difference(newSet.begin(), newSet.end(), oldSet.begin(),
                      oldSet.end(),
                      std::inserter(additions, additions.begin()));
  std::set_difference(oldSet.begin(), oldSet.end(), newSet.begin(),
                      newSet.end(), std::inserter(removals, removals.begin()));
}

#endif  // LIBCOMP_SRC_UTILS_H
