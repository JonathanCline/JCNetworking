
set(lib_name ${PROJECT_NAME})
set(lib_cxx 20)





set(lib_link_public

)
set(lib_link_private 
	SAELib
)


set(lib_include_public
	"include"
)
set(lib_include_private 
	"source"
)


add_library(${lib_name} STATIC "include/${lib_name}.h" "include/${lib_name}Bk.h" "include/${lib_name}Config.h" "include/${lib_name}SockLib.h" "source/${lib_name}.cpp" "source/${lib_name}Bk.cpp")

target_include_directories(${lib_name} PUBLIC ${lib_include_public} PRIVATE ${lib_include_private})
target_link_libraries(${lib_name} PUBLIC "${lib_link_public}" PRIVATE "${lib_link_private}")

target_compile_options(${lib_name} PRIVATE "/wd26812")

set_target_properties(${lib_name} PROPERTIES CXX_STANDARD ${lib_cxx} CXX_STANDARD_REQUIRED True)


add_subdirectory("source/SockLib")
add_subdirectory("source/Socket")
add_subdirectory("source/Error")
add_subdirectory("source/JCNContext")
add_subdirectory("source/Thread")
add_subdirectory("source/Utility")





