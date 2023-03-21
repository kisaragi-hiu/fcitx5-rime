cmake_minimum_required(VERSION 3.4.0)

project(fcitx5-{{{lowername}}} VERSION 5.0.16)

find_package(ECM 1.0.0 REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(FeatureSummary)
include(GNUInstallDirs)
include(ECMSetupVersion)
# include(ECMUninstallTarget)

find_package(Gettext REQUIRED)
find_package(Fcitx5Core 5.0.6 REQUIRED)
find_package(Fcitx5Module REQUIRED COMPONENTS Notifications DBus)
# find_package(PkgConfig REQUIRED)

# set(CMAKE_FIND_LIBRARY_PREFIXES "${CMAKE_STATIC_LIBRARY_PREFIX}")
# set(CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_STATIC_LIBRARY_SUFFIX}")
# list(APPEND PKG_CONFIG_EXECUTABLE "--static")

# pkg_check_modules(Rime REQUIRED IMPORTED_TARGET "rime>=1.0.0")
include(FetchContent)
FetchContent_Declare(rime
  GIT_REPOSITORY https://github.com/rime/librime.git
  GIT_TAG 08dd95f5d9282346f0d4a3e8fc6b20811dc3d063 # 1.8.5
)
FetchContent_Populate(rime)
# Using FetchContent_Declare together with CMAKE_ARGS
# https://stackoverflow.com/questions/62101576/
set(BUILD_STATIC ON CACHE INTERNAL "Build static")
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Don't build shared")
add_subdirectory(${rime_SOURCE_DIR} ${rime_BINARY_DIR})

# list(POP_BACK PKG_CONFIG_EXECUTABLE)

if ("${Rime_VERSION}" VERSION_LESS "1.7.0")
    add_definitions(-DFCITX_RIME_LOAD_PLUGIN)
endif()

if(NOT DEFINED RIME_DATA_DIR)
  find_package(RimeData REQUIRED)
endif(NOT DEFINED RIME_DATA_DIR)
message(STATUS "Precompiler macro RIME_DATA_DIR is set to \"${RIME_DATA_DIR}\"")
add_definitions(-DRIME_DATA_DIR="${RIME_DATA_DIR}")
add_definitions(-DFCITX_GETTEXT_DOMAIN=\"fcitx5-{{{lowername}}}\")
add_definitions(-DFCITX_{{{uppername}}}_VERSION=\"${PROJECT_VERSION}\")
fcitx5_add_i18n_definition()

include("${FCITX_INSTALL_CMAKECONFIG_DIR}/Fcitx5Utils/Fcitx5CompilerSettings.cmake")

add_subdirectory(po)
add_subdirectory(src)
add_subdirectory(data)

fcitx5_translate_desktop_file(org.fcitx.Fcitx5.Addon.{{{name}}}.metainfo.xml.in
                              org.fcitx.Fcitx5.Addon.{{{name}}}.metainfo.xml XML)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/org.fcitx.Fcitx5.Addon.{{{name}}}.metainfo.xml" DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

