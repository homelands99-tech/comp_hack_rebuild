# This file is part of COMP_hack.
#
# Copyright (C) 2010-2019 COMP_hack Team <compomega@tutanota.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# The objgen sources in this tree predate the 2022 release, so by default use the
# comp_objgen.exe shipped with that release to generate identical object code.
OPTION(USE_PREBUILT_OBJGEN_EXE "Use deps/objgen/comp_objgen.exe" ON)

IF(USE_PREBUILT_OBJGEN_EXE AND EXISTS "${CMAKE_SOURCE_DIR}/deps/objgen/comp_objgen.exe")
    MESSAGE("-- Using prebuilt comp_objgen.exe")
    ADD_EXECUTABLE(comp_objgen IMPORTED GLOBAL)
    SET_TARGET_PROPERTIES(comp_objgen PROPERTIES IMPORTED_LOCATION
        "${CMAKE_SOURCE_DIR}/deps/objgen/comp_objgen.exe")
ELSE()
    ADD_SUBDIRECTORY(objgen)
ENDIF()
