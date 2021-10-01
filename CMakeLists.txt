project(stinfopp LANGUAGES CXX)

# In response to CMake 3.0 generating warnings regarding policy CMP0042,
# the OSX RPATH settings have been updated per recommendations found
# in the CMake Wiki:
#  http://www.cmake.org/Wiki/CMake_RPATH_handling#Mac_OS_X_and_the_RPATH
if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW) # MACOSX_RPATH
    set(CMAKE_MACOSX_RPATH TRUE)
endif()

if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW) # CMake option behaviour
endif()

if(WIN32)
    # cmake 3.4 is required for CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS
    cmake_minimum_required(VERSION 3.4)

    # c++ 11 support from cmake 3.4 or newer
    set(CMAKE_CXX_STANDARD 11) # C++11...
    set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
    set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
  else()
    if (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} VERSION_LESS 3.1)
        cmake_minimum_required(VERSION 2.8)
        # c++11 support for cmake 2.8.12 - 3.0.x
        #
        # for non-windows platform we try to keep cmake 2.8 support
        # since entreprise distribution tends to have 2.8 version.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    else()
        # c++ 11 support from cmake 3.1 or newer
        set(CMAKE_CXX_STANDARD 11) # C++11...
        set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
        set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
    endif()
endif()

###########################################################################
### cURL dependency search
message(STATUS "Looking for CURL")
find_package(CURL REQUIRED)

if(CURL_FOUND)
  message(STATUS "Found CURL version: ${CURL_VERSION_STRING}")
  message(STATUS "Using CURL include dir(s): ${CURL_INCLUDE_DIRS}")
  message(STATUS "Using CURL lib(s): ${CURL_LIBRARIES}")
else()
  message(ERRORS "Missing CURL")
endif()

if (NOT EXISTS "${CURL_INCLUDE_DIRS}/curl")
  message(FATAL_ERROR "cURL include dirs not found!")
endif()

###########################################################################
### Load dependencies
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CURL_INCLUDE_DIRS}
  ${LIBTIDY_ROOT_DIR}/include
)

###########################################################################
### Target definitions
file(GLOB_RECURSE STINFOPP_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*")
file(GLOB_RECURSE STINFOPP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*")

option(STINFO_BUILD_SHARED "Build shared library of stinfopp" ON)
if(STINFO_BUILD_SHARED)
  add_library(${PROJECT_NAME} SHARED
    ${STINFOPP_HEADERS}
    ${STINFOPP_SOURCES}
  )

  target_compile_definitions(stinfopp PRIVATE STINFOPP_LIBRARY)

  target_link_libraries(${PROJECT_NAME} tidy-static ${CURL_LIBRARIES})
endif()

add_library(${PROJECT_NAME}_static STATIC
  ${STINFOPP_HEADERS}
  ${STINFOPP_SOURCES}
)

set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}_static PROPERTIES PREFIX "lib")

target_compile_definitions(${PROJECT_NAME}_static PRIVATE STINFOPPSTATIC_LIBRARY)

target_link_libraries(${PROJECT_NAME}_static tidy-static ${CURL_LIBRARIES})
