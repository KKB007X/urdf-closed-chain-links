cmake_minimum_required(VERSION 3.8)
project(clg_plugins)

find_package(ament_cmake REQUIRED)

find_package(gz-plugin2 REQUIRED)
find_package(gz-sim8 REQUIRED)

add_library(spring_plugin SHARED
  src/spring_plugin.cpp
)

target_link_libraries(spring_plugin
  gz-plugin2::gz-plugin2
  gz-sim8::gz-sim8
)

install(
  TARGETS spring_plugin
  LIBRARY DESTINATION lib
)

ament_package()
