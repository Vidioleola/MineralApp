cmake_minimum_required(VERSION 3.5)

if(WIN32)
include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
endif()

project(MineralApp)

set(CMAKE_CXX_STANDARD 17)

SET(INCLUDES
${CMAKE_CURRENT_SOURCE_DIR}/src
)

SET(SRC
${CMAKE_CURRENT_SOURCE_DIR}/src/addmodframe.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/app.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/genreportframe.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/mainframe.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/mineraldb.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/translation.cpp
)

find_package(wxWidgets REQUIRED gl core base richtext OPTIONAL_COMPONENTS net)
include(${wxWidgets_USE_FILE})
if(WIN32)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
else()
find_package (SQLite3 REQUIRED)
endif()
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)

#https://stackoverflow.com/questions/47066115/cmake-get-version-from-multiline-text-file
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/version.in ver)

string(REGEX MATCH "VERSION_MAJOR = ([0-9]*)" _ ${ver})
set(ver_major ${CMAKE_MATCH_1})

string(REGEX MATCH "VERSION_MINOR = ([0-9]*)" _ ${ver})
set(ver_minor ${CMAKE_MATCH_1})

string(REGEX MATCH "VERSION_PATCH = ([0-9]*)" _ ${ver})
set(ver_patch ${CMAKE_MATCH_1})

string(REGEX MATCH "VERSION = ([0-9]*)" _ ${ver})
set(ver_ver ${CMAKE_MATCH_1})

message("version: ${ver_major}.${ver_minor}.${ver_patch}")

if(WIN32)
add_executable(MineralApp WIN32 ${SRC})
else()
add_executable(MineralApp ${SRC})
endif()
target_include_directories(MineralApp PRIVATE ${INCLUDES} ${SQLITE3_INCLUDE_DIRS} ${wxWidgets_INCLUDE_DIRS} ${JPEG_INCLUDE_DIR} ${SQLITE3_INCLUDE_DIRS})
target_compile_options(MineralApp PRIVATE ${wxWidgets_CXX_FLAGS} --std=c++17 -Wall -O3 -DVERSION_MAJOR="${ver_major}" -DVERSION_MINOR="${ver_minor}" -DVERSION_PATCH="${ver_patch}" -DVERSION="${ver_ver}")
if(WIN32)
target_link_libraries(MineralApp ${wxWidgets_LIBRARIES} PNG::PNG ${JPEG_LIBRARIES} unofficial::sqlite3::sqlite3)
else()
target_link_libraries(MineralApp ${wxWidgets_LIBRARIES} PNG::PNG ${JPEG_LIBRARIES} sqlite3)
endif()
