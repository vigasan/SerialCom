cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME "SerialCom")

project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Qml Quick QuickControls2 SerialPort REQUIRED)

add_executable(${PROJECT_NAME}
                frame.cpp 
                frameprocessor.cpp 
                main.cpp 
                serialworker.cpp
                qml.qrc
)

target_link_libraries(${PROJECT_NAME}
                      Qt5::Core
                      Qt5::Gui
                      Qt5::Qml
                      Qt5::Quick
                      Qt5::QuickControls2
                      Qt5::SerialPort
)

set_target_properties(${PROJECT_NAME} 
                      PROPERTIES 
                      CXX_STANDARD 11 
                      CXX_STANDARD_REQUIRED ON)
