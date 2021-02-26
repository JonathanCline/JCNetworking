
set(lib_name ${PROJECT_NAME})
set(lib_cxx 20)





set(lib_link_public
	SAELib
)
set(lib_link_private 
	
)


set(lib_include_public
	"include"
)
set(lib_include_private 
	"source"
)




add_library(${lib_name} STATIC "include/${lib_name}.h" "source/${lib_name}.cpp")

set_target_properties(${lib_name} PROPERTIES CXX_STANDARD ${lib_cxx} CXX_STANDARD_REQUIRED True)

target_include_directories(${lib_name} PUBLIC "${lib_include_public}" PRIVATE "${lib_include_private}")

target_link_libraries(${lib_name} PUBLIC "${lib_link_public}" PRIVATE "${lib_link_private}")





