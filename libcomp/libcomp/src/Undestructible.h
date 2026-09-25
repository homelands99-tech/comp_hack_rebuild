/**
 * @file libcomp/src/Undestructible.h
 * @ingroup libcomp
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Wrapper that constructs an object and never destroys it.
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

#ifndef LIBCOMP_SRC_UNDESTRUCTIBLE_H
#define LIBCOMP_SRC_UNDESTRUCTIBLE_H

// Standard C++11 Includes
#include <new>
#include <type_traits>
#include <utility>

namespace libcomp {

/**
 * Holds an object whose destructor is never run. This is used for global and
 * static objects that must remain valid while other static objects are
 * destroyed at program exit (for example the log or memory manager).
 */
template <typename T>
class Undestructible {
 public:
  template <typename... Args>
  Undestructible(Args&&... args) {
    new (&mStorage) T(std::forward<Args>(args)...);
  }

  Undestructible(const Undestructible&) = delete;
  Undestructible& operator=(const Undestructible&) = delete;

  T* Get() { return reinterpret_cast<T*>(&mStorage); }
  const T* Get() const { return reinterpret_cast<const T*>(&mStorage); }

  T& operator*() { return *Get(); }
  const T& operator*() const { return *Get(); }

  T* operator->() { return Get(); }
  const T* operator->() const { return Get(); }

  operator T&() { return *Get(); }
  operator const T&() const { return *Get(); }

 private:
  typename std::aligned_storage<sizeof(T), alignof(T)>::type mStorage;
};

}  // namespace libcomp

#endif  // LIBCOMP_SRC_UNDESTRUCTIBLE_H
