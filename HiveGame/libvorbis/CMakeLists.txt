
set(VORBISFILE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libvorbis/include PARENT_SCOPE)
include_directories(${CMAKE_SOURCE_DIR}/libvorbis/include ${CMAKE_SOURCE_DIR}/libvorbis/lib ${OGG_INCLUDE_DIRS})


set(vorbis_LIB_SRCS
    lib/mdct.c
    lib/smallft.c
    lib/block.c
    lib/envelope.c
    lib/window.c
    lib/lsp.c
    lib/lpc.c
    lib/analysis.c
    lib/synthesis.c
    lib/psy.c
    lib/info.c
    lib/floor1.c
    lib/floor0.c
    lib/res0.c
    lib/mapping0.c
    lib/registry.c
    lib/codebook.c
    lib/sharedbook.c
    lib/lookup.c
    lib/bitrate.c
    lib/envelope.h
    lib/lpc.h
    lib/lsp.h
    lib/codebook.h
    lib/misc.h
    lib/psy.h
    lib/masking.h
    lib/os.h
    lib/mdct.h
    lib/smallft.h
    lib/highlevel.h
    lib/registry.h
    lib/scales.h
    lib/window.h
    lib/lookup.h
    lib/lookup_data.h
    lib/codec_internal.h
    lib/backends.h
    lib/bitrate.h)
if(WIN32)
	set(vorbis_LIB_SRCS ${vorbis_LIB_SRCS} lib/vorbis.def)
endif(WIN32)
add_library(vorbis SHARED ${vorbis_LIB_SRCS})
set_target_properties(vorbis PROPERTIES VERSION 1.3.2 SOVERSION 1)
set_target_properties(vorbis PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR})
target_link_libraries(vorbis vorbisenc ogg)
#install(TARGETS vorbis ${INSTALL_TARGETS_DEFAULT_ARGS})


set(vorbisfile_LIB_SRCS lib/vorbisfile.c)
if(WIN32)
	set(vorbisfile_LIB_SRCS ${vorbisfile_LIB_SRCS} lib/vorbisfile.def)
endif(WIN32)
add_library(vorbisfile SHARED ${vorbisfile_LIB_SRCS})
target_link_libraries(vorbisfile vorbis)
set_target_properties(vorbisfile PROPERTIES VERSION 1.3.2 SOVERSION 1)
set_target_properties(vorbisfile PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR})
#install(TARGETS vorbisfile ${INSTALL_TARGETS_DEFAULT_ARGS})


set(vorbisenc_LIB_SRCS lib/vorbisenc.c)
add_library(vorbisenc STATIC ${vorbisenc_LIB_SRCS})
#install(TARGETS vorbisenc ${INSTALL_TARGETS_DEFAULT_ARGS})


#set(barkmel_SRCS barkmel.c)
#add_executable(barkmel ${barkmel_SRCS})


#set(tone_SRCS tone.c)
#add_executable(tone ${tone_SRCS})
#target_link_libraries(tone vorbis)


#set(psytune_SRCS psytune.c)
#add_executable(psytune ${psytune_SRCS})
#target_link_libraries(psytune vorbis)


if(MSVC_IDE) # грязнохаки для студии
	set_target_properties(vorbis PROPERTIES PREFIX "../")
	set_target_properties(vorbisfile PROPERTIES PREFIX "../")
endif()


set(VORBISFILE_LIBRARIES vorbis vorbisfile vorbisenc PARENT_SCOPE)
