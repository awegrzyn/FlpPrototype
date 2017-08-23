find_package(Boost COMPONENTS unit_test_framework program_options REQUIRED)
find_package(Configuration REQUIRED)
find_package(Monitoring REQUIRED)
find_package(FairRoot)
find_package(InfoLogger REQUIRED)
find_package(Common REQUIRED)

if (FAIRROOT_FOUND)
    link_directories(${FAIRROOT_LIBRARY_DIR})
    include_directories(${FAIRROOT_INCLUDE_DIR})
else (FAIRROOT_FOUND)
    message(WARNING "FairRoot not found, corresponding classes will not be compiled.")
endif (FAIRROOT_FOUND)

find_package(ROOT 6.06.02 COMPONENTS RHTTP RMySQL Gui)
if (ROOT_FOUND)
    message(STATUS "ROOT ${ROOT_VERSION} found")
else ()
    return()
endif ()

# todo just a quick fix to get the dictionary working . to be revisited when extracting
list(APPEND GLOBAL_ALL_MODULES_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/../QualityControl/include)
list(APPEND GLOBAL_ALL_MODULES_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/../DataFormat/include)

o2_define_bucket(
    NAME
        o2_qcmodules_base

    DEPENDENCIES
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        QualityControl
        ${Monitoring_LIBRARIES}
        ${InfoLogger_LIBRARIES}
        ${Common_LIBRARIES}

    SYSTEMINCLUDE_DIRECTORIES
        ${Boost_INCLUDE_DIRS}
        ${Monitoring_INCLUDE_DIRS}
        ${InfoLogger_INCLUDE_DIRS}
        ${Common_INCLUDE_DIRS}
)

o2_define_bucket(
    NAME
        o2_qcmodules_common

    DEPENDENCIES
        o2_qcmodules_base
)

o2_define_bucket(
    NAME
        o2_qcmodules_example

    DEPENDENCIES
        o2_qcmodules_base

    SYSTEMINCLUDE_DIRECTORIES
        ${Boost_INCLUDE_DIRS}
        ${Configuration_INCLUDE_DIRS}
)