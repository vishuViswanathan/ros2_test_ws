cmake_minimum_required(VERSION 3.8)
project(basic_cpp)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(serial REQUIRED)


add_executable(basic_cpp src/basic.cpp)
add_executable(tcp_client src/tcp_client.cpp)
add_executable(tcp_server src/tcp_server.cpp)
add_executable(tcp_serial_exchange src/tcp_serial_exchange.cpp)
add_executable(talk_to_arduino src/talk_to_arduino.cpp)

target_include_directories(
  basic_cpp
  PRIVATE
  include
)
ament_target_dependencies(
  tcp_serial_exchange 
  rclcpp 
  serial
  )

ament_target_dependencies(
    basic_cpp 
    rclcpp 
    serial
    )
  
ament_target_dependencies(
    talk_to_arduino
    rclcpp 
    serial
  )
    
  install(TARGETS
  basic_cpp
  tcp_client
  tcp_server
  tcp_serial_exchange
  talk_to_arduino
  DESTINATION lib/${PROJECT_NAME})  

ament_package()