
set(OGG_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libogg/include PARENT_SCOPE)
include_directories(${CMAKE_SOURCE_DIR}/libogg/include)


set(ogg_LIB_SRCS src/framing.c src/bitwise.c)
if(WIN32)
	set(ogg_LIB_SRCS ${ogg_LIB_SRCS} src/ogg.def)
endif(WIN32)
add_library(ogg SHARED ${ogg_LIB_SRCS})
set_target_properties(ogg PROPERTIES VERSION 1.2.2 SOVERSION 1)
set_target_properties(ogg PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR})
#install(TARGETS ogg ${INSTALL_TARGETS_DEFAULT_ARGS})

if(MSVC_IDE) # грязнохаки для студии
	set_target_properties(ogg PROPERTIES PREFIX "../")
endif()

set(OGG_LIBRARIES ogg PARENT_SCOPE)

