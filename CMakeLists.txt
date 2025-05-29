cmake_minimum_required(VERSION 3.16)
project(KriusIDE VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt5 components
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network)

# Set up Qt5
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/editor/codeeditor.cpp
    src/editor/syntaxhighlighter.cpp
    src/bagel/bagelclient.cpp
    src/bagel/bagelchatwidget.cpp
    src/project/projectmanager.cpp
    src/ui/sidebar.cpp
    src/ui/statusbar.cpp
)

# Header files
set(HEADERS
    src/mainwindow.h
    src/editor/codeeditor.h
    src/editor/syntaxhighlighter.h
    src/bagel/bagelclient.h
    src/bagel/bagelchatwidget.h
    src/project/projectmanager.h
    src/ui/sidebar.h
    src/ui/statusbar.h
)

# Resources
set(RESOURCES
    resources/resources.qrc
)

# Create executable
add_executable(KriusIDE ${SOURCES} ${HEADERS} ${RESOURCES})

# Link Qt5 libraries
target_link_libraries(KriusIDE 
    Qt5::Core 
    Qt5::Widgets 
    Qt5::Network
)

# Set output directory
set_target_properties(KriusIDE PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Install target
install(TARGETS KriusIDE DESTINATION bin)