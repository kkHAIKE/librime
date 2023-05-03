set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/c_flag_overrides.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cxx_flag_overrides.cmake)

project(rime)
cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS -Wno-deprecated-declarations)

set(rime_version 1.8.5)
set(rime_soversion 1)

add_definitions(-DRIME_VERSION="${rime_version}")

include(GNUInstallDirs)

option(BUILD_SHARED_LIBS "Build Rime as shared library" ON)
option(BUILD_MERGED_PLUGINS "Merge plugins into one Rime library" ON)
option(BUILD_STATIC "Build with dependencies as static libraries" OFF)
option(BUILD_DATA "Build data for Rime" OFF)
option(BUILD_SAMPLE "Build sample Rime plugin" OFF)
option(BUILD_TEST "Build and run tests" ON)
option(BUILD_SEPARATE_LIBS "Build separate rime-* libraries" OFF)
option(ENABLE_LOGGING "Enable logging with google-glog library" ON)
option(ENABLE_ASAN "Enable Address Sanitizer (Unix Only)" OFF)
option(INSTALL_PRIVATE_HEADERS "Install private headers (usually needed for externally built Rime plugins)" OFF)
option(ENABLE_EXTERNAL_PLUGINS "Enable loading of externally built Rime plugins (from directory set by RIME_PLUGINS_DIR variable)" OFF)

set(RIME_DATA_DIR "${CMAKE_INSTALL_FULL_DATADIR}/rime-data" CACHE STRING "Target directory for Rime data")
set(RIME_PLUGINS_DIR "${CMAKE_INSTALL_FULL_LIBDIR}/rime-plugins" CACHE STRING "Target directory for externally built Rime plugins")

if(WIN32)
  set(ext ".exe")
endif(WIN32)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${PROJECT_SOURCE_DIR}")

if (ENABLE_ASAN)
  set(asan_cflags "-fsanitize=address -fno-omit-frame-pointer")
  set(asan_lflags "-fsanitize=address -lasan")
  set(CMAKE_C_FLAGS "${asan_cflags} ${CMAKE_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${asan_cflags} ${CMAKE_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${asan_lflags} ${CMAKE_EXE_LINKER_FLAGS}")
  set(CMAKE_SHARED_LINKER_FLAGS "${asan_lflags} ${CMAKE_SHARED_LINKER_FLAGS}")
endif()

set(Gflags_STATIC ${BUILD_STATIC})
set(Glog_STATIC ${BUILD_STATIC})
set(LevelDb_STATIC ${BUILD_STATIC})
set(Marisa_STATIC ${BUILD_STATIC})
set(Opencc_STATIC ${BUILD_STATIC})
set(YamlCpp_STATIC ${BUILD_STATIC})

include(FetchContent)

if(ENABLE_LOGGING)

  # find_package(Gflags)
  # if(Gflags_FOUND)
  #   include_directories(${Gflags_INCLUDE_PATH})
  # endif()

  # set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
  set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
  set(WITH_GFLAGS OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(
    glog
    URL https://github.com/google/glog/archive/refs/tags/v0.6.0.tar.gz
    URL_HASH SHA256=8a83bf982f37bb70825df71a9709fa90ea9f4447fb3c099e1d720a439d88bad6
    # SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/glog
  )

  # if(Glog_STATIC)
  #   add_definitions(-DGOOGLE_GLOG_DLL_DECL=)
  # endif()

  set(RIME_ENABLE_LOGGING 1)

endif()

find_package(Threads)

if(BUILD_TEST)

  set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.tar.gz
    URL_HASH SHA256=ad7fdba11ea011c1d925b3289cf4af2c66a352e18d4c7264392fead75e919363
    # SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/googletest
  )

    enable_testing()

endif()

set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  yaml-cpp
  URL https://github.com/jbeder/yaml-cpp/archive/refs/tags/yaml-cpp-0.7.0.tar.gz
  URL_HASH SHA256=43e6a9fcb146ad871515f0d0873947e5d497a1c9c60c58cb102a97b47208b7c3
  # SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/yaml-cpp
)

set(LEVELDB_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LEVELDB_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  leveldb
  URL https://github.com/google/leveldb/archive/7a2f64ed504f510425183fa225ae80c671f0145f.tar.gz
  URL_HASH SHA256=34f298843050115c51577e7485567def72b288599a24f30624786326ff004b6f
  # SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/leveldb
)

add_subdirectory(deps) # opencc contains
# find_package(Marisa REQUIRED)
# if(Marisa_FOUND)
#   include_directories(${Marisa_INCLUDE_PATH})
# endif()

FetchContent_Declare(
  opencc
  URL https://github.com/BYVoid/OpenCC/archive/refs/tags/ver.1.1.6.tar.gz
  URL_HASH SHA256=169bff4071ffe814dc16df7d180ff6610db418f4816e9c0ce02cf874bdf058df
)

FetchContent_Declare(
  sigslot
  URL https://github.com/palacaze/sigslot/archive/refs/tags/v1.2.2.tar.gz
  URL_HASH SHA256=d2dd0f91c13dbec1026aa5f393e511db863a8c32146c68c10888a9c193458437
)

FetchContent_Declare(
  fmt
  URL https://github.com/fmtlib/fmt/archive/refs/tags/9.1.0.tar.gz
  URL_HASH SHA256=5dea48d1fcddc3ec571ce2058e13910a0d4a6bab4cc09a809d8b1dd1c88ae6f2
)

