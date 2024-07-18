cmake_minimum_required(VERSION 3.20)

set(PROJECT Streemer)
project(${PROJECT})

find_package(PkgConfig REQUIRED)

include_directories(
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/include
)

# C executables

pkg_check_modules(RDKAFKA_C REQUIRED rdkafka)
pkg_check_modules(GLIB REQUIRED glib-2.0)

list(APPEND C_INCLUDES
  ${GLIB_INCLUDE_DIRS}
  ${RDKAFKA_C_INCLUDE_DIRS}
)

list(APPEND C_LIBS
  ${GLIB_LDFLAGS}
  ${RDKAFKA_LDFLAGS} 
)

# Consumer example build config

add_executable(consumer ${PROJECT_SOURCE_DIR}/src/consumer.c)
target_include_directories(consumer PUBLIC ${C_INCLUDES})
target_link_libraries(consumer ${C_LIBS})

# Producer example build config

add_executable(producer ${PROJECT_SOURCE_DIR}/src/producer.c)
target_include_directories(producer PUBLIC ${C_INCLUDES})
target_link_libraries(producer ${C_LIBS})

# C++ executables

pkg_check_modules(RDKAFKA_CPP REQUIRED rdkafka++)

list(APPEND CPP_INCLUDES
  ${RDKAFKA_CPP_INCLUDE_DIRS}
)