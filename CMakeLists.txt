cmake_minimum_required(VERSION 3.14)
project(cu_submitter)

add_subdirectory(deps)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(AUTOGEN_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(PROJECT_SOURCES
    src/cu_submitter.cpp
    src/chgen/chgen.cpp src/chgen/chgen.h
    src/data/changelog.cpp src/data/changelog.h
    src/utils/error.cpp src/utils/error.h
    src/utils/log.cpp src/utils/log.h
    src/utils/print.cpp src/utils/print.h
    src/transfer/transfer.cpp src/transfer/transfer.h
    src/utils/utils.cpp src/utils/utils.h
    src/submit/submit.cpp src/submit/submit.h
    src/gui/cusubmittergui.cpp src/gui/cusubmittergui.h
)

set(APP_RESOURCES
    src/gui/CUSubmitterGUI.ui
)

add_executable(cu_submitter
    ${PROJECT_SOURCES}
    ${APP_RESOURCES}
)

find_package(Qt6 COMPONENTS
    Core Widgets
    REQUIRED)

target_link_libraries(cu_submitter
    lcf
    Qt::Core
    Qt::Widgets
)

set_target_properties(cu_submitter PROPERTIES
    WIN32_EXECUTABLE TRUE
)
