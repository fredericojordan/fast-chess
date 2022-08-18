project(fast-chess)
CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(chess ${CMAKE_CURRENT_SOURCE_DIR}/src/fast-chess.c)

# FILE(GLOB_RECURSE includes ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
# FILE(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/src/fast-chess.c)
#
# SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
# find_package(SDL2 REQUIRED)
# find_package(SDL2_image REQUIRED)
# find_package(SDL2_ttf REQUIRED)
#
# include_directories(${SDL2_INCLUDE_DIRS})
# link_directories(${CMAKE_SOURCE_DIR}/lib)
#
# add_executable(chess-gui ${CMAKE_CURRENT_SOURCE_DIR}/src/fast-chess-gui.c)
# target_link_libraries(chess-gui ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES})
# target_link_libraries(chess-gui SDL2-2.0.0)
