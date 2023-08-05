# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)
set(EXTRA_COMPONENT_DIRS    ${CMAKE_CURRENT_SOURCE_DIR}/component/esp-idf-lib/components
                            ${CMAKE_CURRENT_SOURCE_DIR}/component/esp-idf-sx127x/components)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(FW_MAJOR 1)
set(FW_MINOR 0)
set(FW_BUILD 0)
set(PROJECT_VER_NUMBER 100)
set(FW_NAME "AIRFirmware_V${PROJECT_VER_NUMBER}")
set(PROJECT_VER "${FW_MAJOR}.${FW_MINOR}.${FW_BUILD}")
set(PROJECT_NAME "${FW_NAME}")

add_definitions(-DFW_VERSION_MAJOR=${FW_MAJOR})
add_definitions(-DFW_VERSION_MINOR=${FW_MINOR})
add_definitions(-DFW_VERSION_BUILD=${FW_BUILD})
add_definitions(-DFW_VERSION=${PROJECT_VER_NUMBER})

project(${PROJECT_NAME})

target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "main/CERT/AmazonRootCA1.pem" TEXT)
target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "main/CERT/certificate.pem.crt" TEXT)
target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "main/CERT/private.pem.key" TEXT)

idf_build_set_property(COMPILE_OPTIONS "-Wno-format;" APPEND)