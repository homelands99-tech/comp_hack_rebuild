# civetweb always adds /WX together with -Wall. Newer Windows SDKs trigger
# informational warnings (such as C4710 for sscanf) under -Wall, which then
# fail the build, so drop /WX after the source is extracted.
# Run with -DSOURCE_DIR=<civetweb source dir>.
SET(_file "${SOURCE_DIR}/CMakeLists.txt")

IF(EXISTS "${_file}")
    FILE(READ "${_file}" _contents)
    STRING(REPLACE "add_c_compiler_flag(/WX)" "" _contents "${_contents}")
    STRING(REPLACE "add_cxx_compiler_flag(/WX)" "" _contents "${_contents}")
    FILE(WRITE "${_file}" "${_contents}")
ENDIF()
