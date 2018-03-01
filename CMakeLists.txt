CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

SET( CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake"
	)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

FILE(GLOB_RECURSE includes ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
FILE(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_executable(chess ${sources})
target_link_libraries(chess ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES}
	${SDL2_TTF_LIBRARIES})