# Fixes applied to the comp_hack MariaDB Connector/C snapshot after it is
# extracted, so it builds with CMake 3.x and Ninja.
# Run with -DSOURCE_DIR=<mariadb source dir>.

# cmake/ConnectorName.cmake closes an IF() with END(); old CMake accepted this
# but CMake 3.x rejects it ("Flow control statements are not properly nested").
SET(_file "${SOURCE_DIR}/cmake/ConnectorName.cmake")

IF(EXISTS "${_file}")
    FILE(READ "${_file}" _contents)
    STRING(REPLACE "  END()" "  ENDIF()" _contents "${_contents}")
    FILE(WRITE "${_file}" "${_contents}")
ENDIF()

# The RelWithDebInfo .pdb is installed from a Visual Studio generator path
# (libmariadb/RelWIthDebInfo/...) that does not exist with Ninja; make the
# install of that debug file optional.
SET(_file "${SOURCE_DIR}/libmariadb/CMakeLists.txt")

IF(EXISTS "${_file}")
    FILE(READ "${_file}" _contents)
    STRING(REPLACE "           COMPONENT Development)" "           COMPONENT Development OPTIONAL)" _contents "${_contents}")
    FILE(WRITE "${_file}" "${_contents}")
ENDIF()
