cmake_minimum_required(VERSION 3.8)

project(JCNetworking)

set(lib_cxx 20)
set(lib_dir "lib")


add_subdirectory("JCNetworking")









find_package(Git QUIET)

macro(ADD_GIT_DEPENDENCY depLibraryDir depTarget depName githubURL)
if(NOT TARGET ${depTarget})
	execute_process(COMMAND 
		${GIT_EXECUTABLE} clone ${githubURL} ${depLibraryDir}/${depName}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		RESULT_VARIABLE GIT_SUBMOD_RESULT)	
	add_subdirectory("${depLibraryDir}/${depName}")	
endif()
endmacro()

macro(ADD_GIT_DEPENDENCY_NO_CMAKE depLibraryDir depTarget depName githubURL)
if(NOT TARGET ${depTarget})
	execute_process(COMMAND 
		${GIT_EXECUTABLE} clone ${githubURL} ${depLibraryDir}/${depName}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		RESULT_VARIABLE GIT_SUBMOD_RESULT)
endif()
endmacro()


#
#	SAELib		https://github.com/JonathanCline/SAELib
#

ADD_GIT_DEPENDENCY("${lib_dir}" SAELib SAELib "https://github.com/JonathanCline/SAELib")