FetchContent_MakeAvailable(glog googletest yaml-cpp leveldb opencc sigslot fmt)

add_library(libopencc_header INTERFACE)
target_compile_definitions(libopencc_header INTERFACE
  Opencc_BUILT_AS_STATIC)
target_include_directories(libopencc_header INTERFACE
  ${opencc_SOURCE_DIR}/src
  ${opencc_BINARY_DIR}/src
)

find_package(ZLIB REQUIRED)

# if(Opencc_STATIC)
#   add_definitions(-DOpencc_BUILT_AS_STATIC)
# endif()

find_path(X11Keysym X11/keysym.h)
if(X11Keysym)
  message(STATUS "Found X11/keysym.h at ${X11Keysym}")
  include_directories(${X11Keysym})
else()
  message(WARNING "X11/keysym.h not found.")
endif()

configure_file(
  "${PROJECT_SOURCE_DIR}/src/rime/build_config.h.in"
  "${PROJECT_BINARY_DIR}/src/rime/build_config.h")

include_directories(${PROJECT_BINARY_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

if(MSVC)
  # https://stackoverflow.com/a/31264946
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
  set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
  if(MSVC10)
    # stdint.h is missing from VS2010
    include_directories(${PROJECT_SOURCE_DIR}/include/msvc)
  endif()
  # Force Boost.Uuid to use wincrypt as random provider instead of bcrypt for Windows XP compatibility
  add_definitions("/wd4244 /wd4996 /DBOOST_UUID_RANDOM_PROVIDER_FORCE_WINCRYPT")
endif()

# if(UNIX)
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
# endif()

if (NOT CMAKE_BUILD_PARALLEL_LEVEL)
  include(ProcessorCount)
  ProcessorCount(N)
  if (NOT N EQUAL 0)
    set(CMAKE_BUILD_PARALLEL_LEVEL ${N})
  endif()
endif()

# keep these variables lest some Rime plugin's cmake file is still using them {
if(NOT DEFINED LIB_INSTALL_DIR)
    set(LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
endif()

if(NOT DEFINED BIN_INSTALL_DIR)
    set(BIN_INSTALL_DIR ${CMAKE_INSTALL_BINDIR})
endif()
# }

# uninstall target
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
add_custom_target(uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux|FreeBSD|DragonFly|GNU" OR MINGW)
  set(prefix "${CMAKE_INSTALL_PREFIX}")
  set(exec_prefix "${CMAKE_INSTALL_PREFIX}")
  set(bindir "${CMAKE_INSTALL_FULL_BINDIR}")
  set(libdir "${CMAKE_INSTALL_FULL_LIBDIR}")
  set(pkgdatadir "${RIME_DATA_DIR}")
  set(pluginsdir "${RIME_PLUGINS_DIR}")
  set(includedir "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
  configure_file(
      ${PROJECT_SOURCE_DIR}/rime.pc.in
      ${PROJECT_BINARY_DIR}/rime.pc
      @ONLY)
  install(FILES ${PROJECT_BINARY_DIR}/rime.pc
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig)
endif()

install(FILES cmake/RimeConfig.cmake
  DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/cmake/rime)

file(GLOB rime_public_header_files ${PROJECT_SOURCE_DIR}/src/*.h)
install(FILES ${rime_public_header_files}
  DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR})
if(INSTALL_PRIVATE_HEADERS)
  file(GLOB rime_private_header_files
    ${PROJECT_SOURCE_DIR}/src/rime/*.h
    ${PROJECT_BINARY_DIR}/src/rime/*.h)
  install(FILES ${rime_private_header_files}
    DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/rime)
  foreach(rime_private_header_files_dir algo config dict gear lever)
    file(GLOB rime_private_header_files
      ${PROJECT_SOURCE_DIR}/src/rime/${rime_private_header_files_dir}/*.h)
    install(FILES ${rime_private_header_files}
      DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/rime/${rime_private_header_files_dir})
  endforeach()
endif()

if(BUILD_DATA)
  file(GLOB rime_preset_data_files ${PROJECT_SOURCE_DIR}/data/preset/*.yaml)
  install(FILES ${rime_preset_data_files} DESTINATION ${RIME_DATA_DIR})
endif()

if(BUILD_SHARED_LIBS)
  add_definitions(-DRIME_BUILD_SHARED_LIBS)
  set(rime_library rime)
  if(BUILD_SEPARATE_LIBS)
    set(rime_dict_library rime-dict)
    set(rime_gears_library rime-gears)
    set(rime_levers_library rime-levers)
  endif()
else()
  set(rime_library rime-static)
endif()

add_subdirectory(plugins)
message(STATUS "rime_plugins_libs: ${rime_plugins_deps}")
message(STATUS "rime_plugins_modules: ${rime_plugins_modules}")
set(list "")
foreach(mod ${rime_plugins_modules})
  set(list "${list},Q(${mod})")
endforeach()
add_definitions(-DRIME_EXTRA_MODULES=${list})
if(BUILD_SHARED_LIBS AND BUILD_SEPARATE_LIBS AND rime_plugins_objs)
  set(rime_plugins_library rime-plugins)
endif()

add_subdirectory(src)

if(BUILD_SHARED_LIBS)
  add_subdirectory(tools)

  if (BUILD_SAMPLE)
    add_subdirectory(sample)
  endif()
endif()

if(BUILD_TEST)
  add_subdirectory(test)
endif()
