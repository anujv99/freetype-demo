add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glad)
add_subdirectory(vendor/freetype)
add_subdirectory(vendor/imgui)

file(GLOB_RECURSE engine_source_files "src/*.*")

add_library(Engine STATIC ${engine_source_files})

target_include_directories(Engine PRIVATE "src")
target_include_directories(Engine PRIVATE "vendor/glfw/include/")
target_include_directories(Engine PRIVATE "vendor/glad/include")
target_include_directories(Engine PRIVATE "vendor/freetype/include")
target_include_directories(Engine PRIVATE "vendor/imgui")

target_include_directories(Engine PUBLIC "vendor/prevmath/include")

target_link_libraries(Engine PRIVATE glfw)
target_link_libraries(Engine PRIVATE glad)
target_link_libraries(Engine PRIVATE freetype)
target_link_libraries(Engine PRIVATE imgui)

option(LOGGING "Enable/Disable Logging" TRUE)
option(ASSERT "Enable/Disable Assers" TRUE)
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

if (WIN32)
	add_compile_definitions(__WINDOWS__)
	target_link_libraries(Engine PRIVATE opengl32)
elseif(UNIX)
	add_compile_definitions(__UNIX__)
	target_link_libraries(Engine PRIVATE GL)
else()
	message(FATAL_ERROR "Only support windows and unix operating system")
endif()

add_compile_definitions(__OPENGL__)	# may add directx later
add_compile_definitions(__GLFW__)