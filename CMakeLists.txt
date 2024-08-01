cmake_minimum_required(VERSION 3.20)

set(PROJECT Streemer)

set (CMAKE_CXX_STANDARD 17)

project(${PROJECT})

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

include_directories(
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/include
)

# C++ executables

pkg_check_modules(RDKAFKA_CPP REQUIRED rdkafka++)

list(APPEND CPP_SOURCES
  ${CMAKE_SOURCE_DIR}/src/IOQueue.cpp
  ${CMAKE_SOURCE_DIR}/src/KafkaStream.cpp
)

list(APPEND CPP_INCLUDES
  ${RDKAFKA_CPP_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

list(APPEND CPP_LIBS
  ${RDKAFKA_CPP_LDFLAGS}
)

add_executable(producer
  ${PROJECT_SOURCE_DIR}/src/main.cpp
  ${CPP_SOURCES}
)
target_include_directories(producer PUBLIC ${CPP_INCLUDES})
target_link_libraries(producer ${CMAKE_THREAD_LIBS_INIT} ${CPP_LIBS})

target_compile_options(producer PUBLIC "-g3")
