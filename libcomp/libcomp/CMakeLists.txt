# This file is part of COMP_hack.
#
# Copyright (C) 2010-2020 COMP_hack Team <compomega@tutanota.com>
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

# Reconstructed: the file list matches the libcomp objects linked into the
# 2022 comp_channel / comp_world / comp_lobby release builds (from their PDBs).

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

PROJECT(libcomp)

FIND_PACKAGE(Threads REQUIRED)

MESSAGE("** Configuring ${PROJECT_NAME} **")

# Add a target to generate API documentation.
FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/api/libcomp)
GENERATE_DOCS(doc/Doxyfile.in)

# Add a directory to put the objgen output into.
FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/objgen)

SET(SYSTEM_LIBRARIES "")

IF(BSD)
    FIND_PACKAGE(ExecInfo REQUIRED)

    SET(SYSTEM_LIBRARIES ${SYSTEM_LIBRARIES} ${EXECINFO_LIBRARY})
ENDIF(BSD)

SET(${PROJECT_NAME}_SRCS
    src/ArgumentParser.cpp
    src/BaseConfig.cpp
    src/BaseLog.cpp
    src/BaseScriptEngine.cpp
    src/BaseServer.cpp
    src/CString.cpp
    src/Compress.cpp
    src/ConnectionMessage.cpp
    src/Convert.cpp
    src/Crypto.cpp
    src/DataFile.cpp
    src/DataStore.cpp
    src/DataSyncManager.cpp
    src/Database.cpp
    src/DatabaseBind.cpp
    src/DatabaseChangeSet.cpp
    src/DatabaseMariaDB.cpp
    src/DatabaseQuery.cpp
    src/DatabaseQueryMariaDB.cpp
    src/DatabaseQuerySQLite3.cpp
    src/DatabaseSQLite3.cpp
    src/EncryptedConnection.cpp
    src/Exception.cpp
    src/InternalConnection.cpp
    src/ManagerPacket.cpp
    src/MemoryManager.cpp
    src/Message.cpp
    src/MessageConnectionClosed.cpp
    src/MessageEncrypted.cpp
    src/MessageInit.cpp
    src/MessagePacket.cpp
    src/MessagePong.cpp
    src/MessageShutdown.cpp
    src/Mutex.cpp
    src/Object.cpp
    src/Packet.cpp
    src/PacketException.cpp
    src/PersistentObject.cpp
    src/Randomizer.cpp
    src/ReadOnlyPacket.cpp
    src/ServerCommandLineParser.cpp
    src/Shutdown.cpp
    src/SqratInt64.cpp
    src/TcpConnection.cpp
    src/TcpServer.cpp
    src/TimerManager.cpp
    src/Worker.cpp
    src/rbtree.c
)

IF(WIN32)
    SET(${PROJECT_NAME}_SRCS ${${PROJECT_NAME}_SRCS}
        src/PlatformWindows.cpp
        src/WindowsService.cpp
        src/WindowsServiceMain.cpp
    )
ENDIF(WIN32)

# All header files (listed so they show up in IDE projects).
FILE(GLOB ${PROJECT_NAME}_HDRS src/*.h)
SET(${PROJECT_NAME}_SCHEMA
    schema/databaseconfig.xml
    schema/databaseconfig_mariadb.xml
    schema/databaseconfig_sqlite3.xml
    schema/server_command_line.xml
    schema/serverconfig.xml
)

SOURCE_GROUP("objgen" ${CMAKE_CURRENT_BINARY_DIR}/objgen/*)

OBJGEN_XML(${PROJECT_NAME}_STRUCTS
    # Main schema
    schema/libcomp-master.xml

    # Included schemas
    ${${PROJECT_NAME}_SCHEMA}

    # Include paths
    schema

    # Output files
    DatabaseConfig.h
    DatabaseConfig.cpp
    DatabaseConfigMariaDB.h
    DatabaseConfigMariaDB.cpp
    DatabaseConfigSQLite3.h
    DatabaseConfigSQLite3.cpp
    ServerCommandLine.h
    ServerCommandLine.cpp
    ServerConfig.h
    ServerConfig.cpp
)

COVERALLS_SOURCES(${${PROJECT_NAME}_SRCS})

ADD_LIBRARY(comp STATIC ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS}
    ${${PROJECT_NAME}_STRUCTS})

TARGET_COMPILE_DEFINITIONS(comp PUBLIC ${SQRAT_DEFINES})

SET_TARGET_PROPERTIES(comp PROPERTIES FOLDER "Libraries")

IF(SYSTEMD_FOUND)
    SET(SYSTEMD_INCLUDES ${SYSTEMD_INCLUDE_DIRS})
    SET(SYSTEMD_LIBS ${SYSTEMD_LIBRARIES})
ENDIF(SYSTEMD_FOUND)

TARGET_INCLUDE_DIRECTORIES(comp PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${LIBCOMP_INCLUDES}
    ${ASIO_INCLUDE_DIRS}
    ${SYSTEMD_INCLUDES}
    ${SQRAT_INCLUDE_DIRS}
    ${GSL_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(comp mariadbclient ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARIES}
    ${SYSTEM_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} objgen sqlite3
    squirrel sqstdlib ttvfs ttvfs_zip tinyxml2 physfs ${SYSTEMD_LIBS})

ADD_DEPENDENCIES(comp asio gsl)

IF(WIN32)
    TARGET_LINK_LIBRARIES(comp shlwapi advapi32 iphlpapi psapi shell32
        userenv ws2_32 dbghelp)
ENDIF(WIN32)

IF(USE_COTIRE)
    cotire(comp)
ENDIF(USE_COTIRE)
