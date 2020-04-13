## MSDFGEN

add_definitions(-DMSDFGEN_USE_CPP11)

file(GLOB_RECURSE msdfgen_HEADERS
	"msdfgen/core/*.h"
	"msdfgen/core/*.hpp"
	"msdfgen/lib/*.h"
	"msdfgen/ext/*.h"
	"msdfgen/include/*.h"
)

file(GLOB_RECURSE msdfgen_SOURCES
	"msdfgen/core/*.cpp"
	"msdfgen/lib/*.cpp"
	"msdfgen/ext/*.cpp"
)

add_library(lib_msdfgen ${msdfgen_SOURCES} ${msdfgen_HEADERS})

target_include_directories(lib_msdfgen PRIVATE "msdfgen/include")
target_include_directories(lib_msdfgen PRIVATE "../engine/vendor/freetype/include")

## Demo

file(GLOB_RECURSE paint_source_files "src/*.*")

add_executable(ftinspect ${paint_source_files})

target_include_directories(ftinspect PRIVATE "src")
target_include_directories(ftinspect PRIVATE "../engine/src")
target_include_directories(ftinspect PRIVATE "../engine/vendor/freetype/include")
target_include_directories(ftinspect PRIVATE "../engine/vendor/prevmath/include")
target_include_directories(ftinspect PRIVATE "../engine/vendor/imgui")
target_include_directories(ftinspect PRIVATE "msdfgen")

target_link_libraries(ftinspect PRIVATE Engine)
target_link_libraries(ftinspect PRIVATE lib_msdfgen)

option(LOGGING "Enable/Disable Logging" TRUE)
option(ASSERT "Enable/Disable Assertions" TRUE)
option(UNICODE "Enable/Disable Unicode" FALSE)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_definitions(__DEBUG__)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
	add_compile_definitions(__RELEASE__)
else()
	message(FATAL_ERROR "Undefined Configuration")
endif()

if (LOGGING)
	add_compile_definitions(__LOGGING__)
endif()

if (ASSERT)
	add_compile_definitions(__ASSERT__)
endif()

if (UNICODE)
	add_compile_definitions(__UNICODE__)
endif()
