cmake_minimum_required(VERSION 2.8...3.13)

project(app)

find_package(OpenSSL REQUIRED)
find_package(websocketpp REQUIRED)

add_executable(
    app
    app.cpp
    serialib.cpp
)

target_include_directories(
    app
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(app PRIVATE OpenSSL::SSL websocketpp::websocketpp)
