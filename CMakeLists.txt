CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

SET( CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_SOURCE_DIR}/cmakeModules"
	)

find_package(SDL 2 REQUIRED)
find_package(SDL2_image REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

FILE(GLOB_RECURSE includes ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
FILE(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_executable(chess ${sources})
target_link_libraries(chess ${SDL_LIBRARIES} ${SDL_IMAGE_LIBRARIES})