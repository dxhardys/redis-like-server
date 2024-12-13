cmake_minimum_required(VERSION 3.12)
project(redis-server)

set(CMAKE_C_STANDARD 99)

add_executable(server server.c src/hashmap.c src/list.c)
target_compile_options(server PRIVATE "-std=c99" "-O3")
target_link_libraries(server pthread cjson)
