cmake_minimum_required(VERSION 3.11)

project(
    ExternalMemory
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Werror)

add_subdirectory(xmem)

add_subdirectory(homework)
