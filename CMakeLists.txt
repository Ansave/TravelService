cmake_minimum_required(VERSION 3.2)

project(hl_server C CXX)

SET (EXAMPLE_BINARY "travelservice")
set (WRITER_MAIN "writer_main")
set (STD_CXX "c++17")
set (REDISCPP_FLAGS "-DREDISCPP_HEADER_ONLY=ON")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -std=${STD_CXX} ${REDISCPP_FLAGS}")
set (CMAKE_CXX_FLAGS_RELEASE "-O3 -g0 -std=${STD_CXX} -Wall -DNDEBUG ${REDISCPP_FLAGS}")

find_package(OpenSSL)
find_package(Threads)
find_package(ZLIB)
find_package(CppKafka REQUIRED)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL Data)

if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})

add_executable(${EXAMPLE_BINARY} main.cpp 
                                 user_server/config/config.cpp 
                                 data_cache/cache.cpp
                                 user_server/database/database.cpp
                                 user_server/database/user.cpp
                                 route_server/database/route.cpp
                                 ride_server/database/ride.cpp)

add_executable(load_data load_data.cpp)

add_executable(${WRITER_MAIN} writer_main.cpp
                              user_server/config/config.cpp
                              user_server/database/database.cpp
                              data_cache/cache.cpp
                              user_server/database/user.cpp)

target_include_directories(${WRITER_MAIN} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${WRITER_MAIN} PRIVATE -Wall -Wextra -pedantic -Werror )

target_include_directories(${EXAMPLE_BINARY} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${EXAMPLE_BINARY} PRIVATE -Wall -Wextra -pedantic -Werror )

target_link_libraries(${EXAMPLE_BINARY} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"                      
                             CppKafka::cppkafka
                             ZLIB::ZLIB)

target_link_libraries(load_data PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"                      
                             ZLIB::ZLIB)

target_link_libraries(${WRITER_MAIN} PRIVATE
                             ${CMAKE_THREAD_LIBS_INIT}
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "rdkafka++"
                             CppKafka::cppkafka
                             "mysqlclient"
                             ZLIB::ZLIB)

set_target_properties(${EXAMPLE_BINARY} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${EXAMPLE_BINARY} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

set_target_properties(${WRITER_MAIN} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${WRITER_MAIN} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
